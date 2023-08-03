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

#ifndef SNIPER_MTS_PRIMARY_TASK_H
#define SNIPER_MTS_PRIMARY_TASK_H

#include "SniperKernel/MtsMicroTask.h"
#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/MtsEvtBufferRing.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/DataStore.h"
#include <atomic>

class MtsPrimaryTask final : public MtsMicroTask
{
public:
    MtsPrimaryTask(std::atomic_flag &ilock, std::atomic_flag &olock);
    virtual ~MtsPrimaryTask();

    void setEvtMax(long evtMax) { m_evtMax = evtMax; }

    // set the SNiPER Tasks for data processing
    void setInputTask(Task *itask) { m_itask = itask; }
    void setOutputTask(Task *otask) { m_otask = otask; }
    void setMainTaskPool(SniperObjPool<Task> *mtaskpool) { m_sniperTaskPool = mtaskpool; }

    // do the data processing as a MicroTask once
    virtual Status exec() override;

private:
    Status execInputTask();
    Status execMainTask(MtsEvtBufferRing::EvtSlot *slot);
    Status cleanTaskPool();

    void setGBEvt2Task(std::any &evt, Task *task);
    void setGBSlot2Task(MtsEvtBufferRing::EvtSlot *slot, Task *task);

    long m_evtMax{-1};          // the max event number to be processed
    std::atomic_long m_done{0}; // the event number has been processed

    // the SNiPER Tasks for I/O
    Task *m_itask{nullptr};
    Task *m_otask{nullptr};

    // atomic_flag as locks
    std::atomic_flag &m_ilock;
    std::atomic_flag &m_olock;

    // for all the SNiPER main Task copies
    SniperObjPool<Task> *m_sniperTaskPool{nullptr};

    // helpers
    MtsEvtBufferRing* m_gb;
    std::map<Task *, Sniper::DataStore<std::any *> *> m_eventStore;
    std::map<Task *, Sniper::DataStore<MtsEvtBufferRing::SlotStatus *> *> m_statusStore;
};

#endif