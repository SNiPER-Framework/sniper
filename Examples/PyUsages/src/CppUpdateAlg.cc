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

#include "CppUpdateAlg.h"
#include "SniperPython/PyDataStore.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperLog.h"

DECLARE_ALGORITHM(CppUpdateAlg);

CppUpdateAlg::CppUpdateAlg(const std::string &name)
    : AlgBase(name)
{
    m_count = 0;
}

CppUpdateAlg::~CppUpdateAlg()
{
}

bool CppUpdateAlg::initialize()
{
    m_ds = SniperDataPtr<PyDataStore>(m_par, "DataStore").data();
    LogInfo << "initialized successfully" << std::endl;

    return true;
}

bool CppUpdateAlg::execute()
{
    ++m_count;
    LogDebug << "CppUpdateAlg count: " << m_count << std::endl;

    // Get from PyDataStore
    m_ds->get("newValue", m_float);
    m_ds->get("aStr", m_string);
    m_ds->get("aVec", m_vector_int);
    m_ds->get("aMap", m_str_int);

    // Update the values
    m_float *= 0.9;
    m_string += "UpdatedInCpp";
    for (auto &iv : m_vector_int)
    {
        iv *= 10;
    }
    for (auto &im : m_str_int)
    {
        im.second *= 10 * m_count;
    }

    // Set back to PyDataStore
    m_ds->set("newValue", m_float);
    m_ds->set("aStr", m_string);
    m_ds->set("aVec", m_vector_int);
    m_ds->set("aMap", m_str_int);

    // Print the update data
    print();

    return true;
}

bool CppUpdateAlg::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}

void CppUpdateAlg::print()
{
    SniperJSON json;
    json["newValue"] = SniperJSON().from(m_float);
    json["aStr"] = SniperJSON().from(m_string);
    json["aVec"] = SniperJSON().from(m_vector_int);
    json["aMap"] = SniperJSON().from(m_str_int);
    LogInfo << "update to: " << json.str(-1) << std::endl;
}
