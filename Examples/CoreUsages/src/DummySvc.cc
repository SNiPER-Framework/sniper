/* Copyright (C) 2021
   Institute of High Energy Physics and Shandong University
   This file is part of mt.sniper.
 
   mt.sniper is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   mt.sniper is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with mt.sniper.  If not, see <http://www.gnu.org/licenses/>. */

#include "CoreUsages/DummySvc.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(DummySvc);

DummySvc::DummySvc(const std::string &name)
    : SvcBase(name),
      m_count(0)
{
    LogInfo << "this is a service shared by algorithms" << std::endl;
}

DummySvc::~DummySvc()
{
}

bool DummySvc::initialize()
{
    LogInfo << "initialized successfully" << std::endl;
    return true;
}

bool DummySvc::finalize()
{
    LogInfo << "finalized successfully" << std::endl;
    return true;
}

void DummySvc::doSomeThing()
{
    ++m_count;
    LogInfo << m_count << " times are invoked" << std::endl;
}