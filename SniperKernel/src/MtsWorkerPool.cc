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

#include "SniperPrivate/MtsWorkerPool.h"
#include "SniperKernel/MtsMicroTaskQueue.h"
#include "SniperKernel/MtSniperUtility.h"
#include "SniperKernel/SniperLog.h"

std::atomic_int MtsWorker::s_id{0};

MtsWorker::MtsWorker()
    : NamedElement("MtSniper:", "Worker"),
      m_thrd(nullptr)
{
    m_name += std::to_string(++s_id);
    LogDebug << "construct MtsWorker " << m_name << std::endl;
}

MtsWorker::~MtsWorker()
{
    delete m_thrd;
    LogDebug << "destruct MtsWorker " << m_name << std::endl;
}

void MtsWorker::start()
{
    m_inUse = 1;
    m_thrd = new std::thread(&MtsWorker::run, this);
}

void MtsWorker::run()
{
    // initialize the necessary pointers statically
    static MtsMicroTaskQueue *taskQueue = MtsMicroTaskQueue::instance();
    static MtsWorkerPool *workerPool = MtsWorkerPool::instance();

    LogInfo << "start worker " << m_name << std::endl;

    // loop the micro tasks in the queue until ...
    while (MtSniperUtil::MtSniperStatus)
    {
        switch (taskQueue->dequeue()->run())
        {
        case MtsMicroTask::Status::OK:
            //LogDebug << "OK" << std::endl;
            continue; //continue the loop
        case MtsMicroTask::Status::BatchEnd:
            LogDebug << "Reach Batch end and waiting..." << std::endl;
            workerPool->push(this); // put self back to the pool and waiting for reusing
            LogDebug << "Wakeup and continue..." << std::endl;
            continue; // the worker wakes up and continue the loop
        case MtsMicroTask::Status::NoTask:
            LogDebug << "NoTask and waiting..." << std::endl;
            MtSniperUtil::Thread::pause(); // pause current thread and wait for a global signal
            LogDebug << "Wakeup and continue..." << std::endl;
            continue; // the worker wakes up and continue the loop
        case MtsMicroTask::Status::NoMoreEvt:
            LogInfo << "NoMoreEvt, endup the worker" << std::endl;
            MtSniperUtil::Thread::resumeAll(); // wakeup any paused workers then finish itself
            workerPool->notifyEndUp(this);
            return;
        default:  //Failed
            LogError << "Failed to exec a MicroTask, endup all workers" << std::endl;
            MtSniperUtil::MtSniperStatus = false;
            MtSniperUtil::Thread::resumeAll(); // wakeup any paused workers then finish itself
            workerPool->notifyEndUp(this);
            return;
        }
    }

    workerPool->notifyEndUp(this);
}

void MtsWorker::join()
{
    if (m_thrd != nullptr)
    {
        m_thrd->join();
    }
}

void MtsWorker::pause()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (--m_inUse == 0)
    {
        m_cv.wait(lock, [this]()
                  { return m_inUse != 0; });
    }
}

void MtsWorker::resume()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (++m_inUse == 1)
    {
        m_cv.notify_one();
    }
}

MtsWorkerPool *MtsWorkerPool::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new MtsWorkerPool();
    }
    return reinterpret_cast<MtsWorkerPool*>(s_instance);
}

MtsWorkerPool::MtsWorkerPool()
    : m_freeWorkers(nullptr)
{
    m_creator = []()
    { return new MtsWorker(); };
}

void MtsWorkerPool::spawn(int n)
{
    if (m_nAliveWorkers != 0)
    {
        LogError << "there are alive workers, don't spawn again!" << std::endl;
        return;
    }

    this->preAllocate(n);
    while (auto w = this->allocate())
    {
        w->start();
    }

    m_nAliveWorkers = n;
}

void MtsWorkerPool::push(MtsWorker *worker)
{
    {
        AtomicFlagLockGuard<true> guard(m_lock);
        if (m_top->up != nullptr)
        {
            m_top = m_top->up;
            m_top->value = worker;
        }
        else
        {
            m_top->up = new Slot(m_top, worker);
            m_top = m_top->up;
        }
        //release the lock before pause
    }
    worker->pause();
}

void MtsWorkerPool::pop()
{
    if (auto w = this->allocate(); w != nullptr)
    {
        w->resume();
    }
    else
    {
        w = new MtsWorker();
        w->start();
    }
}

void MtsWorkerPool::notifyEndUp(MtsWorker *worker)
{
    m_freeWorkers.concurrentPush(worker);
    if (--m_nAliveWorkers == 0)
    {
        m_ending.notify_one();
    }
}

void MtsWorkerPool::waitAll()
{
    {
        // in fact we don't need a lock here...
        std::mutex _local;
        std::unique_lock<std::mutex> _lock(_local);
        m_ending.wait(_lock);
    }

    // release the workers outside the Pool
    while (auto worker = m_freeWorkers.pop())
    {
        worker->join();
        delete worker;
    }

    // release the workers inside the Pool
    while (auto worker = this->allocate())
    {
        worker->resume();
        worker->join();
        delete worker;
    }
}
