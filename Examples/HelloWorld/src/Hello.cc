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
#include "SniperPython/PyDataStore.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
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

bool HelloAlg::initialize()
{
    m_ds = SniperDataPtr<PyDataStore>(m_par, "DataStore").data();

    LogInfo << " initialized successfully" << std::endl;
    print();

    return true;
}

bool HelloAlg::execute()
{
    ++m_count;
    LogInfo << "Hello world: count: " << m_count << std::endl;

    if ( m_count == 1 ) {
        LogDebug << "debug message" << std::endl;
        LogInfo  << "info message" << std::endl;
        LogWarn  << "warn message" << std::endl;
        LogError << "error message" << std::endl;
        LogFatal << "fatal message" << std::endl;

        return true;
    }

    if ( m_name == "SetAlg" ) {
        m_ds->clear();

        LogInfo << " set values into PyDataStore" << std::endl;
        print();

        // a simple value
        m_ds->set("aStr", m_string);
        // a vector
        m_ds->set("aVec", m_vector_int);
        // a map
        m_ds->set("aMap", m_str_int);
    }

    if ( m_name == "GetAlg" ) {
        // a new float value
        float vf = 0;
        m_ds->get("newValue", vf);
        // a simple value
        m_ds->get("aStr", m_string);
        // a vector
        m_ds->get("aVec", m_vector_int);
        // a map: I don't know how to get a map
        //m_ds->get("aMap", m_str_int);

        LogInfo << " get values from PyDataStore" << std::endl;
        std::cout << "A new value = " << vf << std::endl;
        print();
    }

    return true;
}

bool HelloAlg::finalize()
{
    LogInfo << " finalized successfully" << std::endl;

    return true;
}

void  HelloAlg::print()
{
    property("VarString")->show();
    property("VectorInt")->show();
    property("MapStrInt")->show();

    std::cout << std::endl;
}
