/* Copyright (C) 2018-2022
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

#include "SniperPrivate/MappedDataSvc.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/IDataBlock.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(MappedDataSvc);

MappedDataSvc::MappedDataSvc(const std::string &name)
    : SvcBase(name)
{
}

MappedDataSvc::~MappedDataSvc()
{
}

bool MappedDataSvc::regist(ExecUnit &domain, const std::string &src, const std::string &dest)
{
    //LogDebug << "regist " << domain.scope() << domain.objName() << ":" << src << " ==> " << dest << std::endl;
    if (&domain == m_par && src == dest)
    {
        return true;
    }

    if (m_dataMap.find(dest) != m_dataMap.end())
    {
        LogError << dest << " already mapped in this Task" << std::endl;
        return false;
    }

    m_dataMap[dest] = domain.scope() + domain.objName() + ':' + src;

    return true;
}

bool MappedDataSvc::initialize()
{
    auto dsvc = dynamic_cast<DataMemSvc *>(m_par->findSvc("DataMemSvc"));

    for (auto &it : m_dataMap)
    {
        const std::string &fullPath = it.second;
        std::string path = fullPath.substr(fullPath.find(':')+1, std::string::npos);
        IDataBlock* data = SniperDataPtr<IDataBlock>(getRoot(), path).data();

        dsvc->regist(it.first, data, false);
    }

    return true;
}

bool MappedDataSvc::finalize()
{
    return true;
}

SniperJSON MappedDataSvc::json()
{
    SniperJSON json = SvcBase::json();
    json["properties"].from(m_dataMap);

    return json;
}

void MappedDataSvc::eval(const SniperJSON &json)
{
    SniperJSON _json = json;
    auto& properties = _json["properties"];

    for (auto it = properties.map_begin(); it != properties.map_end(); ++it)
    {
        auto& key = it->first;
        m_dataMap[key] = properties[key].get<std::string>();
    }

    _json.erase("properties");
    SvcBase::eval(_json);
}
