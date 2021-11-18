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

#ifndef SNIPER_TASK_H
#define SNIPER_TASK_H

#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/TaskWatchDog.h"
#include "SniperKernel/Incident.h"

//Pre-declarations
class SvcBase;
class AlgBase;

//Task definition
class Task : public ExecUnit
{
public:
    //Constructor
    Task(const std::string &name);

    //Destructor
    virtual ~Task();

    //set and get the max event number to be processed
    void setEvtMax(int evtMax);
    int evtMax() { return m_evtMax; }

    //set the log level of this domain
    void setLogLevel(int level) final;

    //run this task
    virtual bool run();

    //stop this task
    virtual bool stop(Sniper::StopRun mode = Sniper::StopRun::Promptly) override;

    //reset the state of this object
    virtual void reset() override;

    //the json value of this object
    virtual SniperJSON json() override;

    // eval this Task from json
    virtual void eval(const SniperJSON &json) override;

    //ooo~~ just as its name
    TaskWatchDog &Snoopy() override { return m_snoopy; }

protected:
    //none state check... Please use Snoopy for state control
    friend class TaskWatchDog;
    //the concrete task operations
    virtual bool config() override;
    virtual bool initialize() override;
    virtual bool finalize() override;
    virtual bool execute() override;

protected:
    //member data
    bool m_limited;
    int m_evtMax;
    int m_done;
    TaskWatchDog m_snoopy;

private:
    //incidents
    Incident_T<int> m_beginEvt;
    Incident_T<int> m_endEvt;
    Incident_T<AlgBase *> m_beginAlg;
    Incident_T<AlgBase *> m_endAlg;
};

#endif
