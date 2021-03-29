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

#ifndef Hello_World_h
#define Hello_World_h

#include "SniperKernel/AlgBase.h"
#include <vector>
#include <map>

class DummyTool;

class HelloWorld: public AlgBase
{
public:
    HelloWorld(const std::string &name);
    ~HelloWorld();

    bool initialize();
    bool execute();
    bool finalize();

private:
    int m_count;
    DummyTool* m_tool;

    bool m_bool;
    std::vector<float> m_vector_float;
    std::map<int, std::string> m_int_str;
};

#endif
