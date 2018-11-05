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

#include "SniperKernel/PropertyMgr.h"
#include "SniperKernel/Property.h"

PropertyMgr::PropertyMgr(DLElement* par)
    : m_par(par)
{
}

PropertyMgr::~PropertyMgr()
{
    std::map<std::string, Property*>::iterator it = m_dict.end();
    while ( it != m_dict.begin() ) {
        delete (--it)->second;
    }
}

Property* PropertyMgr::property(const std::string& key)
{
    std::map<std::string, Property*>::iterator it = m_dict.find(key);
    if ( it != m_dict.end() ) {
        return it->second;
    }
    throw ContextMsgException(key + " : invalid Property Key");
}

bool PropertyMgr::addProperty(Property* property)
{
    std::string key = property->key();
    std::map<std::string, Property*>::iterator it = m_dict.find(key);
    if ( it == m_dict.end() ) {
        m_dict[key] = property;
        return true;
    }
    throw ContextMsgException(key + " : duplicated Property Key");
}
