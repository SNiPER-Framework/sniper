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
#include <atomic>

class MtsPrimaryTask final : public MtsMicroTask
{
public:
    MtsPrimaryTask();
    virtual ~MtsPrimaryTask();

    void setEvtMax(long evtMax) { m_evtMax = evtMax; }

    virtual int exec() override;

private:
    long m_evtMax;
    long m_done;  //the processed event number
    std::atomic_int m_count;
};

#endif