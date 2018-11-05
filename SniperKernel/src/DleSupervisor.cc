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

#include "SniperKernel/DleSupervisor.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include "SniperKernel/SniperLog.h"
#include <algorithm>

DleSupervisor::DleSupervisor(const std::string& name)
    : NamedElement(name)
{
}

DleSupervisor::DleSupervisor(const std::string& scope, const std::string& name)
    : NamedElement(scope, name)
{
}

DleSupervisor::~DleSupervisor()
{
    this->clear();
}

bool DleSupervisor::initialize()
{
    //try initializing all objs even with errors
    bool res = true;
    for ( auto o : m_list ) {
        if ( ! o->initialize() ) {
            LogError << o->scope() << o->objName() << " initialize failed" << std::endl;
            res = false;
        }
    }
    return res;
}

bool DleSupervisor::finalize()
{
    //try finalizing all objs even with errors
    bool res = true;
    for ( auto o = m_list.rbegin(); o != m_list.rend(); ++o ) {
        if ( ! (*o)->finalize() ) {
            LogError << (*o)->scope() << (*o)->objName() << " finalize failed" << std::endl;
            res = false;
        }
    }
    return res;
}

void DleSupervisor::setLogLevel(int level)
{
    for ( auto o : m_list ) {
        o->setLogLevel(level);
    }
}

DLElement* DleSupervisor::find(const std::string& name)
{
    std::string::size_type pseg = name.find(".");
    if ( pseg == std::string::npos ) {
        auto r = m_map.find(name);
        if ( r != m_map.end() ) {
            return (*r).second.first;
        }
    }
    else {
        AlgBase* obj = dynamic_cast<AlgBase*>(this->find(name.substr(0, pseg)));
        if ( obj != nullptr ) {
            return obj->findTool(name.substr(pseg+1, std::string::npos));
        }
    }

    return nullptr;
}

bool DleSupervisor::append(DLElement* obj, bool owned)
{
    if ( m_map.find(obj->objName()) == m_map.end() ) {
        m_map.insert(std::make_pair(obj->objName(), std::make_pair(obj, owned)));
        m_list.push_back(obj);
        return true;
    }

    LogError << "Duplicated object " << obj->objName() << std::endl;
    return false;
}

bool DleSupervisor::remove(const std::string& name) 
{
    auto r = m_map.find(name);
    if ( r != m_map.end() ) {
        m_list.remove( (*r).second.first );
        if ( (*r).second.second ) delete (*r).second.first;
        m_map.erase(r);
        return true;
    }

    return false;
}

void DleSupervisor::clear() {
    std::for_each( m_list.rbegin(), m_list.rend(), [this](DLElement* o) {
            if ( m_map[o->objName()].second ) delete o;
            });
    m_list.clear();
    m_map.clear();
}
