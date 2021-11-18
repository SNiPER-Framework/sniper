/* Copyright (C) 2018-2021
   Haolai Tian <tianhl@ihep.ac.cn> Jiaheng Zou <zoujh@ihep.ac.cn> 
   Weidong Li <liwd@ihep.ac.cn> 
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

#ifndef SNIPER_WORKFLOW_H
#define SNIPER_WORKFLOW_H

#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/TaskWatchDog.h"

//Workflow definition
class Workflow : public ExecUnit
{
public:
    //Constructor
    Workflow(const std::string &name);

    //Destructor
    virtual ~Workflow();

    //set the log level of this domain
    void setLogLevel(int level) final;

    //run this workflow
    virtual bool run();

    //stop this workflow
    virtual bool stop(Sniper::StopRun mode = Sniper::StopRun::Promptly) override;

    //clear all svcs and algs
    virtual void reset() override;

    //ooo~~ just as its name
    TaskWatchDog &Snoopy() override { return m_snoopy; }

protected:
    //none state check... Please use Snoopy for state control
    friend class TaskWatchDog;
    //the concrete workflow operations
    virtual bool config() override;
    virtual bool initialize() override;
    virtual bool finalize() override;
    virtual bool execute() override;

protected:
    TaskWatchDog m_snoopy;
};

#endif
