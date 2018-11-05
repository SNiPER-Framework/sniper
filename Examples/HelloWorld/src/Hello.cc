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

#include "Hello.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperLog.h"

DECLARE_ALGORITHM(HelloAlg);

HelloAlg::HelloAlg(const std::string& name)
    : AlgBase(name)
{
    m_count = 0;
    // Using declare property
    declProp("VarString", m_string);
    declProp("VectorInt", m_vector_int);
    declProp("MapStrInt", m_str_int);
}

HelloAlg::~HelloAlg()
{
}

bool
HelloAlg::initialize()
{
    LogInfo << " initialized successfully" << std::endl;
    LogInfo << " MyString: " << m_string << std::endl;

    LogInfo << " MyVectorInt(" << m_vector_int.size() << "): " << std::endl;
    for (std::vector<int>::iterator i=m_vector_int.begin();
            i!=m_vector_int.end(); ++i) {
        LogInfo << (*i) << std::endl;
    }
    LogInfo << std::endl;

    LogInfo << " MyStrInt(" << m_str_int.size() << "): " << std::endl; 
    for (std::map<std::string, int>::iterator i=m_str_int.begin();
            i!=m_str_int.end(); ++i) {
        LogInfo << i->first << ":"
                << i->second
                << std::endl;
    }
    return true;
}

bool
HelloAlg::execute()
{
    ++m_count;
    LogInfo << "Hello world: count: " << m_count << std::endl;

    LogDebug << "debug message" << std::endl;
    LogInfo  << "info message" << std::endl;
    LogWarn  << "warn message" << std::endl;
    LogError << "error message" << std::endl;
    LogFatal << "fatal message" << std::endl;

    return true;
}

bool
HelloAlg::finalize()
{
    LogInfo << " finalized successfully" << std::endl;

    return true;
}
