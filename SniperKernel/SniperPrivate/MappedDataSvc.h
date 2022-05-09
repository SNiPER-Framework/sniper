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

#ifndef SNIPER_MAPPED_DATA_SVC_H
#define SNIPER_MAPPED_DATA_SVC_H

#include "SniperKernel/SvcBase.h"
#include "SniperKernel/ExecUnit.h"

class MappedDataSvc : public SvcBase
{
public:
    MappedDataSvc(const std::string &name);
    virtual ~MappedDataSvc();

    bool regist(ExecUnit &domain, const std::string &src, const std::string &dest);

    virtual bool initialize() override;
    virtual bool finalize() override;

    virtual SniperJSON json() override;
    virtual void eval(const SniperJSON &json) override;

private:
    std::map<std::string, std::string> m_dataMap;
};

#endif