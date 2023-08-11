/* Copyright (C) 2018-2023
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

#ifndef SNIPER_SNIPER_DATA_PTR_H
#define SNIPER_SNIPER_DATA_PTR_H

#include "SniperKernel/RefBase.h"
#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/DataMemSvc.h"
#include <string>

// Get DataBlock reference with its path
template <typename Data>
class SniperDataPtr : public RefBase<Data>
{
public:
    SniperDataPtr(ExecUnit *domain, const std::string &path);
    SniperDataPtr(ExecUnit &domain, const std::string &path);

    virtual ~SniperDataPtr() = default;

private:
    // following methods are not supported
    SniperDataPtr() = delete;
    SniperDataPtr(const SniperDataPtr &) = delete;
    SniperDataPtr &operator=(const SniperDataPtr &) = delete;

    void init(ExecUnit &domain, const std::string &path);
};

// SniperDataPtr(const std::string& path) is removed (for MT mode)

template <typename Data>
SniperDataPtr<Data>::SniperDataPtr(ExecUnit *domain, const std::string &path)
{
    init(*domain, path);
}

template <typename Data>
SniperDataPtr<Data>::SniperDataPtr(ExecUnit &domain, const std::string &path)
{
    init(domain, path);
}

template <typename Data>
void SniperDataPtr<Data>::init(ExecUnit &domain, const std::string &path)
{
    ExecUnit *_domain = &domain;
    std::string name = path;

    std::string::size_type pseg = path.rfind(":");
    if (pseg != std::string::npos)
    {
        _domain = dynamic_cast<ExecUnit *>(domain.find(path.substr(0, pseg)));
        name = path.substr(pseg + 1, std::string::npos);
    }

    DataMemSvc *svc = _domain->dataSvc();
    this->m_obj = (svc != 0) ? dynamic_cast<Data *>(svc->find(name)) : 0;
}

#endif
