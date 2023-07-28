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

#ifndef SNIPER_TASK_WATCH_DOG_H
#define SNIPER_TASK_WATCH_DOG_H

#include "SniperKernel/SniperRunState.h"
#include <string>
#include <list>

class ExecUnit;
class DLElement;

class TaskWatchDog
{
public:
    // constructor and destructor
    TaskWatchDog(ExecUnit *task);
    virtual ~TaskWatchDog();

    // operations
    virtual bool config();
    bool initialize();
    bool run();
    bool pause();
    bool stop(Sniper::StopRun mode = Sniper::StopRun::Promptly);
    bool finalize();
    bool terminate();
    // execute one time and then pause
    virtual bool run_once();

    // reset its state and the task
    void reset();

    // call it at the place where error happens
    void setErr();

    // status report
    bool isRunning() { return m_stat == Sniper::RunState::Running; }
    bool isStopped() { return m_stat == Sniper::RunState::Stopped; }
    bool isErr();

    // ...
    const Sniper::RunState &state() { return m_stat; }
    Sniper::RunState real_state();

    ExecUnit &host() { return m_task; }

protected:
    // pretend as a NamedElement
    int logLevel();
    const std::string &scope();
    const std::string &objName();

    // helper functions
    const std::list<DLElement *> &algList();
    long *evtDone();

    // Data members
    Sniper::RunState m_stat;
    ExecUnit &m_task;

    // we should not copy a TaskWatchDog
    TaskWatchDog() = delete;
    TaskWatchDog(const TaskWatchDog &) = delete;
    TaskWatchDog &operator=(const TaskWatchDog &) = delete;
};

#endif
