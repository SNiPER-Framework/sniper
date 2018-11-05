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

#ifndef SNIPER_DECLARE_DLE_H
#define SNIPER_DECLARE_DLE_H

#include <string>

class DLElement;

struct SniperBookDLE
{
    SniperBookDLE() = delete;
    SniperBookDLE(const SniperBookDLE&) = delete;
    SniperBookDLE& operator=(const SniperBookDLE&) = delete;

    typedef DLElement* (*DLECreator)(const std::string&);
    SniperBookDLE(const std::string& type, DLECreator creator);
};

#define SNIPER_DECLARE_DLE(DLEClass) \
DLElement* sniper_DLE_##DLEClass##_creator_(const std::string& name) { \
   return new DLEClass(name); \
} \
SniperBookDLE sniper_book_DLE_##DLEClass##_(#DLEClass, &sniper_DLE_##DLEClass##_creator_)

#endif
