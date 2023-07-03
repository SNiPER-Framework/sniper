/* Copyright (C) 2023
   Institute of High Energy Physics and Shandong University
   This file is part of SNiPER.

   SNiPER is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   SNiPER is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with SNiPER.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef SNIPER_MTS_INCUBATOR_H
#define SNIPER_MTS_INCUBATOR_H

#include "SniperKernel/MtsMicroTaskQueue.h"
#include "SniperKernel/MtsWorkerPool.h"
#include "SniperKernel/SniperException.h"
#include <type_traits>

template <typename T>
class MtsIncubator : public MtsIncubatorBase
{
public:
    static_assert(std::is_base_of<MtsMicroTask, T>::value, "Non MtsMicroTask inherited type");

    MtsIncubator();
    ~MtsIncubator();

    T *allocate();
    void wait();

    void cleanup();

private:
    friend class MtsMicroTask;
    virtual bool notify(MtsMicroTask *egg) override;

    SniperObjPool<T> *m_taskPool;
    MtsMicroTaskQueue *m_taskQueue;
    MtsWorkerPool *m_workerPool;

    std::atomic_int m_nEggs{0};
    Sniper::Queue<T *> m_eggs;

    MtsSyncAssistant m_sync;

    static std::atomic_int m_nIncubator;
};

template <typename T>
std::atomic_int MtsIncubator<T>::m_nIncubator{0};

template <typename T>
MtsIncubator<T>::MtsIncubator()
    : m_eggs(nullptr)
{
    m_taskPool = SniperObjPool<T>::instance([]()
                                        { return new T(); });
    m_taskQueue = MtsMicroTaskQueue::instance();
    m_workerPool = MtsWorkerPool::instance();
    ++m_nIncubator;
}

template <typename T>
MtsIncubator<T>::~MtsIncubator()
{
}

template <typename T>
T *MtsIncubator<T>::allocate()
{
    T *egg = m_taskPool->secureAllocate();
    egg->setIncubator(this);
    m_eggs.push(egg); //concurrently allocate is unnecessary
    ++m_nEggs;
    return egg;
}

template <typename T>
void MtsIncubator<T>::wait()
{
    if (m_nEggs > 1)
    {
        m_taskQueue->concurrentMerge(m_eggs);
        m_workerPool->pop(); // wakeup another worker
        m_sync.pause();      // pause current thread and wait
    }
    else if (m_nEggs == 1)
    {
        auto egg = m_eggs.pop();
        egg->exec();
        m_taskPool->deallocate(egg);
        m_nEggs = 0;
    }
}

template <typename T>
bool MtsIncubator<T>::notify(MtsMicroTask *egg)
{
    auto _egg = dynamic_cast<T *>(egg);
    if (_egg != nullptr)
    {
        m_taskPool->deallocate(_egg);
        if (--m_nEggs == 0)
        {
            m_sync.notifyOne();
            return true;
        }
        return false;
    }
    throw ContextMsgException("MtsIncubator::notify() receives a bad egg");
}

template <typename T>
void MtsIncubator<T>::cleanup()
{
    if (--m_nIncubator == 0)
        m_taskPool->destroy();
}

#endif