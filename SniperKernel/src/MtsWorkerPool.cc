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

#include "SniperKernel/MtsWorkerPool.h"
#include "SniperKernel/MtsMicroTaskQueue.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/MtSniperContext.h"

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
    m_thrd = new std::thread(&MtsWorker::run, this);
}

void MtsWorker::run()
{
    // initialize the necessary pointers statically
    static MtsMicroTaskQueue *taskQueue = MtsMicroTaskQueue::instance();
    static MtsWorkerPool *workerPool = MtsWorkerPool::instance();
    static auto& globalSyncAssistant = mt_sniper_context->global_sync_assistant;

    LogInfo << "start worker " << m_name << std::endl;

    // loop the micro tasks in the queue until ...
    while (globalSyncAssistant.active())
    {
        switch (taskQueue->concurrentPop()->run())
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
            globalSyncAssistant.pause(); // wait for a global signal
            LogDebug << "Wakeup and continue..." << std::endl;
            continue; // the worker wakes up and continue the loop
        case MtsMicroTask::Status::NoMoreEvt:
            LogInfo << "NoMoreEvt, endup the worker" << std::endl;
            globalSyncAssistant.notifyAll(); // wakeup any paused workers so it can finish itself
            workerPool->notifyEndUp(this);
            return;
        default:  //Failed
            LogError << "Failed to exec a MicroTask, endup all workers" << std::endl;
            globalSyncAssistant.deactive();
            globalSyncAssistant.notifyAll(); // wakeup any paused workers so it can finish itself
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
}

MtsWorker *MtsWorkerPool::create()
{
    auto w = new MtsWorker();
    return w;
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
        // FIXME: rare case when resume() before pause(), it fails
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
        m_sync.notifyOne();
    }
}

void MtsWorkerPool::waitAll(unsigned int nWorkers)
{
    m_nAliveWorkers = nWorkers;
    m_sync.pause();

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
