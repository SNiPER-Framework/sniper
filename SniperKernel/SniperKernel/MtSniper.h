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

#include "SniperKernel/Task.h"
#include "SniperKernel/MtsMicroTaskQueue.h"
#include <condition_variable>

class MtSniper : public DLElement
{
public:
    // constructor and destructor
    MtSniper(const std::string &name = "MtSniper");
    virtual ~MtSniper();

    // public methods
    void setNumThreads(unsigned int nthreads) { m_nthrds = nthreads; }
    void setEvtMax(long evtMax) { m_evtMax = evtMax; }
    void configGlobalBuffer(int capacity, int threshold);

    // set the primary micro task (not a SNiPER Task) for MtSniper
    void setPrimaryTask(MtsMicroTask *task);

    // create or get the SNiPER Tasks for data processing
    Task *createInputTask(const std::string &identifier);
    Task *createOutputTask(const std::string &identifier);
    Task *createMainTask(const std::string &identifier);
    Task *getInputTask() { return m_itask; }
    Task *getOutputTask() { return m_otask; }
    Task *getMainTask() { return m_mtask; }
    // the interfaces for json
    virtual SniperJSON json() override;
    virtual void eval(const SniperJSON &json) override;

    // create and start workers
    bool run();

    // notify and wait for the initialize of all SNiPER Tasks
    static void notifyInitialized() { s_initialized.notify_one(); }
    static void wait4Initialize();

protected:
    Task *createSniperTask(const std::string &identifier);

    // implement the pure virtual interfaces in DLElement
    virtual bool initialize() override;
    virtual bool finalize() override;

    // properties
    unsigned int m_nthrds;
    long m_evtMax;
    int m_gbufCapacity;
    int m_gbufThreshold;

    bool m_hasExternalPrimaryTask{false};

    Task *m_itask{nullptr};
    Task *m_otask{nullptr};
    Task *m_mtask{nullptr};
    std::atomic_flag m_ilock{ATOMIC_FLAG_INIT};
    std::atomic_flag m_olock{ATOMIC_FLAG_INIT};

    MtsMicroTaskQueue *m_microTaskQueue;
    SniperObjPool<Task> *m_sniperTaskPool; // for all the main task copies

    static std::condition_variable s_initialized;
};

#endif
