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

MtsWorker::MtsWorker(bool isThreadHandle)
    : NamedElement("MtSniper:", "Worker"),
      m_isThreadHandle(isThreadHandle)
{
    m_name += std::to_string(++s_id);

    LogDebug << "construct MtsWorker " << m_name << std::endl;
    m_pool = MtsWorkerPool::instance();
}

MtsWorker::~MtsWorker()
{
    LogDebug << "destruct MtsWorker " << m_name << std::endl;
}

void MtsWorker::initContext()
{
    if (getcontext(&m_ctx) == 0)
    {
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = sizeof(m_stack);
        makecontext(&m_ctx, reinterpret_cast<void (*)()>(&MtsWorker::sRun), 1, this);
    }
    else
    {
        LogFatal << "getcontext() failed" << std::endl;
    }
}

void MtsWorker::run()
{
    // initialize the necessary pointers statically
    static MtsMicroTaskQueue *taskQueue = MtsMicroTaskQueue::instance();

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
            yield(MtSniperUtil::Worker::getIncubatorContext());
            LogDebug << "Wakeup and continue..." << std::endl;
            m_active = true;
            continue; // the worker wakes up and continue the loop
        case MtsMicroTask::Status::NoTask:
            LogDebug << "NoTask and waiting..." << std::endl;
            MtSniperUtil::Thread::pause(); // pause current thread and wait for a global signal
            LogDebug << "Wakeup and continue..." << std::endl;
            continue; // the worker wakes up and continue the loop
        case MtsMicroTask::Status::NoMoreEvt:
            LogInfo << "NoMoreEvt, endup the worker" << std::endl;
            MtSniperUtil::Thread::resumeAll(); // wakeup any paused workers then finish itself
            m_pool->syncEndUp(this);
            return;
        default:  //Failed
            LogError << "Failed to exec a MicroTask, endup all workers" << std::endl;
            MtSniperUtil::MtSniperStatus = false;
            MtSniperUtil::Thread::resumeAll(); // wakeup any paused workers then finish itself
            m_pool->syncEndUp(this);
            return;
        }
    }

    m_pool->syncEndUp(this);
}

void MtsWorker::yield(ucontext_t *ctx)
{
    getcontext(&m_ctx);
    if (m_active)
    {
        m_active = false;
        m_pool->deallocate(this);
        setcontext(ctx);
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

MtsWorkerPool::~MtsWorkerPool()
{
    // release the workers inside the Pool
    while (auto worker = this->allocate())
    {
        delete worker;
    }

    // release the workers outside the Pool
    while (auto worker = m_freeWorkers.pop())
    {
        delete worker;
    }
}

void MtsWorkerPool::spawn(int n)
{
    m_nAlive = n;
    for (int i = 0; i < n; ++i)
    {
        m_threads.push(new std::thread(&MtsWorker::run, new MtsWorker(true)));
    }
}

void MtsWorkerPool::syncEndUp(MtsWorker *worker)
{
    m_freeWorkers.concurrentPush(worker);

    if (worker->isThreadHandle())
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (--m_nAlive == 0)
        {
            m_sync.notify_all();
        }
        return;
    }

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (--m_nAlive != 0)
        {
            m_sync.wait(lock);
        }
        else
        {
            m_sync.notify_all();
        }
    }

    while (auto w = this->allocate())
    {
        if (w->isThreadHandle())
        {
            // all threads have to end up on a different thread handle, or else join() will fail
            // but it unnecessary to end up on its original thread handle in our case
            w->resume();
        }
        m_freeWorkers.concurrentPush(w);
    }
}

void MtsWorkerPool::waitAll()
{
    while (auto thrd = m_threads.pop())
    {
        thrd->join();
        delete thrd;
    }
}
