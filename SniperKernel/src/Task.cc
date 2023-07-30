/* Copyright (C) 2018-2021
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

#include "SniperKernel/Task.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperException.h"
#include "SniperKernel/DeclareDLE.h"
#include "SniperPrivate/MtsInterAlgDag.h"
#include "SniperPrivate/TaskProperty.h"
#include "SniperPrivate/WhiteBoard.h"

SNIPER_DECLARE_DLE(Task);

Task::Task(const std::string &name)
    : ExecUnit(name),
      m_limited(false),
      m_evtMax(-1),
      m_done(0),
      m_beginEvt("BeginEvent"),
      m_endEvt("EndEvent"),
      m_beginAlg("BeginAlg"),
      m_endAlg("EndAlg")
{
    if (m_tag.empty())
        m_tag = "Task"; //protection for derived classes

    declProp("EvtMax", m_evtMax);
    m_pmgr.addProperty(new TaskProperty("svcs", this));
    m_pmgr.addProperty(new TaskProperty("algs", this));
}

Task::~Task()
{
}

void Task::setEvtMax(long evtMax_)
{
    m_evtMax = evtMax_;
    m_limited = (m_evtMax >= 0);
}

void Task::setLogLevel(int level)
{
    DLElement::setLogLevel(level);

    for (auto target : m_targets)
    {
        target->setLogLevel(level);
    }
}

bool Task::run()
{
    if (sniper_context->check(Sniper::SysMode::MT))
    {
        LogWarn << "please use Muster::run() instead" << std::endl;
        return true;
    }

    if (m_snoopy->config())
    {
        if (m_snoopy->initialize())
        {
            if (!m_snoopy->run())
            {
                //LogError << "Failed to execute algorithms" << std::endl;
            }
            m_snoopy->finalize();
        }
    }

    return !m_snoopy->isErr();
}

bool Task::stop(Sniper::StopRun mode)
{
    return m_snoopy->stop(mode);
}

void Task::reset()
{
    m_evtMax = -1;
    m_done = 0;
    m_limited = false;

    ExecUnit::reset();
}

SniperJSON Task::json()
{
    static SniperJSON keys = SniperJSON().from(std::vector<std::string>{
        "sniper",
        "description",
        "identifier",
        "properties",
        "services",
        "algorithms",
        "algDAG"});

    SniperJSON j = ExecUnit::json();
    j.insert("ordered_keys", keys);

    if (m_interAlgConcurrency)
    {
        j.insert("algDAG", DAG()->json());
    }

    return j;
}

void Task::eval(const SniperJSON &json)
{
    //eval for base class
    ExecUnit::eval(json);
    //set event number limitation
    m_limited = (m_evtMax >= 0);

    if (json.find("algDAG") != json.map_end())
    {
        enableInterAlgConcurrency();
        DAG()->eval(json["algDAG"]);
    }
}

void Task::enableInterAlgConcurrency()
{
    setSnoopy(new MtsInterAlgDag(this));
    m_interAlgConcurrency = true;
}

MtsInterAlgDag *Task::DAG()
{
    return dynamic_cast<MtsInterAlgDag *>(m_snoopy);
}

bool Task::config()
{
    WhiteBoard::mark(this);
    return true;
}

bool Task::initialize()
{
    bool stat = true;
    if (!m_svcs.initialize())
        stat = false;
    if (!m_algs.initialize())
        stat = false;

    if (stat)
    {
        LogInfo << "initialized" << std::endl;
    }
    else
    {
        m_snoopy->setErr();
    }

    return stat;
}

bool Task::finalize()
{
    bool stat = true;
    if (!m_algs.finalize())
        stat = false;
    if (!m_svcs.finalize())
        stat = false;

    LogInfo << "events processed " << m_done << std::endl;

    if (!stat)
    {
        m_snoopy->setErr();
    }

    return stat;
}

bool Task::execute()
{
    if (m_limited && m_done >= m_evtMax)
    {
        m_snoopy->stop();
        return true;
    }

    try
    {
        if (m_snoopy->state() == Sniper::RunState::Stopped)
            return true;
        if (m_snoopy->isErr())
            return false;
        //BeginEvent is fired
        m_beginEvt.load(m_done).fire(*this);
        for (auto alg : m_algs.list())
        {
            auto _alg = dynamic_cast<AlgBase *>(alg);
            //BeginAlg is fired
            ScopedIncidentsPair sis{m_beginAlg.load(_alg), m_endAlg.load(_alg), *this};
            if (_alg->execute())
                continue;
            throw SniperException(alg->scope() + alg->objName() + " execute failed");
            //EndAlg is fired even there is an exception
        }
        //EndEvent is fired except there is an exception
        m_endEvt.load(m_done).fire(*this);
    }
    catch (StopRunThisEvent &e)
    {
        LogDebug << "stop current event and continue next one" << std::endl;
    }
    catch (StopRunProcess &e)
    {
        LogInfo << "stop run promptly." << std::endl;
        throw e;
    }
    catch (std::exception &e)
    {
        m_snoopy->setErr();
        LogError << e.what() << std::endl;
    }
    catch (...)
    {
        m_snoopy->setErr();
        LogError << "catch an unknown exception" << std::endl;
    }

    bool stat = !m_snoopy->isErr();
    if (stat)
    {
        ++m_done;
    }
    return stat;
}