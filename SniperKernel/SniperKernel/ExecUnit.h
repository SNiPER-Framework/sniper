/* Copyright (C) 2018-2023
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

#ifndef SNIPER_EXECUNIT_H
#define SNIPER_EXECUNIT_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/DleSupervisor.h"
#include "SniperKernel/TaskWatchDog.h"

class SvcBase;
class AlgBase;
class DataMemSvc;

class ExecUnit : public DLElement
{
public:
    // constructor and destructor
    ExecUnit(const std::string &name);
    virtual ~ExecUnit();

    // whether this the root node in the DLE tree
    bool isRoot() { return m_par == nullptr; }

    // find an owned service
    SvcBase *findSvc(const std::string &svcName);
    // find an owned algorithm
    AlgBase *findAlg(const std::string &algName);
    // get the owned DataMemSvc
    DataMemSvc *dataSvc() { return m_dataSvc; }

    // create an owned service
    SvcBase *createSvc(const std::string &svcName);
    // create an owned algorithm
    AlgBase *createAlg(const std::string &algName);
    // add concrete Svc/Alg to Task without ownership
    // it may be helpful for writing Svc/Alg in Python
    SvcBase *addSvc(SvcBase *svc);
    AlgBase *addAlg(AlgBase *alg);

    // clear all svcs/algs seperately
    void clearSvcs() { m_svcs.clear(); }
    void clearAlgs() { m_algs.clear(); }

    // find an owned element
    virtual DLElement *find(const std::string &name);
    // create an owned element, the type can be svc or alg
    virtual DLElement *create(const std::string &type, const std::string &name);
    // remove an owned element
    virtual void remove(const std::string &name);
    // clear all elements
    virtual void reset();

    // the json value of this object
    virtual SniperJSON json() override;
    // eval this Task from json
    virtual void eval(const SniperJSON &json) override;

    // use a customized WatchDog instead of the default one
    void setSnoopy(TaskWatchDog *snoopy);
    // get the current WatchDog
    TaskWatchDog &Snoopy() { return *m_snoopy; }

public:
    // none state check... Please use Snoopy for state control
    friend class TaskWatchDog;
    // the concrete task operations
    virtual bool config() = 0;
    virtual bool execute() = 0;
    virtual bool stop(Sniper::StopRun mode = Sniper::StopRun::Promptly) = 0;

protected:
    // the pointer to the WatchDog
    TaskWatchDog *m_snoopy;
    // the pointer to the DataMemSvc
    DataMemSvc *m_dataSvc;

    // the supervisors of services and algorithms
    DleSupervisor m_svcs;
    DleSupervisor m_algs;

    // the supervisor list
    std::list<DleSupervisor *> m_targets;
};

#endif
