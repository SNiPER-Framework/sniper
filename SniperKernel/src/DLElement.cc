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

#include "SniperKernel/DLElement.h"
#include "SniperKernel/Task.h"

DLElement::DLElement(const std::string &name)
    : NamedElement(name),
      m_par(nullptr)
{
    declProp("LogLevel", m_logLevel);
}

DLElement::~DLElement()
{
}

Task *DLElement::getRoot()
{
    DLElement *p = this;
    while (p->m_par != nullptr)
    {
        p = p->m_par;
    }
    return dynamic_cast<Task *>(p);
}

void DLElement::setParent(Task *par)
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
    j["class"].from(m_tag);
    j["name"].from(m_name);

    SniperJSON &jprop = j["properties"];
    for (auto &p : m_pmgr.properties())
    {
        auto jv = p.second->json();
        if (jv.valid())
        {
            jprop.insert(p.first, jv.format(false));
        }
    }

    return j;
}

void DLElement::show()
{
    std::cout << json().str() << std::endl;
}
