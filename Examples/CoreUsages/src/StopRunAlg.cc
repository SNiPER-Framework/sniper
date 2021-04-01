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

#include "StopRunAlg.h"
#include "CoreUsages/DummyDataStore.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/AlgFactory.h"

DECLARE_ALGORITHM(StopRunAlg);

StopRunAlg::StopRunAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("StopMode", m_stopMode);
}

StopRunAlg::~StopRunAlg()
{
}

bool StopRunAlg::initialize()
{
    if (m_stopMode == "Promptly")
    {
        m_mode = Sniper::StopRun::Promptly;
    }
    else if (m_stopMode == "Peacefully")
    {
        m_mode = Sniper::StopRun::Peacefully;
    }
    else if (m_stopMode == "ThisEvent")
    {
        m_mode = Sniper::StopRun::ThisEvent;
    }

    SniperDataPtr<DummyDataStore> iStore(m_par, "input");
    m_input = iStore.data();

    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool StopRunAlg::execute()
{
    int data = m_input->get();
    LogDebug << "execute event " << data << std::endl;

    if (data == 2)
    {
        LogDebug << "before stop event " << data << ": " << m_stopMode << std::endl;
        m_par->stop(m_mode);
        LogDebug << "after stop event " << data << ": " << m_stopMode << std::endl;
    }

    return true;
}

bool StopRunAlg::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}
