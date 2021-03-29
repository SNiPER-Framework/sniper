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

#include "HelloWorld.h"
#include "DummyTool.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"

DECLARE_ALGORITHM(HelloWorld);

HelloWorld::HelloWorld(const std::string &name)
    : AlgBase(name),
      m_count(0)
{
    // declare properties
    declProp("VarBool", m_bool);
    declProp("VecFloat", m_vector_float);
    declProp("MapIntStr", m_int_str);
}

HelloWorld::~HelloWorld()
{
}

bool HelloWorld::initialize()
{
    // test logs
    LogDebug << "debug message" << std::endl;
    LogInfo << "info message" << std::endl;
    LogWarn << "warn message" << std::endl;
    LogError << "error message" << std::endl;
    LogFatal << "fatal message" << std::endl;

    // test properties
    LogDebug << "properties are initialized as:" << std::endl;
    LogDebug << "VarBool: " << SniperJSON().from(m_bool).str() << std::endl;
    LogDebug << "VecFloat: " << SniperJSON().from(m_vector_float).str(-1) << std::endl;
    LogDebug << "MapIntStr: " << SniperJSON().from(m_int_str).str(-1) << std::endl;

    // get the tool
    m_tool = dynamic_cast<DummyTool*>(m_tools["HelloTool"]);

    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool HelloWorld::execute()
{
    LogDebug << "Begin Event " << m_count++ << std::endl;

    // test tools
    m_tool->doSomeThing(m_bool);
    LogDebug << "VarBool: " << SniperJSON().from(m_bool).str() << std::endl;
    m_tool->doSomeThing(m_vector_float);
    LogDebug << "VecFloat: " << SniperJSON().from(m_vector_float).str(-1) << std::endl;
    m_tool->doSomeThing(m_int_str);
    LogDebug << "MapIntStr: " << SniperJSON().from(m_int_str).str() << std::endl;

    return true;
}

bool HelloWorld::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}
