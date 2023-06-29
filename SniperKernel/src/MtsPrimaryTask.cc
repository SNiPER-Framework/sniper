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
      m_sniperTaskPool(nullptr)
{
    m_name = "MtsPrimaryTask";
}

MtsPrimaryTask::~MtsPrimaryTask()
{
    LogInfo << "events processed " << m_done << std::endl;
}

MtsMicroTask::Status MtsPrimaryTask::exec()
{
    static const bool infinite = m_evtMax < 0;
    static std::atomic_long count{0};  //it's different to m_done

    long nexec = 0;
    //if (gb->ample())
    //{
    //    auto evtslot = gb->next();
    //    if (evtslot != nullptr)
    //    {
    //        //xxx set context
    //        execMainTask();
    //        ++nexec;
    //    }
    //    evtslot = gb->front();
    //    if (evtslot->status == 2 && m_olock.test_and_set())
    //    {
    //        guard(m_olock);
    //        while (evtslot->status == 2)
    //        {
    //            execOutputTask();
    //            gb->pop_front();
    //            evtslot = gb->front();
    //            ++nexec;
    //        }
    //    }
    //}

    //if (nexec == 0 && m_itask.test_and_set())
    //{
    //    guard(m_ilock);
    //    while (!gb->full())
    //    {
    //        execInputTask();
    //        ++nexec;
    //    }
    //}

    //if (nexec == 0)
    //{
    //    return 9; //nothing to do, wait for new microtasks
    //}

    if (infinite || count++ < m_evtMax)
    {
        auto task = m_sniperTaskPool->allocate();
        if (task->Snoopy().run_once())
        {
            m_sniperTaskPool->deallocate(task);
            long done = ++m_done;
            LogDebug << "processed event: " << done << std::endl;
            return Status::OK;
        }
        return Status::Failed; // error
    }
    else {
        auto task = m_sniperTaskPool->allocate();
        if (task != nullptr)
        {
            task->Snoopy().finalize();
        }
    }

    return Status::NoMoreEvt;
}
