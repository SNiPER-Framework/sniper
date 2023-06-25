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

#include "SniperKernel/MtSniper.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperContext.h"
#include "SniperPrivate/MtsPrimaryTask.h"

MtSniper::MtSniper()
    : DLElement("MtSniper")
{
    m_tag = "MtSniper";

    declProp("NumThreads", m_nthrds = 4);
    declProp("EvtMax", m_evtMax = -1);

    m_taskQueue = MtsMicroTaskQueue::instance();
    m_workerPool = MtsWorkerPool::instance();
}

MtSniper::~MtSniper()
{
    m_taskQueue->destroy();
    m_workerPool->destroy();
}

bool MtSniper::initialize()
{
    sniper_context->set(Sniper::SysMode::MT);
    sniper_context->set_threads(m_nthrds);

    auto ptask = new MtsPrimaryTask();
    ptask->setEvtMax(m_evtMax);
    m_taskQueue->setPrimaryTask(ptask);

    return true;
}

bool MtSniper::finalize()
{
    return true;
}

void MtSniper::run()
{
    initialize();

    LogInfo << "begin run" << std::endl;

    for (int i = 0; i < m_nthrds; ++i)
    {
        // create and start a worker
        auto w = m_workerPool->get();
        w->start();
    }

    // wait for all the workers
    m_workerPool->waitAll();

    LogInfo << "end run" << std::endl;

    finalize();
}