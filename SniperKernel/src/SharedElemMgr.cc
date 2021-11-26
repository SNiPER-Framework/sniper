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

#include "SniperPrivate/SharedElemMgr.h"
#include "SniperKernel/DLElement.h"
#include "SniperKernel/SharedElemFactory.h"
#include <vector>
#include <mutex>

namespace SharedElemMgr
{
    static auto s_shared_elements = new std::vector<DLElement *>();
    static std::mutex s_mutex;
}

void SharedElemMgr::take_ownership(DLElement *obj)
{
    const std::lock_guard<std::mutex> lock(s_mutex);
    //the id can be used as the object index in s_shared_elements
    dynamic_cast<SharedElemBase*>(obj)->index = s_shared_elements->size();
    s_shared_elements->push_back(obj);
}

DLElement *SharedElemMgr::get(int id)
{
    const std::lock_guard<std::mutex> lock(s_mutex);
    return s_shared_elements->at(id);
}

void SharedElemMgr::release()
{
    for (auto obj : *s_shared_elements)
    {
        delete obj;
    }
    delete s_shared_elements;
}
