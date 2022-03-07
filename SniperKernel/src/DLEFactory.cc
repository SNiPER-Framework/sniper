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

#include "SniperKernel/DLElement.h"
#include "SniperPrivate/DLEFactory.h"
#include "SniperKernel/SniperException.h"

DLEFactory* DLEFactory::s_obj = nullptr;

DLEFactory& DLEFactory::instance()
{
    return (s_obj != nullptr) ? *s_obj : *(s_obj = new DLEFactory);
}

void DLEFactory::release()
{
    if ( s_obj ) {
        delete s_obj;
        s_obj = nullptr;
    }
}

DLElement* DLEFactory::create(const std::string& name)
{
    std::string type = name;
    std::string nobj = name;
    std::string::size_type pseg = name.find('/');
    if ( pseg != std::string::npos ) {
        type = type.substr(0, pseg);
        nobj = nobj.substr(pseg+1, std::string::npos);
    }

    Type2CreatorMap::iterator it = m_creators.find(type);
    if ( it != m_creators.end() ) {
        DLElement* result = (it->second)(nobj);
        result->setTag(type);
        return result;
    }

    std::string msg = type + " is not a valid DLE type";
    throw ContextMsgException(msg);
}

bool DLEFactory::book(const std::string& type, DLECreator creator)
{
    Type2CreatorMap::iterator it = m_creators.find(type);
    if ( it == m_creators.end() ) {
        m_types.push_back(type);
        m_creators.insert(std::make_pair(type, creator));
        return true;
    }

    std::string msg = type + " is a duplicated class name";
    throw ContextMsgException(msg);
}

DLEFactory::DLEFactory()
    : m_name("DLEFactory")
{
}

DLEFactory::~DLEFactory()
{
}
