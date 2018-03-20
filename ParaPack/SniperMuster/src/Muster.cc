/* Copyright (C) 2018 Jiaheng Zou <zoujh@ihep.ac.cn>
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

#include "Muster.h"
#include "TaskSupervisor.h"
#include "MusterContext.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperException.h"
#include "SniperKernel/SniperLog.h"
#include <tbb/task.h>
#include <tbb/task_scheduler_init.h>
#include <boost/python.hpp>

namespace bp = boost::python;

Muster::Muster()
    : m_threads(0)
{
    MusterContext::create();
    sniper_context.set(Sniper::SysMode::MT);
    m_supervisor = new(tbb::task::allocate_root()) TaskSupervisor();
}

Muster::~Muster()
{
    LogInfo << "releasing all SNiPER Tasks" << std::endl;
    m_children.clear();
    MusterContext::destroy();
}

void Muster::setEvtMax(int evtMax)
{
    MusterContext::instance().setEvtMax(evtMax);
}

bool Muster::config(bp::api::object& functor, unsigned int nthreads)
{
    for ( unsigned int i = 0; i < nthreads; ++i )
    {
        append(functor);
        ++m_threads;
    }

    return true;
}

bool Muster::append(boost::python::api::object& functor)
{
    bp::api::object obj = functor();
    bp::extract<Task&> xobj(obj);

    if ( xobj.check() )
    {
        m_children.push_back(obj);
        m_supervisor->intake(xobj().Snoopy());
    }
    else
    {
        throw ContextMsgException("Invalid Task object");
    }

    return true;
}

bool Muster::run()
{
    sniper_context.set_threads(m_threads);

    tbb::task_scheduler_init scheduler_init(m_threads);

    LogInfo << "totally open " << m_threads << " tbb threads" << std::endl;
    tbb::task::spawn_root_and_wait(*m_supervisor);

    return true;
}
