/* Copyright (C) 2021
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

#ifndef SNIPER_JSON_PARSER_H
#define SNIPER_JSON_PARSER_H

#include "SniperKernel/SniperJSON.h"

namespace Sniper
{
    class JSONParser
    {
    public:
        JSONParser(const SniperJSON &json) : m_json(json) {}

        bool check(const std::vector<std::string> &keys);
        const std::string &err() { return m_err; }

        template <typename Type>
        void assign_if_exist(const std::string &key, Type &var);

        template <typename Type>
        void exefunc_if_exist(const ::std::string &key, void (*func)(Type));

    private:
        std::string m_err;
        const SniperJSON &m_json;
    };

    template <typename Type>
    void JSONParser::assign_if_exist(const std::string &key, Type &var)
    {
        auto it = m_json.find(key);
        if (it != m_json.map_end())
        {
            var = it->second.get<Type>();
        }
    }

    template <typename Type>
    void JSONParser::exefunc_if_exist(const ::std::string &key, void (*func)(Type))
    {
        auto it = m_json.find(key);
        if (it != m_json.map_end())
        {
            func(it->second.get<Type>());
        }
    }
}

#endif
