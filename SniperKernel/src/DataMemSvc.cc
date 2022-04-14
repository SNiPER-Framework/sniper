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

#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/IDataBlock.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(DataMemSvc);

DataMemSvc::DataMemSvc(const std::string& name)
    : SvcBase(name)
{
}

DataMemSvc::~DataMemSvc()
{
}

IDataBlock* DataMemSvc::find(const std::string& name)
{
    auto it = m_mems.find(name);
    if ( it != m_mems.end() ) {
        return it->second.first;
    }
    return (IDataBlock*)0;
}

bool DataMemSvc::regist(const std::string& name, IDataBlock* mem, bool owned)
{
    if ( m_mems.find(name) == m_mems.end() ) {
        m_mems.insert(std::make_pair(name, std::make_pair(mem, owned)));
        return true;
    }
    LogError << name << " is already registered here!" << std::endl;
    return false;
}

bool DataMemSvc::initialize()
{
    return true;
}

bool DataMemSvc::finalize()
{
    for (auto &it : m_mems)
    {
        //delete the DataBlock if this service owns it
        if (it.second.second) delete it.second.first;
    }
    m_mems.clear();

    return true;
}
