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

#ifndef SNIPER_TASK_H
#define SNIPER_TASK_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/DleSupervisor.h"
#include "SniperKernel/TaskWatchDog.h"
#include "SniperKernel/Incident.h"

//Pre-declarations
class SvcBase;
class AlgBase;

//Task definition
class Task : public DLElement
{
    public :

        //Constructor
        Task(const std::string& name);

        //Destructor
        virtual ~Task();

        //run this task
        virtual bool run();

        //stop this task
        virtual bool stop();

        //ooo~~ just as its name
        TaskWatchDog& Snoopy() { return m_snoopy; }

        //for compatibility
        bool isRoot() { return m_par == nullptr; }

    protected :
        //none state check... Please use Snoopy for state control
        friend class TaskWatchDog;
        //the concrete task operations
        virtual bool config();
        virtual bool initialize();
        virtual bool finalize();
        virtual bool execute();
        //clear all svcs and algs
        void reset();

    public :
        //set the log level of this domain
        void setLogLevel(int level) final;

        //set and get the max event number to be processed
        void setEvtMax(int evtMax);
        int  evtMax() { return m_evtMax; }

        //create owned Svc/Alg by name
        SvcBase* createSvc(const std::string& svcName);
        AlgBase* createAlg(const std::string& algName);

        //add concrete Svc/Alg to Task without ownership
        //it may be helpful for writing Svc/Alg in Python
        SvcBase* addSvc(SvcBase* svc);
        AlgBase* addAlg(AlgBase* alg);

        //find an element by its name
        virtual DLElement* find(const std::string& name);

        //remove an element with its name
        virtual void remove(const std::string& name);

        //clear all svcs/algs seperately
        void clearSvcs() { m_svcs.clear(); }
        void clearAlgs() { m_algs.clear(); }

        //override from base class DLElement
        void show(int indent) final;


    protected :

        void queue(DleSupervisor* target);

        //member data
        int                     m_evtMax;
        int                     m_done;
        TaskWatchDog            m_snoopy;
        DleSupervisor           m_svcs;
        DleSupervisor           m_algs;

    private :

        bool                    m_limited;
        Incident                m_beginEvt;
        Incident                m_endEvt;
        std::list<DleSupervisor*>  m_targets;
};

#endif
