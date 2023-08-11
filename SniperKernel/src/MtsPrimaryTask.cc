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

#include "SniperPrivate/MtsPrimaryTask.h"
#include "SniperKernel/MtSniperUtility.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/SniperException.h"
#include "SniperKernel/SniperLog.h"

MtsPrimaryTask::MtsPrimaryTask(std::atomic_flag &ilock, std::atomic_flag &olock)
    : m_ilock(ilock),
      m_olock(olock)
{
    m_name = "MtsPrimaryTask";
    m_gb = MtSniperUtil::GlobalBuffer::instance();
}

MtsPrimaryTask::~MtsPrimaryTask()
{
    LogDebug << "events processed " << m_done << std::endl;
}

MtsMicroTask::Status MtsPrimaryTask::exec()
{
    // whether to run InputTask
    if (m_gb->urgent() && !m_ilock.test_and_set())
    {
        AtomicFlagLockGuard<false> guard(m_ilock);
        return execInputTask();
    }

    // ...
    Status returnCode = Status::OK;
    long nsubtask = 0;

    // whether to run MainTask
    auto evtslot = m_gb->next();
    if (evtslot != nullptr)
    {
        returnCode = execMainTask(evtslot);
        ++nsubtask;
    }
    // whether to run OutputTask
    evtslot = m_gb->front();
    if (evtslot->status == MtsEvtBufferRing::SlotStatus::Done && !m_olock.test_and_set())
    {
        static auto &snoopy = m_otask->Snoopy();
        AtomicFlagLockGuard<false> guard(m_olock);
        while (evtslot->status == MtsEvtBufferRing::SlotStatus::Done)
        {
            setGBEvt2Task(evtslot->evt, m_otask);
            snoopy.run_once(); // suppose it never fails
            m_gb->pop_front();
            evtslot = m_gb->front();
            ++nsubtask;
        }
    }

    if (nsubtask != 0)
    {
        return returnCode;
    }

    return m_gb->vigorous() ? Status::NoTask : cleanTaskPool();
}

MtsMicroTask::Status MtsPrimaryTask::execInputTask()
{
    static auto &snoopy = m_itask->Snoopy();

    bool status = true;
    while (m_gb->eager() && status)
    {
        try
        {
            status = snoopy.run_once();
            MtSniperUtil::Thread::resumeOne();
        }
        catch (StopRunProcess &e)
        {
            m_gb->deVigorous();
        }
    }
    return status ? Status::OK : Status::Failed;
}

MtsMicroTask::Status MtsPrimaryTask::execMainTask(MtsEvtBufferRing::EvtSlot *slot)
{
    static const bool infinite = m_evtMax < 0;
    static std::atomic_long count{0};  //it's different to m_done

    if (infinite || count++ < m_evtMax)
    {
        auto task = m_sniperTaskPool->allocate();
        setGBSlot2Task(slot, task);
        bool status = task->Snoopy().run_once();
        // in case of inter-algorithm MT:
        // 1. the slot status must be set at the EndEvent point
        // 2. the SniperTask must not be deallocated until the EndEvent point
        // slot->status = MtsEvtBufferRing::SlotStatus::Done;
        // m_sniperTaskPool->deallocate(task);
        if (status)
        {
            LogDebug << "processed event: " << ++m_done << std::endl;
            return Status::OK;
        }
        return Status::Failed; // error
    }
    else {
        m_gb->deVigorous();
        return cleanTaskPool();
    }
}

MtsMicroTask::Status MtsPrimaryTask::cleanTaskPool()
{
    if (!m_ilock.test_and_set())
    {
        m_itask->Snoopy().finalize();
    }
    while (auto task = m_sniperTaskPool->allocate())
    {
        task->Snoopy().finalize();
        delete task;
    }
    return Status::NoMoreEvt;
}

void MtsPrimaryTask::setGBEvt2Task(std::any &evt, Task *task)
{
    auto p = m_eventStore[task];
    if (p == nullptr)
    {
        p = dynamic_cast<Sniper::DataStore<std::any *> *>(task->dataSvc()->find("GBEVENT"));
        m_eventStore[task] = p;
    }
    p->put(&evt);
}

void MtsPrimaryTask::setGBSlot2Task(MtsEvtBufferRing::EvtSlot *slot, Task *task)
{
    auto p = m_eventStore[task];
    if (p == nullptr)
    {
        auto dsvc = task->dataSvc();
        p = dynamic_cast<Sniper::DataStore<std::any *> *>(dsvc->find("GBEVENT"));
        m_eventStore[task] = p;
        m_statusStore[task] = dynamic_cast<Sniper::DataStore<MtsEvtBufferRing::SlotStatus *> *>(dsvc->find("GBSTATUS"));
    }
    p->put(&(slot->evt));
    m_statusStore[task]->put(&(slot->status));
}