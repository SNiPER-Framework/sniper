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

#ifndef SNIPER_MTS_MICRO_TASK_H
#define SNIPER_MTS_MICRO_TASK_H

#include "SniperKernel/NamedElement.h"
#include "SniperKernel/MtsIncubatorBase.h"

class MtsMicroTask : public NamedElement
{
public:
    enum class Status
    {
        OK,
        Failed,
        BatchEnd, // a batch of MicroTasks from a Incubator finished
        NoTask,
        NoMoreEvt
    };

    MtsMicroTask() : NamedElement("MtSniper:", "MtsMicroTask") {}
    virtual ~MtsMicroTask() = default;

    void setIncubator(MtsIncubatorBase *incubator) { m_incubator = incubator; }

    inline Status run()
    {
        auto status = exec();
        if (m_incubator != nullptr && m_incubator->notify(this))
        {
            return Status::BatchEnd;
        }
        return status;
    }

    virtual Status exec() = 0;

private:
    MtsIncubatorBase *m_incubator{nullptr};
};

#endif