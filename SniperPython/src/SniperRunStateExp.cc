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

#include "SniperKernel/SniperRunState.h"
#include <boost/python.hpp>

void export_Sniper_RunState()
{
    using namespace boost::python;
    using Sniper::RunState;

    enum_<RunState>("RunState")
        .value("Invalid",   RunState::Invalid)
        .value("StartUp",   RunState::StartUp)
        .value("Ready",     RunState::Ready)
        .value("Running",   RunState::Running)
        .value("Stopped",   RunState::Stopped)
        .value("Finalized", RunState::Finalized)
        .value("EndUp",     RunState::EndUp)
        .value("Error",     RunState::Error)
        ;
}
