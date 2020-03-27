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

#include "SniperKernel/TaskWatchDog.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperException.h"
#include <map>

using Sniper::RunState;
using Sniper::StopRun;

static std::map<RunState, const char*> snoopy_o2str{
    { RunState::Invalid,    "((Invalid))" },
    { RunState::StartUp,    "((StartUp))" },
    { RunState::Ready,      "((Ready))" },
    { RunState::Running,    "((Running))" },
    { RunState::Stopped,    "((Stopped))" },
    { RunState::Finalized,  "((Finalized))" },
    { RunState::EndUp,      "((EndUp))" },
    { RunState::Error,      "((Error))" }
};

#define SNOOPY_PASS_AWAY(StateX) \
    if ( m_stat == StateX ) { \
        LogInfo << "already in state " << snoopy_o2str[m_stat] << std::endl; \
        return true; \
    }

#define SNOOPY_CHECK_ERR() \
    if ( isErr() ) { \
        LogWarn << __func__ << " ignored due to errors" << std::endl; \
        return false; \
    }

#define SNOOPY_WARN_MSG(StateX) \
    LogWarn << "invalid state tranform " << snoopy_o2str[ real_state() ] \
        << " => " << snoopy_o2str[StateX] << std::endl


TaskWatchDog::TaskWatchDog(Task* task)
    : m_stat(RunState::Invalid),
      m_task(*task)
{
}

TaskWatchDog::~TaskWatchDog()
{
    if ( isErr() ) {
        std::string msg = " ** ";
        msg += m_task.scope();
        msg += m_task.objName();
        msg += " **  Terminated with Error";
        sniper_context.reg_msg(msg);
    }
}

bool TaskWatchDog::config()
{
    if ( m_stat == RunState::Invalid ) {
        m_stat = RunState::StartUp;
        return m_task.config();
    }

    SNOOPY_PASS_AWAY(RunState::StartUp);
    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::StartUp);
    return false;
}

bool TaskWatchDog::initialize()
{
    if ( m_stat == RunState::StartUp ) {
        m_stat = RunState::Ready;
        return m_task.initialize();
    }

    SNOOPY_PASS_AWAY(RunState::Ready);
    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::Ready);
    return false;
}

bool TaskWatchDog::run()
{
    if ( m_stat == RunState::Ready ) {
        m_stat = RunState::Running;
        bool res = true;
        try {
            while ( res && m_stat == RunState::Running ) {
                res = m_task.execute();
            }
        }
        catch (StopRunProcess& e) {
            if ( ! m_task.isRoot() ) throw e;
        }
        return res;
    }

    SNOOPY_PASS_AWAY(RunState::Running);
    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::Running);
    return false;
}

bool TaskWatchDog::pause()
{
    if ( m_stat == RunState::Running ) {
        m_stat = RunState::Ready;
        return true;
    }

    SNOOPY_PASS_AWAY(RunState::Ready);
    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::Ready);
    return false;
}

bool TaskWatchDog::stop(StopRun mode)
{
    if ( m_stat == RunState::Running || m_stat == RunState::Ready ) {
        if ( mode == StopRun::ThisEvent ) {
            throw StopRunThisEvent();
        }

        m_stat = RunState::Stopped;

        if ( mode == StopRun::Promptly ) {
            throw StopRunProcess();
        }

        return true;  // mode == StopRun::Peacefully
    }

    SNOOPY_PASS_AWAY(RunState::Stopped);
    SNOOPY_PASS_AWAY(RunState::Finalized);
    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::Stopped);
    return false;
}

bool TaskWatchDog::finalize()
{
    const RunState real_stat = real_state();
    if ( real_stat == RunState::Stopped || real_stat == RunState::Ready ) {
        bool errFlag = isErr();
        m_stat = RunState::Finalized;
        if ( errFlag ) {
            this->setErr();
            LogWarn << "try to finalize within error" << std::endl;
        }
        m_task.finalize();
        return isErr();
    }
    else if ( real_stat == RunState::Finalized ) {
        LogInfo << "already in state " << snoopy_o2str[RunState::Finalized] << std::endl;
        return isErr();
    }

    SNOOPY_WARN_MSG(RunState::Finalized);

    return false;
}

bool TaskWatchDog::terminate()
{
    const RunState real_stat = real_state();
    if ( real_stat == RunState::Running || real_stat == RunState::Ready ) {
        LogDebug << "try to finalize before terminate" << std::endl;
        this->finalize();
    }
    else if ( real_stat == RunState::EndUp ) {
        return isErr();
    }

    typedef Sniper::RunStateInt S_Int;
    if ( (S_Int)(m_stat) & (S_Int)(RunState::Finalized) ) {
        bool errFlag = isErr();
        m_stat = RunState::EndUp;
        m_task.reset();
        if ( errFlag ) {
            this->setErr();
        }
        return isErr();
    }

    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::EndUp);
    return false;
}

bool TaskWatchDog::run_once()
{
    if ( m_stat == RunState::Ready ) {
        //m_stat = RunState::Running;
        return m_task.execute();
        //m_stat = RunState::Ready;
    }

    SNOOPY_PASS_AWAY(RunState::Running);
    if ( m_stat == RunState::Stopped ) {
        return false;
    }
    SNOOPY_CHECK_ERR();
    SNOOPY_WARN_MSG(RunState::Running);
    return false;
}

void TaskWatchDog::reset()
{
    m_stat = RunState::Invalid;
    m_task.reset();
}

void TaskWatchDog::setErr()
{
    typedef Sniper::RunStateInt S_Int;
    m_stat = (RunState)((S_Int)(RunState::Error) | (S_Int)(m_stat) );
}

bool TaskWatchDog::isErr()
{
    typedef Sniper::RunStateInt S_Int;
    return ((S_Int)(m_stat) & (S_Int)(RunState::Error));
}

Sniper::RunState TaskWatchDog::real_state()
{
    typedef Sniper::RunStateInt S_Int;
    if ( isErr() ) {
        return (RunState)((S_Int)(RunState::Error) ^ (S_Int)(m_stat));
    }
    else {
        return m_stat;
    }
}

int TaskWatchDog::logLevel()
{
    return m_task.logLevel();
}

const std::string& TaskWatchDog::scope()
{
    return m_task.scope();
}

const std::string& TaskWatchDog::objName()
{
    return m_task.objName();
}
