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
#include "CoreUsages/DummyDataStore.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/AlgFactory.h"

DECLARE_ALGORITHM(DummyAlg);

DummyAlg::DummyAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("INFO", m_info);
}

DummyAlg::~DummyAlg()
{
}

bool DummyAlg::initialize()
{
    SniperDataPtr<DummyDataStore> iStore(m_par, "input");
    SniperDataPtr<DummyDataStore> oStore(m_par, "output");
    if (iStore.invalid() || oStore.invalid())
    {
        LogError << "failed to find the data store" << std::endl;
        return false;
    }

    m_input = iStore.data();
    m_output = oStore.data();

    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool DummyAlg::execute()
{
    //get input data
    int data = m_input->get();
    LogDebug << m_info << data << std::endl;

    //any calculations here
    data *= 10;

    //update output data
    m_output->update(data);

    return true;
}

bool DummyAlg::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}
