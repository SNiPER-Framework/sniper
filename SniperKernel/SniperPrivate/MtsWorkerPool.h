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

#include "SniperKernel/SniperQueue.h"
#include "SniperKernel/NamedElement.h"
#include <thread>
#include <atomic>
#include <ucontext.h>

class MtsWorkerPool;

class MtsWorker final : public NamedElement
{
public:
    MtsWorker();
    ~MtsWorker();

    ucontext_t *context() { return &m_ctx; }
    void initContext();

    void run();
    void yield();
    void resume();

private:
    bool m_active{true};
    MtsWorkerPool *m_pool;
    ucontext_t m_ctx;
    char m_stack[1024*4];

    static std::atomic_int s_id;
};

class MtsWorkerPool final : public SniperObjPool<MtsWorker>
{
public:
    static MtsWorkerPool *instance();
    static void destroy() { SniperObjPool<MtsWorker>::destroy(); }

    void spawn(int n);
    void notifyEndUp(MtsWorker *worker);
    void waitAll();

private:
    Sniper::Queue<std::thread *> m_threads{nullptr};
    Sniper::Queue<MtsWorker *> m_freeWorkers{nullptr};

    MtsWorkerPool();
    virtual ~MtsWorkerPool();

    static MtsWorker *creator();
};

#endif