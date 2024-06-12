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

#include "SniperKernel/MtsMicroTaskQueue.h"
#include "SniperKernel/MtSniperUtility.h"

MtsMicroTaskQueue *MtsMicroTaskQueue::s_instance = nullptr;

MtsMicroTaskQueue *MtsMicroTaskQueue::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new MtsMicroTaskQueue();
    }
    return s_instance;
}

void MtsMicroTaskQueue::destroy()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

MtsMicroTaskQueue::MtsMicroTaskQueue()
    : Sniper::Queue<MtsMicroTask *>(nullptr)
{
}

MtsMicroTaskQueue::~MtsMicroTaskQueue()
{
    if (m_tail->value)
    {
        delete m_tail->value;
    }
}

void MtsMicroTaskQueue::enqueue(MtsMicroTask *task)
{
    concurrentPush(task);
    MtSniperUtil::Thread::resumeOne();
}

void MtsMicroTaskQueue::enqueue(Sniper::Queue<MtsMicroTask *> &tasks)
{
    concurrentMerge(tasks);
    MtSniperUtil::Thread::resumeAll();
}

void MtsMicroTaskQueue::setPrimaryTask(MtsMicroTask *ptask)
{
    if (m_tail->value)
    {
        delete m_tail->value;
    }
    m_tail->value = ptask;
}