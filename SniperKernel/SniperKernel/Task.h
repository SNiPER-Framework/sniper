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

#include "SniperKernel/IExecUnit.h"
#include "SniperKernel/DleSupervisor.h"
#include "SniperKernel/TaskWatchDog.h"
#include "SniperKernel/Incident.h"

//Pre-declarations
class SvcBase;
class AlgBase;

//Task definition
class Task : public IExecUnit
{
public:
    //Constructor
    Task(const std::string &name);

    //Destructor
    virtual ~Task();

    //run this task
    virtual bool run();

    //stop this task
    virtual bool stop(Sniper::StopRun mode = Sniper::StopRun::Promptly) override;

    //ooo~~ just as its name
    TaskWatchDog &Snoopy() override { return m_snoopy; }

protected:
    //none state check... Please use Snoopy for state control
    friend class TaskWatchDog;
    //the concrete task operations
    virtual bool config();
    virtual bool initialize();
    virtual bool finalize();
    virtual bool execute();
    //clear all svcs and algs
    void reset();

public:
    //set the log level of this domain
    void setLogLevel(int level) final;

    //set and get the max event number to be processed
    void setEvtMax(int evtMax) override;
    int evtMax() override { return m_evtMax; }

    //create owned Svc/Alg by name
    SvcBase *createSvc(const std::string &svcName) override;
    AlgBase *createAlg(const std::string &algName) override;

    //add concrete Svc/Alg to Task without ownership
    //it may be helpful for writing Svc/Alg in Python
    SvcBase *addSvc(SvcBase *svc);
    AlgBase *addAlg(AlgBase *alg);

    //find an element by its name
    virtual DLElement *find(const std::string &name) override;

    //remove an element with its name
    virtual void remove(const std::string &name);

    //clear all svcs/algs seperately
    void clearSvcs() { m_svcs.clear(); }
    void clearAlgs() { m_algs.clear(); }

    //the json value of this object
    virtual SniperJSON json();
    // eval this Task from json
    virtual void eval(const SniperJSON &json) override;

protected:
    void queue(DleSupervisor *target);

    //member data
    int m_evtMax;
    int m_done;
    TaskWatchDog m_snoopy;
    DleSupervisor m_svcs;
    DleSupervisor m_algs;

private:
    bool m_limited;
    Incident_T<int> m_beginEvt;
    Incident_T<int> m_endEvt;
    Incident_T<AlgBase *> m_beginAlg;
    Incident_T<AlgBase *> m_endAlg;
    std::list<DleSupervisor *> m_targets;
};

#endif
