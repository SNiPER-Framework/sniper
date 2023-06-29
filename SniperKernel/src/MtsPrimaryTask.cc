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
#include "SniperKernel/SniperLog.h"

MtsPrimaryTask::MtsPrimaryTask()
    : m_evtMax(-1),
      m_done(0),
      m_itask(nullptr),
      m_otask(nullptr),
      m_ilock(ATOMIC_FLAG_INIT),
      m_olock(ATOMIC_FLAG_INIT),
      m_sniperTaskPool(nullptr)
{
    m_name = "MtsPrimaryTask";
    m_gb = mt_sniper_context->global_buffer;
}

MtsPrimaryTask::~MtsPrimaryTask()
{
    LogInfo << "events processed " << m_done << std::endl;
}

MtsMicroTask::Status MtsPrimaryTask::exec()
{
    // whether to run InputTask
    if (m_gb->urgent() && !m_ilock.test_and_set())
    {
        AtomicFlagLockGuard guard(false, m_ilock);
        return execInputTask();
    }

    // ...
    Status _status = Status::OK;
    long nsubtask = 0;

    // whether to run MainTask
    auto evtslot = m_gb->next();
    if (evtslot != nullptr)
    {
        _status = execMainTask(evtslot);
        ++nsubtask;
    }
    // whether to run OutputTask
    evtslot = m_gb->front();
    if (evtslot->status == 2 && !m_olock.test_and_set())
    {
        AtomicFlagLockGuard guard(false, m_olock);
        while (evtslot->status == 2)
        {
            execOutputTask(); // suppose it never fails
            m_gb->pop_front();
            evtslot = m_gb->front();
            ++nsubtask;
        }
    }

    if (nsubtask != 0)
    {
        return _status;
    }

    return m_gb->status() ? Status::NoTask : Status::NoMoreEvt;
}

MtsMicroTask::Status MtsPrimaryTask::execInputTask()
{
    static auto &snoopy = m_itask->Snoopy();

    bool status = true;
    while (status && m_gb->thirsty())
    {
        status = snoopy.run_once();
    }
    return status ? Status::OK : Status::Failed;
}

MtsMicroTask::Status MtsPrimaryTask::execOutputTask()
{
    static auto &snoopy = m_otask->Snoopy();
    return Status::OK;
}

MtsMicroTask::Status MtsPrimaryTask::execMainTask(MtsEvtBufferRing::EvtSlot *slot)
{
    static const bool infinite = m_evtMax < 0;
    static std::atomic_long count{0};  //it's different to m_done

    if (infinite || count++ < m_evtMax)
    {
        auto task = m_sniperTaskPool->allocate();
        bool status = task->Snoopy().run_once();
        m_sniperTaskPool->deallocate(task);
        if (status)
        {
            LogDebug << "processed event: " << ++m_done << std::endl;
            return Status::OK;
        }
        return Status::Failed; // error
    }
    else {
        auto task = m_sniperTaskPool->allocate();
        if (task != nullptr)
        {
            task->Snoopy().finalize();
            delete task;
        }
        return Status::NoMoreEvt;
    }
}
