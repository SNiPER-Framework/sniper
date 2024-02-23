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
#include <condition_variable>
#include <ucontext.h>

class MtsWorkerPool;

class MtsWorker final : public NamedElement
{
public:
    static void sRun(MtsWorker *p) { p->run(); }
    void initContext();
    void resume() { setcontext(&m_ctx); }

private:
    friend class MtsWorkerPool;
    friend class SniperObjPool<MtsWorker>;

    MtsWorker(bool isThreadHandle);
    ~MtsWorker();

    void run();
    void yield(ucontext_t *ctx); // yield the CPU and go to another context

    bool isThreadHandle() { return m_isThreadHandle; }

    bool m_isThreadHandle;
    bool m_active{true};
    MtsWorkerPool *m_pool;
    ucontext_t m_ctx;
    // ATTENTION: it may crash the program when the stack is not big enough in some extreme scene
    char m_stack[1024*256];

    static std::atomic_int s_id;
};

class MtsWorkerPool final : public SniperObjPool<MtsWorker>
{
public:
    static MtsWorkerPool *instance();
    static void destroy() { SniperObjPool<MtsWorker>::destroy(); }

    MtsWorker *create() { return new MtsWorker(false); }

    void spawn(int n);
    void syncEndUp(MtsWorker *worker);
    void waitAll();

private:
    int m_nAlive{0};
    std::mutex m_mutex;
    std::condition_variable m_sync;
    Sniper::Queue<MtsWorker *> m_freeWorkers{nullptr};
    Sniper::Queue<std::thread *> m_threads{nullptr};

    MtsWorkerPool() = default;
    virtual ~MtsWorkerPool();
};

#endif