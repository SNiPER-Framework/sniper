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

#ifndef SNIPER_PROPERTY_MGR_H
#define SNIPER_PROPERTY_MGR_H

#include "SniperKernel/Property.h"
#include <vector>
#include <map>
#include <string>

class DLElement;

class PropertyMgr
{
    public :

        typedef std::map<std::string, Property*> PropertyMap;

        PropertyMgr(DLElement* par);

        //this class will not used for inheriting
        ~PropertyMgr();

        //for SniperLog convenience
        const char* objName() { return "PropertyMgr"; }

        //get property by key
        Property* property(const std::string& key);

        //get all properties at one time
        const PropertyMap& properties() { return m_dict; }

        //add a simple property
        template<typename Type>
        bool addProperty(const std::string& key, Type& var);

        //add a vector property
        template<typename Type>
        bool addProperty(const std::string& key, std::vector<Type>& var);

        //add a map property
        template<typename K, typename V>
        bool addProperty(const std::string& key, std::map<K, V>& var);

        //take over a property pointer
        bool addProperty(Property* property);

    private :

        DLElement*  m_par;

        std::map<std::string, Property*> m_dict;
};

template<typename Type>
bool PropertyMgr::addProperty(const std::string& key, Type& var)
{
    std::map<std::string, Property*>::iterator it = m_dict.find(key);
    if ( it == m_dict.end() ) {
        m_dict[key] = new SniperProperty<Type>(key, var);
        return true;
    }
    throw ContextMsgException(key + " : duplicated Property Key");
}

template<typename Type>
bool PropertyMgr::addProperty(const std::string& key, std::vector<Type>& var)
{
    std::map<std::string, Property*>::iterator it = m_dict.find(key);
    if ( it == m_dict.end() ) {
        m_dict[key] = new SniperProperty<std::vector<Type> >(key, var);
        return true;
    }
    throw ContextMsgException(key + " : duplicated Property Key");
}

template<typename K, typename V>
bool PropertyMgr::addProperty(const std::string& key, std::map<K, V>& var)
{
    std::map<std::string, Property*>::iterator it = m_dict.find(key);
    if ( it == m_dict.end() ) {
        m_dict[key] = new SniperProperty<std::map<K, V> >(key, var);
        return true;
    }
    throw ContextMsgException(key + " : duplicated Property Key");
}

#endif
