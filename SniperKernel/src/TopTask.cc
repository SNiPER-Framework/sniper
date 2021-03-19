/* Copyright (C) 2021
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

//We can construct a complex tree of nested tasks for particular utilities.
//For example:
//                 TopTask      (only TopTask can has nested tasks)
//                 /  |   \.
//           TopTask Task TopTask
//           /  |  \       /    \.
//    TopTask Task Task  Task  TopTask
//     /   \                      |
//  Task   Task                  Task
//
//However, this is not encouraged.
//Do not use nested tasks more than 2 layers unless it's indeed necessary.

#include "SniperKernel/TopTask.h"
#include "SniperKernel/SniperLog.h"
#include "NonUserIf/DLEFactory.h"
#include "SniperKernel/DeclareDLE.h"

SNIPER_DECLARE_DLE(TopTask);

TopTask::TopTask(const std::string &name)
    : Task(name),
      m_tasks(name, "subtasks")
{
    m_tag = "TopTask";
    queue(&m_tasks);
}

TopTask::~TopTask()
{
}

Task *TopTask::createTask(const std::string &taskName)
{
    DLElement *obj = DLEFactory::instance().create(taskName);
    if (obj != nullptr)
    {
        Task *result = dynamic_cast<Task *>(obj);
        if (result != nullptr)
        {
            if (m_tasks.append(result, true))
            {
                result->setParent(this);
                return result;
            }
        }
        else
        {
            LogFatal << obj->objName() << " cannot cast to Task." << std::endl;
        }
        delete obj;
    }
    return nullptr;
}

Task *TopTask::addTask(Task *task)
{
    if (m_tasks.append(task, false))
    {
        task->setParent(this);
        return task;
    }
    return nullptr;
}

bool TopTask::config()
{
    bool stat = Task::config();

    for (auto obj : m_tasks.list())
    {
        Task *task = dynamic_cast<Task *>(obj);
        if (!task->Snoopy().config())
            return false;
    }

    return stat;
}

bool TopTask::initialize()
{
    bool stat = true;

    if (!Task::initialize())
        stat = false;

    for (auto obj : m_tasks.list())
    {
        Task *task = dynamic_cast<Task *>(obj);
        if (!task->Snoopy().initialize())
            stat = false;
    }

    return stat;
}

bool TopTask::finalize()
{
    bool stat = true;

    auto &tasks = m_tasks.list();
    for (auto it = tasks.rbegin(); it != tasks.rend(); ++it)
    {
        Task *task = dynamic_cast<Task *>(*it);
        if (!task->Snoopy().finalize())
            stat = false;
    }

    if (!Task::finalize())
        stat = false;

    return stat;
}

DLElement *TopTask::find(const std::string &name)
{
    std::string::size_type pseg = name.find(":");
    if (pseg == std::string::npos)
    {
        return Task::find(name);
    }

    Task *obj = dynamic_cast<Task *>(m_tasks.find(name.substr(0, pseg)));
    if (obj != nullptr)
    {
        return obj->find(name.substr(pseg + 1, std::string::npos));
    }

    LogWarn << "Cann't find Object " << name << std::endl;
    return nullptr;
}

void TopTask::remove(const std::string &name)
{
    std::string::size_type pseg = name.find(":");
    if (pseg == std::string::npos)
    {
        return Task::remove(name);
    }

    Task *obj = dynamic_cast<Task *>(m_tasks.find(name.substr(0, pseg)));
    if (obj != nullptr)
    {
        return obj->remove(name.substr(pseg + 1, std::string::npos));
    }

    LogWarn << "Cannot remove, no such element " << name << std::endl;
    return;
}
