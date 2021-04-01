/* Copyright (C) 2021
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

#include "SvcAndToolAlg.h"
#include "DummyTool.h"
#include "CoreUsages/DummySvc.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/AlgFactory.h"

DECLARE_ALGORITHM(SvcAndToolAlg);

SvcAndToolAlg::SvcAndToolAlg(const std::string &name)
    : AlgBase(name),
      m_count(0)
{
    declProp("SharedSvc", m_svcName = "SharedSvc");
    declProp("OwnedTool", m_toolName = "OwnedTool");
}

SvcAndToolAlg::~SvcAndToolAlg()
{
}

bool SvcAndToolAlg::initialize()
{
    // get the SharedSvc
    SniperPtr<DummySvc> svc(getParent(), m_svcName);
    if (!svc.valid())
    {
        LogError << "failed to get service " << m_svcName << std::endl;
        return false;
    }
    m_svc = svc.data();

    // get the OwnedTool
    m_tool = this->tool<DummyTool>(m_toolName);
    if (m_tool == nullptr)
    {
        LogError << "failed to get tool " << m_toolName << std::endl;
        return false;
    }

    // initialize the tool if necessary
    if (!m_tool->initialize())
    {
        LogError << "failed to initialize tool " << m_toolName << std::endl;
        return false;
    }

    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool SvcAndToolAlg::execute()
{
    LogDebug << "Begin Event " << m_count << std::endl;

    m_svc->doSomeThing();
    m_tool->doSomeThing();

    ++m_count;

    return true;
}

bool SvcAndToolAlg::finalize()
{
    // finalize the tool if necessary
    if (!m_tool->finalize())
    {
        LogError << "failed to finalize tool " << m_toolName << std::endl;
        return false;
    }

    LogInfo << "finalized successfully" << std::endl;
    return true;
}
