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

#include "SniperKernel/Incident.h"
#include "SniperKernel/ExecUnit.h"
#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>

void export_Sniper_Incident()
{
    using namespace boost::python;

    int (Incident::*member_fire)(ExecUnit&) = &Incident::fire;

    class_<Incident>("Incident", init<const std::string&>())
        .def("fire", member_fire)
        .def("name", &Incident::name,
                return_value_policy<copy_const_reference>())
        ;
}
