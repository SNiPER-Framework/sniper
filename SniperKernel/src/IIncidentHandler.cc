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

#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/IExecUnit.h"
#include "SniperKernel/SniperLog.h"
#include "SniperPrivate/IncidentMgr.h"
#include <algorithm>

IIncidentHandler::IIncidentHandler(IExecUnit &domain)
    : m_domain(domain),
      m_name("IncidentHandler"),
      m_scope(domain.scope() + domain.objName() + ':'),
      m_id(long(domain.getRoot()))
{
}

IIncidentHandler::IIncidentHandler(IExecUnit* domain)
    : m_domain(*domain),
      m_name("IncidentHandler"),
      m_scope(domain->scope() + domain->objName() + ':'),
      m_id(long(domain->getRoot()))
{
}

IIncidentHandler::~IIncidentHandler()
{
    std::list<std::string>::reverse_iterator it = m_msg.rbegin();
    while ( it != m_msg.rend() ) {
        this->unregist(*it);
        it = m_msg.rbegin();
    }
}

bool IIncidentHandler::regist(const std::string& incident)
{
    std::string msg = m_domain.isRoot()
        ? incident : (m_domain.objName() + ':' + incident);
    std::list<std::string>::iterator it =
        std::find(m_msg.begin(), m_msg.end(), msg);

    if ( it == m_msg.end() ) {
        m_msg.push_back(msg);
        IncidentMgr::instance(m_id).regist(msg, this);
    }
    else {
        LogInfo << "ignore already registered incident: "
                << incident << std::endl;
    }

    return true;
}

void IIncidentHandler::unregist(const std::string& incident)
{
    std::list<std::string>::iterator it =
        std::find(m_msg.begin(), m_msg.end(), incident);
    if ( it != m_msg.end() ) {
        m_msg.erase(it);
        IncidentMgr::instance(m_id).unregist(incident, this);
    }
}

void IIncidentHandler::listening()
{
    std::cout << "listening:";
    std::list<std::string>::iterator it = m_msg.begin();
    while ( it != m_msg.end() ) {
        std::cout << "  " << *it;
        ++it;
    }
    std::cout << std::endl;
}
