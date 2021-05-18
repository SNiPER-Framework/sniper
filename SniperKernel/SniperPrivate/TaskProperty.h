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

#ifndef SNIPER_TASK_PROPERTY_H
#define SNIPER_TASK_PROPERTY_H

#include "SniperKernel/Property.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SvcBase.h"
#include <iostream>

class TaskProperty : public SniperProperty<std::vector<std::string>>
{
public:
    typedef SniperProperty<std::vector<std::string>> BaseType;

    TaskProperty(const std::string &key_, ExecUnit *domain_)
        : BaseType(key_, m_names), m_domain(domain_)
    {
        if (key_ == "algs")
        {
            pclear = &ExecUnit::clearAlgs;
            pcreate = &TaskProperty::createAlg;
            padd = &TaskProperty::addAlg;
        }
        else if (key_ == "svcs")
        {
            pclear = &ExecUnit::clearSvcs;
            pcreate = &TaskProperty::createSvc;
            padd = &TaskProperty::addSvc;
        }
        else
        {
            throw ContextMsgException(key_ + " : invalid TaskProperty Key");
        }
    }

    bool set(const std::string &var)
    {
        if (!m_names.empty())
        {
            throw ContextMsgException("ERROR: set a non-empty TaskProperty");
        }
        return this->append(var);
    }

    bool append(const std::string &var)
    {
        unsigned int begin = m_names.size();
        if (!BaseType::append(var))
        {
            return false;
        }
        for (unsigned int i = begin; i < m_names.size(); ++i)
        {
            if (!(this->*pcreate)(m_names[i]))
            {
                return false;
            }
        }
        return true;
    }

    bool append(DLElement *obj)
    {
        return (this->*padd)(obj);
    }

    SniperJSON json() { return SniperJSON(); }

    void show() {}

private:
    bool createAlg(const std::string &name)
    {
        return m_domain->createAlg(name) != 0;
    }
    bool createSvc(const std::string &name)
    {
        return m_domain->createSvc(name) != 0;
    }
    bool addAlg(DLElement *alg)
    {
        return m_domain->addAlg(dynamic_cast<AlgBase *>(alg)) != 0;
    }
    bool addSvc(DLElement *svc)
    {
        return m_domain->addSvc(dynamic_cast<SvcBase *>(svc)) != 0;
    }

    void (ExecUnit::*pclear)();
    bool (TaskProperty::*pcreate)(const std::string &);
    bool (TaskProperty::*padd)(DLElement *);

    ExecUnit *m_domain;
    std::vector<std::string> m_names;
};

#endif
