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

#ifndef SNIPER_I_INCIDENT_HANDLER_H
#define SNIPER_I_INCIDENT_HANDLER_H

#include <string>
#include <list>

class IExecUnit;
class Incident;

class IIncidentHandler
{
public:
    IIncidentHandler(IExecUnit &domain);
    IIncidentHandler(IExecUnit *domain);

    virtual ~IIncidentHandler();

    virtual bool handle(Incident &incident) = 0;

    bool regist(const std::string &incident);

    void unregist(const std::string &incident);

    void listening();

    const std::string &objName() { return m_name; }
    const std::string &scope() { return m_scope; }

protected:
    IExecUnit &m_domain;
    std::string m_name;

private:
    std::string m_scope;
    long m_id;
    std::list<std::string> m_msg;
};

#endif
