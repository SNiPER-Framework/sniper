/* Copyright (C) 2018 Jiaheng Zou <zoujh@ihep.ac.cn>
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

#include "TaskSupervisor.h"
#include "SniperTbbTask.h"
#include "SniperKernel/SniperLog.h"

bool TaskSupervisor::intake(TaskWatchDog& snoopy)
{
    m_children.push_back(
            new(allocate_child()) SniperTbbTask(snoopy) );

    return true;
}

tbb::task* TaskSupervisor::execute()
{
    if ( m_stat )  // @ continuation status
    {
        LogInfo << "entering continuation status" << std::endl;
    }
    else  // @ init status
    {
        LogInfo << "entering init status" << std::endl;
        int n = m_children.size();

        recycle_as_safe_continuation();
        m_stat = true;

        set_ref_count(n+1);
        for ( auto c : m_children ) spawn(*c);

        m_children.clear();
    }

    return nullptr;
}
