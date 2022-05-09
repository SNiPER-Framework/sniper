/* Copyright (C) 2018-2022
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

#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/SniperLog.h"
#include "SniperPrivate/IncidentMgr.h"
#include <algorithm>

IIncidentHandler::IIncidentHandler(ExecUnit &domain)
    : NamedElement(domain.scope()+domain.objName()+':', "IncidentHandler"),
      m_domain(domain),
      m_id(long(domain.getRoot()))
{
    m_logLevel = domain.logLevel();
}

IIncidentHandler::IIncidentHandler(ExecUnit *domain)
    : NamedElement(domain->scope()+domain->objName()+':', "IncidentHandler"),
      m_domain(*domain),
      m_id(long(domain->getRoot()))
{
    m_logLevel = domain->logLevel();
}

IIncidentHandler::~IIncidentHandler()
{
    for (auto it = m_msg.rbegin(); it != m_msg.rend(); ++it)
    {
        IncidentMgr::instance(m_id).unregist(*it, this);
    }
}

bool IIncidentHandler::regist(const std::string &incident)
{
    std::string msg = m_domain.scope() + m_domain.objName() + ':' + incident;
    std::list<std::string>::iterator it =
        std::find(m_msg.begin(), m_msg.end(), msg);

    if (it == m_msg.end())
    {
        m_msg.push_back(msg);
        IncidentMgr::instance(m_id).regist(msg, this);
    }
    else
    {
        LogInfo << "ignore already registered incident: "
                << incident << std::endl;
    }

    return true;
}

void IIncidentHandler::unregist(const std::string &incident)
{
    std::string msg = m_domain.scope() + m_domain.objName() + ':' + incident;
    std::list<std::string>::iterator it =
        std::find(m_msg.begin(), m_msg.end(), msg);
    if (it != m_msg.end())
    {
        m_msg.erase(it);
        IncidentMgr::instance(m_id).unregist(msg, this);
    }
}

void IIncidentHandler::listening()
{
    std::cout << "listening:";
    std::list<std::string>::iterator it = m_msg.begin();
    while (it != m_msg.end())
    {
        std::cout << "  " << *it;
        ++it;
    }
    std::cout << std::endl;
}
