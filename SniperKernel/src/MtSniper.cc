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
#include "SniperKernel/Sniper.h"
#include "SniperPrivate/MtsPrimaryTask.h"

MtSniper::MtSniper()
    : DLElement("MtSniper"),
      m_hasExternalPrimaryTask(false),
      m_itask(nullptr),
      m_otask(nullptr)
{
    m_tag = "MtSniper";

    declProp("NumThreads", m_nthrds = 4);
    declProp("EvtMax", m_evtMax = -1);

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
    return m_itask;
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
    sniper_context->set(Sniper::SysMode::MT);
    sniper_context->set_threads(m_nthrds);

    {
        // get the sniper main task instance and create its copies
        auto *mtask = m_sniperTaskPool->allocate();
        if (mtask != nullptr)
        {
            auto jstr4task = mtask->json().str(-1);
            for (int i = m_nthrds; i > 1; --i)
            {
                auto pobj = Sniper::create(jstr4task);
                auto ptask = dynamic_cast<Task *>(pobj);
                if (ptask == nullptr)
                {
                    LogError << "Failed to create a copy of the Sniper main Task" << std::endl;
                    delete pobj;
                    return false;
                }
                ptask->setScopeString(std::string("(")+std::to_string(i)+")");
                m_sniperTaskPool->deallocate(ptask); // put the new copy to the pool
            }
            m_sniperTaskPool->deallocate(mtask); // put the original instance back to the pool
        }
    }

    if (!m_hasExternalPrimaryTask)
    {
        // use the default primary task
        auto ptask = new MtsPrimaryTask();
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
            auto w = m_workerPool->get();
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