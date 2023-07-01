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
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/MtSniperContext.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/MtsMicroTask4Sniper.h"
#include "SniperPrivate/MtsPrimaryTask.h"

Sniper::MtContext *mt_sniper_context = nullptr;
thread_local std::any* Sniper::MtContext::current_event = nullptr;

MtSniper::MtSniper()
    : DLElement("MtSniper")
{
    m_tag = "MtSniper";

    declProp("NumThreads", m_nthrds = 4);
    declProp("EvtMax", m_evtMax = -1);
    declProp("GlobalBufferCapacity", m_gbufCapacity = -1);
    declProp("GlobalBufferThreshold", m_gbufThreshold = -1);

    mt_sniper_context = new Sniper::MtContext;
    mt_sniper_context->global_buffer = nullptr;

    m_microTaskQueue = MtsMicroTaskQueue::instance();
    m_workerPool = MtsWorkerPool::instance();
    m_sniperTaskPool = SniperObjPool<Task>::instance();
}

MtSniper::~MtSniper()
{
    delete m_itask;
    delete m_otask;

    m_sniperTaskPool->destroy();
    m_workerPool->destroy();
    m_microTaskQueue->destroy();

    delete mt_sniper_context;
}

void MtSniper::configGlobalBuffer(int capacity, int threshold)
{
    m_gbufCapacity = capacity;
    m_gbufThreshold = threshold;
}

void MtSniper::setPrimaryTask(MtsMicroTask *task)
{
    m_hasExternalPrimaryTask = true;
    m_microTaskQueue->setPrimaryTask(task);
}

Task *MtSniper::createSniperTask(const std::string &identifier)
{
    auto pobj = Sniper::create(identifier);
    auto ptask = dynamic_cast<Task *>(pobj);
    if (ptask == nullptr)
    {
        LogError << "Failed to create " << identifier << std::endl;
        delete pobj;
    }
    return ptask;
}

Task *MtSniper::createInputTask(const std::string &identifier)
{
    m_itask = createSniperTask(identifier);
    return m_itask;
}

Task *MtSniper::createOutputTask(const std::string &identifier)
{
    m_otask = createSniperTask(identifier);
    return m_otask;
}

Task *MtSniper::createMainTask(const std::string &identifier)
{
    auto mtask = createSniperTask(identifier);
    if (mtask != nullptr)
    {
        mtask->setScopeString("(1)");
        m_sniperTaskPool->deallocate(mtask);
    }
    return mtask;
}

bool MtSniper::initialize()
{
    auto maxThreads = std::thread::hardware_concurrency();
    if (maxThreads != 0 && m_nthrds > maxThreads)
    {
        LogWarn << "decrease the thread number to the hardware limit: " << maxThreads << std::endl;
        m_nthrds = maxThreads;
    }
    sniper_context->set(Sniper::SysMode::MT);
    sniper_context->set_threads(m_nthrds);

    // create the global buffer for MtSniper
    if (m_gbufThreshold < 0 || m_gbufThreshold >= m_gbufCapacity)
    {
        LogWarn << "improper GlobalBuffer capacity and threshold" << std::endl;
        return false;
    }
    mt_sniper_context->global_buffer = new MtsEvtBufferRing(m_gbufCapacity, m_gbufThreshold);

    // create the micro tasks for IO Task initialize
    m_microTaskQueue->push(new InitializeSniperTask(m_itask, m_ilock));
    m_microTaskQueue->push(new InitializeSniperTask(m_otask, m_olock));

    // get the sniper main Task instance and create its copies and micro tasks for initialize
    auto *mtask = m_sniperTaskPool->allocate();
    m_microTaskQueue->push(new InitializeSniperTask(mtask));
    auto jtask = mtask->json();
    auto identifier = jtask["identifier"].get<std::string>();
    for (int i = m_nthrds; i > 1; --i)
    {
        auto ptask = createSniperTask(identifier);
        ptask->setScopeString(std::string("(") + std::to_string(i) + ")");
        ptask->eval(jtask);
        m_microTaskQueue->push(new InitializeSniperTask(ptask));
    }

    // use the default primary task if it's not set externally
    if (!m_hasExternalPrimaryTask)
    {
        auto ptask = new MtsPrimaryTask(m_ilock, m_olock);
        ptask->setEvtMax(m_evtMax);
        ptask->setInputTask(m_itask);
        ptask->setOutputTask(m_otask);
        ptask->setMainTaskPool(m_sniperTaskPool);
        m_microTaskQueue->setPrimaryTask(ptask);
    }

    return true;
}

bool MtSniper::finalize()
{
    if (mt_sniper_context->global_buffer != nullptr)
    {
        delete mt_sniper_context->global_buffer;
    }

    return true;
}

bool MtSniper::run()
{
    bool status = initialize();
    if (status)
    {
        LogInfo << "MtSniper initialized and now start workers..." << std::endl;
        for (int i = 0; i < m_nthrds; ++i)
        {
            auto w = m_workerPool->create();
            w->start();
        }

        // wait for all the workers
        m_workerPool->waitAll();

        status = finalize();
        if (status)
        {
            LogInfo << "all workers finished successfully" << std::endl;
        }
    }

    return status;
}