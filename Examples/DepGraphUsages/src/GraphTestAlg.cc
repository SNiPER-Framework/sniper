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

#include "DepGraphUsages/GraphTestAlg.h"
#include "SniperKernel/AlgFactory.h"

DECLARE_ALGORITHM(GraphTestAlg);

GraphTestAlg::GraphTestAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("INFO", m_info);
}

GraphTestAlg::~GraphTestAlg()
{
}

bool GraphTestAlg::initialize()
{
    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool GraphTestAlg::execute()
{
    LogDebug << m_info << std::endl;
    return true;
}

bool GraphTestAlg::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}
