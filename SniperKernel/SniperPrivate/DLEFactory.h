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

#ifndef SNIPER_DLE_FACTORY_H
#define SNIPER_DLE_FACTORY_H

#include <string>
#include <vector>
#include <map>

class DLElement;

class DLEFactory
{
public:
    typedef DLElement *(*DLECreator)(const std::string &);

    // get the singleton instance
    static DLEFactory &instance();

    // release the singleton instance
    static void release();

    // create a DLE object with its name
    DLElement *create(const std::string &name);

    // book the creator of new DLE types
    bool book(const std::string &type, DLECreator creator);

    // name
    const std::string &objName() { return m_name; }

    // valid types
    const std::vector<std::string> &validTypes() { return m_types; }

private:
    typedef std::map<std::string, DLECreator> Type2CreatorMap;

    // standard constructor
    DLEFactory();
    ~DLEFactory();

    // members
    std::string m_name;
    std::vector<std::string> m_types;
    Type2CreatorMap m_creators;

    static DLEFactory *s_obj;
};

#endif
