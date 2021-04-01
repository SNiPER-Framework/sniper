/* Copyright (C) 2021
   Institute of High Energy Physics and Shandong University
   This file is part of mt.sniper.
 
   mt.sniper is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   mt.sniper is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with mt.sniper.  If not, see <http://www.gnu.org/licenses/>. */

#include "CoreUsages/DataIOSvc.h"
#include "CoreUsages/DummyDataStore.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(DataIOSvc);

struct BeginEvtHandler : public IIncidentHandler
{
    int m_count;

    BeginEvtHandler(Task *domain)
        : IIncidentHandler(domain),
          m_count(0)
    {
        m_name = "BeginEvtHandler";
    }

    bool handle(Incident &incident) override;
};

bool BeginEvtHandler::handle(Incident &incident)
{
    LogDebug << "Update input data store with " << m_count << std::endl;

    //We can update with data in an input file in real world
    SniperDataPtr<DummyDataStore> dstore(m_domain, "input");
    dstore->update(m_count++);

    return true;
}

struct EndEvtHandler : public IIncidentHandler
{
    EndEvtHandler(Task *domain)
        : IIncidentHandler(domain)
    {
        m_name = "EndEvtHandler";
    }

    bool handle(Incident &incident) override;
};

bool EndEvtHandler::handle(Incident &incident)
{
    SniperDataPtr<DummyDataStore> dstore(m_domain, "output");
    auto data = dstore->get();

    //We can save the result to an output file in real world
    LogDebug << "Get " << data << " from output data store" << std::endl;

    return true;
}

bool DataIOSvc::initialize()
{
    //create data stores and regist them to the DataMemSvc
    SniperPtr<DataMemSvc> dsvc(m_par, "DataMemSvc");
    dsvc->regist("input", new DummyDataStore);
    dsvc->regist("output", new DummyDataStore);

    //create and regist the handler for BeginEvent
    m_bh = new BeginEvtHandler(m_par);
    m_bh->regist("BeginEvent");

    //create and regist the handler for EndEvent
    m_eh = new EndEvtHandler(m_par);
    m_eh->regist("EndEvent");

    LogInfo << m_description << std::endl;
    return true;
}

bool DataIOSvc::finalize()
{
    //unregist and delete the handlers
    m_bh->unregist("BeginEvent");
    m_eh->unregist("EndEvent");
    delete m_bh;
    delete m_eh;

    LogInfo << "finalized successfully" << std::endl;
    return true;
}
