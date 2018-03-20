/* Copyright (C) 2018 Jiaheng Zou <zoujh@ihep.ac.cn>
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

#ifndef SNIPER_INCIDENT_H
#define SNIPER_INCIDENT_H

#include <string>

class Task;

class Incident
{
    public :

        static bool fire(Task& domain, const std::string& key);

        Incident(const std::string& key)
            : m_key(key) {}

        virtual ~Incident() {}

        virtual bool fire(Task& domain);

        const std::string& name() { return m_key; }


    protected :

        std::string  m_key;

        //not provided
        Incident() = delete;
};

#endif
