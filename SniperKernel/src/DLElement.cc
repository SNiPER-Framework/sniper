/* Copyright (C) 2018-2023
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

#include "SniperKernel/DLElement.h"
#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/DataMemSvc.h"

DLElement::DLElement(const std::string &name)
    : NamedElement(name),
      m_par(nullptr)
{
    declProp("LogLevel", m_logLevel);
}

DLElement::~DLElement()
{
}

ExecUnit *DLElement::getRoot()
{
    DLElement *p = this;
    while (p->m_par != nullptr)
    {
        p = p->m_par;
    }
    return dynamic_cast<ExecUnit *>(p);
}

void DLElement::setScopeString(const std::string &scope_)
{
    if (m_par == nullptr)
    {
        m_scope = scope_;
    }
    else
    {
        LogWarn << "this object has a parent, cannot set its scope string manually" << std::endl;
    }
}

void DLElement::setProperties(const SniperJSON &json)
{
    for (auto it = json.map_begin(); it != json.map_end(); ++it)
    {
        this->property(it->first)->set(it->second.str(-1));
    }
}

void DLElement::setParent(ExecUnit *par)
{
    m_logLevel = par->logLevel();
    m_scope = par->scope() + par->objName() + ':';
    m_par = par;
}

Property *DLElement::property(const std::string &key)
{
    return m_pmgr.property(key);
}

SniperJSON DLElement::json()
{
    SniperJSON j;
    j["identifier"].from(m_tag + '/' + m_name);

    if (!m_description.empty())
    {
        j["description"].from(m_description);
    }

    SniperJSON &jprop = j["properties"];
    for (auto &p : m_pmgr.properties())
    {
        auto jv = p.second->json();
        if (jv.valid())
        {
            jprop.insert(p.first, jv.format(false));
        }
    }

    if (m_par != nullptr && m_logLevel == m_par->logLevel())
    {
        jprop.erase("LogLevel");
        if (jprop.size() == 0)
        {
            j.erase("properties");
        }
    }

    return j;
}

void DLElement::eval(const SniperJSON &json)
{
    std::string jid = json["identifier"].get<std::string>();
    std::string::size_type jsep = jid.find('/');
    std::string type = (jsep == std::string::npos) ? jid : jid.substr(0, jsep);
    if (type != m_tag)
    {
        static const char *_errmsg = "mismatched types while eval json";
        LogFatal << type << " and " << m_tag << ": " << _errmsg << std::endl;
        throw ContextMsgException(_errmsg);
    }

    auto idest = json.find("description");
    if (idest != json.map_end())
    {
        setDescription(idest->second.get<std::string>());
    }

    idest = json.find("properties");
    if (idest != json.map_end())
    {
        this->setProperties(idest->second);
    }
}

void DLElement::show()
{
    std::cout << json().str(2) << std::endl;
}

DLElement *DLElement::findRecursivelyUpToRoot(const std::string &name)
{
    DLElement *res = nullptr;
    if (auto p = m_par)
    {
        res = p->find(name);
        while (res == nullptr && (p = p->getParent()))
        {
            res = p->find(name);
        }
    }
    return res;
}

IDataBlock *DLElement::findDataRecursivelyUpToRoot(const std::string &path)
{
    IDataBlock *res = nullptr;
    if (auto p = m_par)
    {
        res = (p->dataSvc())->find(path);
        while (res == nullptr && (p = p->getParent()))
        {
            res = (p->dataSvc())->find(path);
        }
    }
    return res;
}
