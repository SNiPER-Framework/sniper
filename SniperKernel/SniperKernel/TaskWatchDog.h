/* Copyright (C) 2018
   Jiaheng Zou <zoujh@ihep.ac.cn> Tao Lin <lintao@ihep.ac.cn>
   Weidong Li <liwd@ihep.ac.cn> Xingtao Huang <huangxt@sdu.edu.cn>
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

class Task;

class TaskWatchDog final  //XXX: final?
{
    public:

        // constructor and destructor
        TaskWatchDog(Task* task);
        virtual ~TaskWatchDog();

        // operations
        bool config();
        bool initialize();
        bool run();
        bool pause();
        bool stop();
        bool finalize();
        bool terminate();
        // execute one time and then pause
        bool run_once();

        // reset its state and the task
        void reset();

        // call it at the place where error happens
        void setErr();

        // status report
        bool isErr();

        // ...
        const Sniper::RunState& state() { return m_stat; }
        Sniper::RunState  pre_state();

    private:

        // pretend as a NamedElement
        int   logLevel();
        const std::string& scope();
        const std::string& objName();

        //Data members
        Sniper::RunState  m_stat;
        Task&             m_task;

        // we should not copy a TaskWatchDog
        TaskWatchDog() = delete;
        TaskWatchDog(const TaskWatchDog&) = delete;
        TaskWatchDog& operator=(const TaskWatchDog&) = delete;
};

#endif
