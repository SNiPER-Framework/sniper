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

#ifndef SNIPER_DLE_SUPERVISOR_H
#define SNIPER_DLE_SUPERVISOR_H

#include "SniperKernel/NamedElement.h"
#include <list>
#include <map>
#include <bits/stl_pair.h>
#include <string>

class DLElement;

class DleSupervisor : public NamedElement
{
    public:

        DleSupervisor(const std::string& name);
        DleSupervisor(const std::string& scope, const std::string& name);
        virtual ~DleSupervisor();

        // proxies of DLElement operations
        bool initialize();
        bool finalize();
        void setLogLevel(int level);

        // find an object by its name
        DLElement* find(const std::string& name);

        // append an object into supervisor w/o ownership
        bool append(DLElement* obj, bool owned);

        // remove an object by its name
        bool remove(const std::string& name);

        // remove all objects in supervisor
        void clear();

        // return the object list
        const std::list<DLElement*>& list() { return m_list; }


    protected:

        std::list<DLElement*> m_list;
        std::map<std::string, std::pair<DLElement*, bool>> m_map;

        //deleted methods
        DleSupervisor() = delete;
        DleSupervisor(const DleSupervisor&) = delete;
        DleSupervisor& operator=(const DleSupervisor&) = delete;
};

#endif
