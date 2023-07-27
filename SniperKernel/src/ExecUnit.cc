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

#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/Sniper.h"
#include "SniperPrivate/DLEFactory.h"
#include "SniperPrivate/SharedElemMgr.h"

ExecUnit::ExecUnit(const std::string &name)
    : DLElement(name),
      m_snoopy(new TaskWatchDog(this)),
      m_svcs(name, "services"),
      m_algs(name, "algorithms"),
      m_targets{&m_svcs, &m_algs}
{
    this->createSvc("DataMemSvc");
}

ExecUnit::~ExecUnit()
{
    m_snoopy->terminate();
    delete m_snoopy;

    reset();
}

SvcBase *ExecUnit::findSvc(const std::string &svcName)
{
    return dynamic_cast<SvcBase *>(m_svcs.find(svcName));
}

AlgBase *ExecUnit::findAlg(const std::string &algName)
{
    return dynamic_cast<AlgBase *>(m_algs.find(algName));
}

SvcBase *ExecUnit::createSvc(const std::string &svcName)
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

AlgBase *ExecUnit::createAlg(const std::string &algName)
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

SvcBase *ExecUnit::addSvc(SvcBase *svc)
{
    if (m_svcs.append(svc, false))
    {
        svc->setParent(this);
        return svc;
    }
    return nullptr;
}

AlgBase *ExecUnit::addAlg(AlgBase *alg)
{
    if (m_algs.append(alg, false))
    {
        alg->setParent(this);
        return alg;
    }
    return nullptr;
}

DLElement *ExecUnit::find(const std::string &name)
{
    for (auto target : m_targets)
    {
        if (auto res = target->find(name))
            return res;
    }

    LogWarn << "Cann't find Object " << name << std::endl;
    return nullptr;
}

DLElement *ExecUnit::create(const std::string &type, const std::string &name)
{
    if (type == "svc")
    {
        return createSvc(name);
    }
    else if (type == "alg")
    {
        return createAlg(name);
    }
    else
    {
        LogFatal << "can not create " << name << " with type " << type << std::endl;
    }
    return nullptr;
}

void ExecUnit::remove(const std::string &name)
{
    for (auto target : m_targets)
    {
        if (target->remove(name))
            return;
    }

    LogWarn << "Cannot remove, no such element " << name << std::endl;
}

void ExecUnit::reset()
{
    for (auto it = m_targets.rbegin(); it != m_targets.rend(); ++it)
    {
        (*it)->clear();
    }
}

SniperJSON ExecUnit::json()
{
    SniperJSON j = DLElement::json();

    if (isRoot())
    {
        j.insert("sniper", SniperJSON(Sniper::Config::json_str()));
        auto &jsniper = j["sniper"];
        if (jsniper.find("LoadDlls") != jsniper.map_end())
            jsniper["LoadDlls"].format(false);
    }

    for (auto target : m_targets)
    {
        SniperJSON &jcomponents = j[target->objName()];
        for (auto obj : target->list())
        {
            if (obj->tag() != "DataMemSvc")
            {
                jcomponents.push_back(obj->json());
            }
        }
        if (!jcomponents.valid())
        {
            jcomponents = SniperJSON::loads("[]");
        }
    }

    return j;
}

void ExecUnit::eval(const SniperJSON &json)
{
    //eval for base class
    DLElement::eval(json);

    //eval the services
    auto &svcs = json["services"];
    for (auto it = svcs.vec_begin(); it != svcs.vec_end(); ++it)
    {
        auto idStr = (*it)["identifier"].get<std::string>();
        if (idStr.front() != '[')
        {
            SvcBase *svc = this->createSvc(idStr);
            svc->eval(*it);
        }
        else
        {
            SvcBase *svc = dynamic_cast<SvcBase *>(SharedElemMgr::get(idStr));
            this->addSvc(svc);
        }
    }

    //eval the algorithms
    auto &algs = json["algorithms"];
    for (auto it = algs.vec_begin(); it != algs.vec_end(); ++it)
    {
        auto idStr = (*it)["identifier"].get<std::string>();
        if (idStr.front() != '[')
        {
            AlgBase *alg = this->createAlg(idStr);
            alg->eval(*it);
        }
        else
        {
            AlgBase *alg = dynamic_cast<AlgBase *>(SharedElemMgr::get(idStr));
            this->addAlg(alg);
        }
    }
}

void ExecUnit::setSnoopy(TaskWatchDog *snoopy)
{
    delete m_snoopy;
    m_snoopy = snoopy;
}