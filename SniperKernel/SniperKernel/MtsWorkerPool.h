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

#ifndef SNIPER_MTS_WORKER_H
#define SNIPER_MTS_WORKER_H

#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/SniperQueue.h"
#include "SniperKernel/MtsSyncAssistant.h"
#include "SniperKernel/NamedElement.h"
#include <thread>
#include <atomic>

class MtsWorkerPool;

class MtsWorker final : public NamedElement
{
public:
    MtsWorker();
    ~MtsWorker();

    void start();
    void run();
    void wait();

    MtsSyncAssistant &syncAssistant() { return m_sync; }

private:
    std::thread *m_thrd;
    MtsSyncAssistant m_sync;

    static std::atomic_int s_id;
};

class MtsWorkerPool final : private SniperObjPool<MtsWorker>
{
public:
    static MtsWorkerPool *instance();
    static void destroy() { SniperObjPool<MtsWorker>::destroy(); }

    MtsWorker* create();
    void push(MtsWorker *worker);
    void pop();
    void waitAll();

private:
    std::atomic_flag m_lock{ATOMIC_FLAG_INIT};
    Sniper::Queue<MtsWorker*> m_allWorkers;

    MtsWorkerPool();
    virtual ~MtsWorkerPool() = default;
};

#endif