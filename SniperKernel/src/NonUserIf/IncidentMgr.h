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

#ifndef SNIPER_INCIDENT_MGR_H
#define SNIPER_INCIDENT_MGR_H

#include <string>
#include <map>
#include <list>

class Incident;
class IIncidentHandler;
class Task;

class IncidentMgr
{
    public :

        friend class Incident;
        friend class IIncidentHandler;

        static IncidentMgr& instance(Task& root);
        static IncidentMgr& instance(long id);

        static void create(Task& root);
        static void release();

    private :

        /// return the number of handlers for the incident, -1 in case of error
        int  handle(Incident& incident);
        void regist(const std::string& incident, IIncidentHandler* handler);
        void unregist(const std::string& incident, IIncidentHandler* handler);

        const std::string& scope() { return m_scope; }
        const std::string& objName() { return m_name; }

        IncidentMgr();
        ~IncidentMgr();

        typedef std::list<IIncidentHandler*>  HandlerList;
        typedef std::map<std::string, HandlerList>  IncidentMap;

        std::string  m_scope;
        std::string  m_name;
        IncidentMap  m_map;
};

#endif
