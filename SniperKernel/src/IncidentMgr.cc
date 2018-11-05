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

#include "NonUserIf/IncidentMgr.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperLog.h"
#include <algorithm>
#include <map>

static std::map<long, IncidentMgr*> s_mgrMap;

IncidentMgr& IncidentMgr::instance(Task& root)
{
    return *s_mgrMap[long(&root)];
}

IncidentMgr& IncidentMgr::instance(long id)
{
    return *s_mgrMap[id];
}

void IncidentMgr::create(Task& root)
{
    long id = long(&root);
    if ( s_mgrMap[id] == nullptr ) {
        s_mgrMap[id] = new IncidentMgr;
        s_mgrMap[id]->m_scope = root.scope();
    }
}

void IncidentMgr::release()
{
    for ( auto i : s_mgrMap ) {
        delete i.second;
    }
    s_mgrMap.clear();
}

int IncidentMgr::handle(Incident& incident)
{
    //LogTest << "Processing incident " << incident.name() << std::endl;
    int count = 0;
    IncidentMap::iterator ii = m_map.find(incident.name());
    if ( ii != m_map.end() ) {
        HandlerList& handlers = ii->second;
        for ( auto& ih : handlers ) {
            if ( ! ih->handle(incident) ) {
                return -1;
            }
        }
        count = handlers.size();
    }
    return count;
}

void IncidentMgr::regist(const std::string& incident, IIncidentHandler* handler)
{
    m_map[incident].push_back(handler);
}

void IncidentMgr::unregist(const std::string& incident, IIncidentHandler* handler)
{
    IncidentMap::iterator ii = m_map.find(incident);
    if ( ii != m_map.end() ) {
        HandlerList& handlers = ii->second;
        auto ih = std::find(handlers.begin(), handlers.end(), handler);
        if ( ih != handlers.end() ) {
            handlers.erase(ih);
        }
        if ( handlers.empty() ) {
            m_map.erase(ii);
        }
    }
}

IncidentMgr::IncidentMgr()
    : m_name("IncidentMgr")
{
}

IncidentMgr::~IncidentMgr()
{
}
