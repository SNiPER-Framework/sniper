/* Copyright (C) 2018-2021
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

#include "SniperPrivate/WhiteBoard.h"
#include "SniperPrivate/IncidentMgr.h"
#include <mutex>

static WhiteBoard s_wbObj;
static std::mutex s_local_mutex;

void WhiteBoard::mark(ExecUnit* task)
{
    std::lock_guard<std::mutex> lock(s_local_mutex);
    if ( task->isRoot() ) {
        IncidentMgr::create(*task);
    }
    else {
        s_wbObj.m_list.push_back( new TaskTrigger(task) );
    }
}

void WhiteBoard::clear()
{
    for ( auto i : s_wbObj.m_list ) {
        delete i;
    }
    s_wbObj.m_list.clear();
}

WhiteBoard::~WhiteBoard()
{
    clear();
}
