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
#include "SniperKernel/NamedElement.h"
#include <thread>
#include <atomic>

class MtsWorker final : public NamedElement
{
public:
    MtsWorker();
    ~MtsWorker();

    void start();
    void run();
    void wait();

private:
    std::thread *m_thrd;

    static std::atomic_int s_id;
};

class MtsWorkerPool final : private SniperObjPool<MtsWorker>
{
public:
    static MtsWorkerPool *instance();
    static void destroy() { SniperObjPool<MtsWorker>::destroy(); }

    MtsWorker* get();
    void put(MtsWorker *w) { deallocate(w); }
    void waitAll();

private:
    MtsWorkerPool();
    virtual ~MtsWorkerPool() = default;

    Sniper::Queue<MtsWorker*> m_allWorkers;
};

#endif