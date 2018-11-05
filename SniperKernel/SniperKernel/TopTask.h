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

#ifndef SNIPER_TOP_TASK_H
#define SNIPER_TOP_TASK_H

#include "SniperKernel/Task.h"

class TopTask final : public Task
{
    public:

        TopTask(const std::string& name);
        virtual ~TopTask();

        // interfaces for sub-tasks
        Task* createTask(const std::string& taskName);
        Task* addTask(Task* task);
        void  clearTasks() { m_tasks.clear(); }

        // override of base class
        bool config() override;
        bool initialize() override;
        bool finalize() override;
        DLElement* find(const std::string& name) override;
        void remove(const std::string& name) override;


    private:

        DleSupervisor           m_tasks;
};

#endif
