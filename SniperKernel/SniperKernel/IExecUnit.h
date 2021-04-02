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

#ifndef SNIPER_IEXECUNIT_H
#define SNIPER_IEXECUNIT_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/TaskWatchDog.h"
#include "SniperKernel/SniperRunState.h"

class SvcBase;
class AlgBase;

class IExecUnit : public DLElement
{
public:
    IExecUnit(const std::string &name)
        : DLElement(name) {}

    // common interfaces
    bool isRoot() { return m_par == nullptr; }

    // pure virtual interfaces
    virtual DLElement *find(const std::string &name) = 0;
    virtual SvcBase *createSvc(const std::string &svcName) = 0;
    virtual AlgBase *createAlg(const std::string &algName) = 0;
    virtual int evtMax() = 0;
    virtual void setEvtMax(int evtMax) = 0;
    virtual TaskWatchDog &Snoopy() = 0;
    virtual bool stop(Sniper::StopRun mode = Sniper::StopRun::Promptly) = 0;
};

#endif
