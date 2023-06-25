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

#ifndef SNIPER_MT_SNIPER_H
#define SNIPER_MT_SNIPER_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/MtsMicroTaskQueue.h"
#include "SniperPrivate/MtsWorkerPool.h"

class MtSniper : public DLElement
{
public:
    // constructor and destructor
    MtSniper();
    virtual ~MtSniper();

    // implement the pure virtual interfaces in DLElement
    virtual bool initialize() override;
    virtual bool finalize() override;

    // public methods
    void setNumThreads(unsigned int nthreads) { m_nthrds = nthreads; }
    void setEvtMax(long evtMax) { m_evtMax = evtMax; }

    // set the primary micro task for MtSniper
    void setPrimaryTask(MtsMicroTask *task) { m_taskQueue->setPrimaryTask(task); }

    // set the SNiPER Tasks for data processing
    void setInputTask();
    void setOutputTask();
    void setWorkerTask();

    // create and start workers
    void run();

protected:
    // properties
    unsigned int m_nthrds;
    long m_evtMax;

    MtsMicroTaskQueue *m_taskQueue;
    MtsWorkerPool *m_workerPool;
};

#endif