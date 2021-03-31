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

#include "DummyAlg.h"
#include "DummyTool.h"
#include "CoreUsages/DummySvc.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"

DECLARE_ALGORITHM(DummyAlg);

DummyAlg::DummyAlg(const std::string &name)
    : AlgBase(name),
      m_count(0)
{
}

DummyAlg::~DummyAlg()
{
}

bool DummyAlg::initialize()
{
    // get the SharedSvc
    SniperPtr<DummySvc> svc(getParent(), "SharedSvc");
    if (svc.valid())
    {
        m_svc = svc.data();
    }

    // get the OwnedTool
    m_tool = this->tool<DummyTool>("OwnedTool");

    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool DummyAlg::execute()
{
    LogDebug << "Begin Event " << m_count << std::endl;

    m_svc->doSomeThing();
    m_tool->doSomeThing();

    ++m_count;

    return true;
}

bool DummyAlg::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}
