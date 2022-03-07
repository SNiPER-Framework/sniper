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

#ifndef SNIPER_SHARED_ELEM_MGR_H
#define SNIPER_SHARED_ELEM_MGR_H

#include <string>

class DLElement;

namespace SharedElemMgr
{
    void take_ownership(DLElement *obj);
    int number_of_elements();
    DLElement *get(int index);
    DLElement *get(const std::string &identifier);
    void release();
}

#endif
