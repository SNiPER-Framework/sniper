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

#include "SniperKernel/Task.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperException.h"
#include "SniperKernel/DeclareDLE.h"
#include "NonUserIf/TaskProperty.h"
#include "NonUserIf/WhiteBoard.h"
#include "NonUserIf/DLEFactory.h"

SNIPER_DECLARE_DLE(Task);

Task::Task(const std::string &name)
    : DLElement(name),
      m_evtMax(-1),
      m_done(0),
      m_snoopy(this),
      m_svcs(name, "services"),
      m_algs(name, "xalgorithms"), //'x'algs presents after svcs in json.str()
      m_limited(false),
      m_beginEvt("BeginEvent"),
      m_endEvt("EndEvent"),
      m_targets{&m_svcs, &m_algs}
{
    if (m_tag.empty())
        m_tag = "Task"; //protection for derived classes

    declProp("EvtMax", m_evtMax);
    m_pmgr.addProperty(new TaskProperty("svcs", this));
    m_pmgr.addProperty(new TaskProperty("algs", this));

    //TODO: should be optimized
    this->createSvc("DataMemSvc");
}

Task::~Task()
{
    m_snoopy.terminate();
}

bool Task::run()
{
    if (sniper_context.check(Sniper::SysMode::MT))
    {
        LogWarn << "please use Muster::run() instead" << std::endl;
        return true;
    }

    if (m_snoopy.config())
    {
        if (m_snoopy.initialize())
        {
            if (!m_snoopy.run())
            {
                //LogError << "Failed to execute algorithms" << std::endl;
            }
            m_snoopy.finalize();
        }
    }

    return !m_snoopy.isErr();
}

bool Task::stop(Sniper::StopRun mode)
{
    return m_snoopy.stop(mode);
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
        m_snoopy.setErr();
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
        m_snoopy.setErr();
    }

    return stat;
}

bool Task::execute()
{
    if (m_limited && m_done >= m_evtMax)
    {
        m_snoopy.stop();
        return true;
    }

    try
    {
        m_beginEvt.fire(*this);
        if (m_snoopy.state() == Sniper::RunState::Stopped)
            return true;
        if (m_snoopy.isErr())
            return false;
        for (auto alg : m_algs.list())
        {
            if (dynamic_cast<AlgBase *>(alg)->execute())
                continue;
            throw SniperException(alg->scope() + alg->objName() + " execute failed");
        }
        m_endEvt.fire(*this);
        if (m_snoopy.isErr())
            return false;
    }
    catch (StopRunThisEvent &e)
    {
        LogDebug << "stop current event and continue next one" << std::endl;
    }
    catch (StopRunProcess &e)
    {
        LogInfo << "stop run promtly." << std::endl;
        throw e;
    }
    catch (std::exception &e)
    {
        m_snoopy.setErr();
        LogError << e.what() << std::endl;
    }
    catch (...)
    {
        m_snoopy.setErr();
        LogError << "catch an unknown exception" << std::endl;
    }

    if (!m_snoopy.isErr())
    {
        ++m_done;
        return true;
    }
    return false;
}

void Task::reset()
{
    m_evtMax = -1;
    m_done = 0;
    m_limited = false;

    for (auto it = m_targets.rbegin(); it != m_targets.rend(); ++it)
    {
        (*it)->clear();
    }
    m_targets.clear();
}

void Task::setLogLevel(int level)
{
    DLElement::setLogLevel(level);

    for (auto target : m_targets)
    {
        target->setLogLevel(level);
    }
}

void Task::setEvtMax(int evtMax_)
{
    m_evtMax = evtMax_;
    m_limited = (m_evtMax >= 0);
}

SvcBase *Task::createSvc(const std::string &svcName)
{
    DLElement *obj = DLEFactory::instance().create(svcName);
    if (obj != nullptr)
    {
        SvcBase *result = dynamic_cast<SvcBase *>(obj);
        if (result != nullptr)
        {
            if (m_svcs.append(result, true))
            {
                result->setParent(this);
                return result;
            }
        }
        else
        {
            LogFatal << obj->objName() << " cannot cast to SvcBase."
                     << std::endl;
        }
        delete obj;
    }
    return nullptr;
}

AlgBase *Task::createAlg(const std::string &algName)
{
    DLElement *obj = DLEFactory::instance().create(algName);
    if (obj != nullptr)
    {
        AlgBase *result = dynamic_cast<AlgBase *>(obj);
        if (result != nullptr)
        {
            if (m_algs.append(result, true))
            {
                result->setParent(this);
                return result;
            }
        }
        else
        {
            LogFatal << obj->objName() << " cannot cast to AlgBase."
                     << std::endl;
        }
        delete obj;
    }
    return nullptr;
}

SvcBase *Task::addSvc(SvcBase *svc)
{
    if (m_svcs.append(svc, false))
    {
        svc->setParent(this);
        return svc;
    }
    return nullptr;
}

AlgBase *Task::addAlg(AlgBase *alg)
{
    if (m_algs.append(alg, false))
    {
        alg->setParent(this);
        return alg;
    }
    return nullptr;
}

DLElement *Task::find(const std::string &name)
{
    for (auto target : m_targets)
    {
        if (auto res = target->find(name))
            return res;
    }

    LogWarn << "Cann't find Object " << name << std::endl;
    return nullptr;
}

void Task::remove(const std::string &name)
{
    for (auto target : m_targets)
    {
        if (target->remove(name))
            return;
    }

    LogWarn << "Cannot remove, no such element " << name << std::endl;
}

SniperJSON Task::json()
{
    SniperJSON j = DLElement::json();

    for (auto target : m_targets)
    {
        SniperJSON &jcomponents = j[target->objName()];
        for (auto obj : target->list())
        {
            jcomponents.push_back(obj->json());
        }
        if (!jcomponents.valid())
        {
            jcomponents = SniperJSON::loads("[]");
        }
    }

    return j;
}

void Task::queue(DleSupervisor *target)
{
    m_targets.push_back(target);
}
