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
#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/SniperLog.h"

MtsMicroTask::Status InitializeSniperTask::exec()
{
    auto &snoopy = m_sniperTask->Snoopy();
    bool status = snoopy.config() && snoopy.initialize();
    if (m_lock == nullptr)
    {
        // this is a MainTask, put it back to the SniperTaskPool
        SniperObjPool<Task>::instance()->deallocate(m_sniperTask);
    }
    delete this; // manages itself
    return status ? Status::OK : Status::Failed;
}