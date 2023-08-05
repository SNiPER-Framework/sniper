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
#include "SniperKernel/MtsMicroTask4Sniper.h"
#include "SniperKernel/MtSniperUtility.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/DataStore.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/DeclareDLE.h"
#include "SniperPrivate/MtsPrimaryTask.h"
#include <thread>

SNIPER_DECLARE_DLE(MtSniper);

MtSniper::MtSniper(const std::string &name)
    : DLElement(name)
{
    m_tag = "MtSniper";

    declProp("NumThreads", m_nthrds = 4);
    declProp("EvtMax", m_evtMax = -1);
    declProp("GlobalBufferCapacity", m_gbufCapacity = -1);
    declProp("GlobalBufferThreshold", m_gbufThreshold = -1);

    MtSniperUtil::contextInit();
    m_microTaskQueue = MtsMicroTaskQueue::instance();
    m_sniperTaskPool = SniperObjPool<Task>::instance();
}

MtSniper::~MtSniper()
{
    delete m_itask;
    delete m_otask;

    m_sniperTaskPool->destroy();
    m_microTaskQueue->destroy();
    MtSniperUtil::contextRelease();
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
    m_mtask = createSniperTask(identifier);
    if (m_mtask != nullptr)
    {
        m_mtask->setScopeString("(1)");
    }
    return m_mtask;
}

SniperJSON MtSniper::json()
{
    static SniperJSON keys = SniperJSON().from(std::vector<std::string>{
        "sniper",
        "description",
        "identifier",
        "properties",
        "InputTask",
        "OutputTask",
        "MainTask"});

    SniperJSON j = DLElement::json();
    if (m_itask != nullptr)
    {
        j.insert("InputTask", m_itask->json());
        j["InputTask"].erase("sniper");
    }
    if (m_otask != nullptr)
    {
        j.insert("OutputTask", m_otask->json());
        j["OutputTask"].erase("sniper");
    }
    if (m_mtask != nullptr)
    {
        j.insert("MainTask", m_mtask->json());
        j.insert("sniper", j["MainTask"]["sniper"]);
        j["MainTask"].erase("sniper");
    }

    j.insert("ordered_keys", keys);

    return j;
}

void MtSniper::eval(const SniperJSON &json)
{
    DLElement::eval(json);

    auto &jitask = json["InputTask"];
    auto itask = createInputTask(jitask["identifier"].get<std::string>());
    itask->eval(jitask);

    auto &jotask = json["OutputTask"];
    auto otask = createOutputTask(jotask["identifier"].get<std::string>());
    otask->eval(jotask);

    auto &jmtask = json["MainTask"];
    auto mtask = createMainTask(jmtask["identifier"].get<std::string>());
    mtask->eval(jmtask);
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

    // config the global buffer for MtSniper
    if (m_gbufThreshold < m_nthrds || m_gbufThreshold > m_gbufCapacity)
    {
        LogError << "Improper GlobalBuffer options, please make sure: NumThreads <= threshold <= capacity" << std::endl;
        return false;
    }
    MtSniperUtil::GlobalBuffer::instance()->config(m_gbufCapacity, m_gbufThreshold);

    // create the micro tasks for IO Task initialize
    m_microTaskQueue->enqueue(new InitializeSniperTask(m_itask, m_ilock));
    m_microTaskQueue->enqueue(new InitializeSniperTask(m_otask, m_olock));

    // get the sniper main Task instance and create its copies and micro tasks for initialize
    m_microTaskQueue->enqueue(new InitializeSniperTask(m_mtask));
    auto jtask = m_mtask->json();
    auto identifier = jtask["identifier"].get<std::string>();
    for (int i = m_nthrds; i > 1; --i)
    {
        auto ptask = createSniperTask(identifier);
        ptask->setScopeString(std::string("(") + std::to_string(i) + ')');
        ptask->eval(jtask);
        m_microTaskQueue->enqueue(new InitializeSniperTask(ptask));
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
    return m_otask->Snoopy().finalize(); // m_itask has been finalized in PrimaryTask
}

bool MtSniper::run()
{
    if (!this->initialize())
    {
        LogError << "initialize failed" << std::endl;
        return false;
    }

    LogInfo << "MtSniper initialized and now start workers..." << std::endl;

    // start m_nthrds workers and each runs in a seperated threads
    MtSniperUtil::Worker::spawn(m_nthrds);

    // wait for the ending of all the workers
    MtSniperUtil::Worker::waitAll();

    if (!this->finalize())
    {
        LogError << "finalize failed" << std::endl;
        return false;
    }

    LogInfo << "all workers finished successfully" << std::endl;

    return true;
}