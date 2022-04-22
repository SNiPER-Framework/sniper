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

#include "SubTasksAlg.h"
#include "SniperKernel/AlgFactory.h"

DECLARE_ALGORITHM(SubTasksAlg);

SubTasksAlg::SubTasksAlg(const std::string &name)
    : AlgBase(name),
      m_count(0),
      m_i1("SubT1"),
      m_i2("SubT2")
{
}

SubTasksAlg::~SubTasksAlg()
{
}

bool SubTasksAlg::initialize()
{
    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool SubTasksAlg::execute()
{
    LogDebug << "Begin Event " << m_count << std::endl;

    if (m_count % 2 == 0)
    {
        LogDebug << "fire sub-task SubT1" << std::endl;
        m_i1.fire(*m_par);
    }
    else
    {
        LogDebug << "fire sub-task SubT2" << std::endl;
        m_i2.fire(*m_par);
    }

    ++m_count;

    return true;
}

bool SubTasksAlg::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}
