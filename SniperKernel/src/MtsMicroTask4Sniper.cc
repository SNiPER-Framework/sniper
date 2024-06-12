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

#include "SniperKernel/MtsMicroTask4Sniper.h"
#include "SniperKernel/DataStore.h"
#include "SniperKernel/MtsEvtBufferRing.h"
#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/MtSniper.h"
#include <any>

std::atomic_int InitializeSniperTask::s_count = 0;

class EndEvtHandler4MtsMainTask : public IIncidentHandler
{
public:
    EndEvtHandler4MtsMainTask(Task *task, Sniper::DataStore<MtsEvtBufferRing::SlotStatus *> *store);
    virtual ~EndEvtHandler4MtsMainTask() = default;

    virtual bool handle(Incident &incident) override;

private:
    Task *m_task;
    Sniper::DataStore<MtsEvtBufferRing::SlotStatus *> *m_store;
    SniperObjPool<Task> *m_sniperTaskPool{nullptr};
};

EndEvtHandler4MtsMainTask::EndEvtHandler4MtsMainTask(Task *task, Sniper::DataStore<MtsEvtBufferRing::SlotStatus *> *store)
    : IIncidentHandler(task),
      m_task(task),
      m_store(store)
{
    m_sniperTaskPool = SniperObjPool<Task>::instance();
}

bool EndEvtHandler4MtsMainTask::handle(Incident & /*Incident*/)
{
    *(m_store->get()) = MtsEvtBufferRing::SlotStatus::Done;
    m_sniperTaskPool->deallocate(m_task);
    return true;
}

MtsMicroTask::Status InitializeSniperTask::exec()
{
    auto dsvc = m_sniperTask->dataSvc();
    dsvc->regist("GBEVENT", new Sniper::DataStore<std::any *>());

    auto &snoopy = m_sniperTask->Snoopy();
    bool status = snoopy.config() && snoopy.initialize();
    if (m_lock == nullptr)
    {
        // this is a MainTask
        auto store = new Sniper::DataStore<MtsEvtBufferRing::SlotStatus *>();
        dsvc->regist("GBSTATUS", store);
        auto handler = new EndEvtHandler4MtsMainTask(m_sniperTask, store);
        handler->regist("EndEvent");
        SniperObjPool<EndEvtHandler4MtsMainTask>::instance()->deallocate(handler);
        // put it back to the SniperTaskPool
        SniperObjPool<Task>::instance()->deallocate(m_sniperTask);
    }

    if (--s_count == 0)
    {
        MtSniper::notifyInitialized();
    }
    delete this; // manages itself

    return status ? Status::OK : Status::Failed;
}
