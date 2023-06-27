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
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/MtsMicroTaskQueue.h"
#include "SniperPrivate/MtsWorkerPool.h"

std::atomic_int MtsWorker::s_id{0};

MtsWorker::MtsWorker()
    : NamedElement("MtSniper:", "Worker"),
      m_thrd(nullptr)
{
    m_name += std::to_string(s_id++);
    LogDebug << "construct MtsWorker " << m_name << std::endl;
}

MtsWorker::~MtsWorker()
{
    LogDebug << "destruct MtsWorker " << m_name << std::endl;
}

void MtsWorker::start()
{
    m_thrd = new std::thread(&MtsWorker::run, this);
}

void MtsWorker::run()
{
    // initialize the necessary pointers statically
    static MtsMicroTaskQueue *taskQueue = MtsMicroTaskQueue::instance();
    static MtsWorkerPool *workerPool = MtsWorkerPool::instance();

    LogDebug << "start worker " << m_name << std::endl;

    // loop the micro tasks in the queue until ...
    while (taskQueue->concurrentPop()->exec() == 0)
        continue;

    // put self back to the pool for reusing
    workerPool->put(this);
}

void MtsWorker::wait()
{
    if (m_thrd != nullptr)
    {
        m_thrd->join();
        delete m_thrd;
        m_thrd = nullptr;
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
    : m_allWorkers(nullptr)
{
}

MtsWorker *MtsWorkerPool::get()
{
    auto w = this->allocate();
    if (w == nullptr)
    {
        w = new MtsWorker();
        m_allWorkers.concurrentPush(w);
    }
    return w;
}

void MtsWorkerPool::waitAll()
{
    auto worker = m_allWorkers.concurrentPop();
    while (worker != nullptr)
    {
        worker->wait();
        worker = m_allWorkers.concurrentPop();
    }
}
