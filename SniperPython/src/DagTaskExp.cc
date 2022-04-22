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

#include <boost/python.hpp>
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/DagTask.h"

namespace bp = boost::python;

void export_Sniper_DagTask()
{
    bp::class_<DagTask, bp::bases<DagBase>, boost::noncopyable>
        ("DagTask", bp::init<const std::string&>())
        .def("insertNode", &DagTask::insertNode,
                bp::return_value_policy<bp::reference_existing_object>())
        .def("makeEdge", &DagTask::makeEdge)
        .def("done", &DagTask::done)
    ;
}