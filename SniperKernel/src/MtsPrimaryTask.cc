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
      m_count(0)
{
    m_name = "MtsPrimaryTask";
}

MtsPrimaryTask::~MtsPrimaryTask()
{
    LogInfo << "events processed " << m_done << std::endl;
}

int MtsPrimaryTask::exec()
{
    static const bool infinite = m_evtMax < 0;

    int count = m_count++;
    if (count < m_evtMax || infinite)
    {
        LogInfo << "execute a primary task, count " << count << std::endl;
        ++m_done;
        return 0;
    }

    return -9; // reach the evtMax
}
