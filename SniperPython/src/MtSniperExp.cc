/* Copyright (C) 2023
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

#include "SniperKernel/MtSniper.h"
#include <boost/python/class.hpp>

void export_Sniper_MtSniper()
{
    using namespace boost::python;

    class_<MtSniper, bases<DLElement>, boost::noncopyable>("MtSniper")
        .def("setNumThreads", &MtSniper::setNumThreads)
        .def("setEvtMax", &MtSniper::setEvtMax)
        .def("createInputTask", &MtSniper::createInputTask,
                return_value_policy<reference_existing_object>())
        .def("createOutputTask", &MtSniper::createOutputTask,
                return_value_policy<reference_existing_object>())
        .def("createMainTask", &MtSniper::createMainTask,
                return_value_policy<reference_existing_object>())
        .def("run", &MtSniper::run);
}

