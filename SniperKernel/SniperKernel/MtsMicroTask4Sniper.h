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

#ifndef SNIPER_MTS_MICRO_TASK_FOR_SNIPER_H
#define SNIPER_MTS_MICRO_TASK_FOR_SNIPER_H

#include "SniperKernel/MtsMicroTask.h"
#include "SniperKernel/Task.h"
#include <atomic>

class InitializeSniperTask : public MtsMicroTask
{
public:
    // for a MainTask, no lock needed
    InitializeSniperTask(Task *task)
        : m_sniperTask(task),
          m_lock(nullptr) {}

    // for an I/O Task, it should be locked before it initialized
    InitializeSniperTask(Task *task, std::atomic_flag &lock)
        : m_sniperTask(task),
          m_lock(&lock) { lock.test_and_set(); }

    virtual ~InitializeSniperTask()
    {
        if (m_lock != nullptr)
            m_lock->clear();
    }

    virtual Status exec() override;

private:
    Task *m_sniperTask;
    std::atomic_flag *m_lock;
};

#endif