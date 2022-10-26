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

#include "SniperKernel/SvcBase.h"
#include <boost/python/class.hpp>
#include <boost/python/pure_virtual.hpp>

namespace bp = boost::python;

struct SvcBaseWarp : SvcBase, bp::wrapper<SvcBase>
{
    SvcBaseWarp(const std::string& name)
        : SvcBase(name)
    {
    }

    bool initialize() {
        return this->get_override("initialize")();
    }

    bool finalize() {
        return this->get_override("finalize")();
    }
};

void export_Sniper_SvcBase()
{
    using namespace boost::python;

    class_<SvcBaseWarp, bases<DLElement>, boost::noncopyable>
        ("SvcBase", init<const std::string&>())
        .def("initialize", pure_virtual(&SvcBase::initialize))
        .def("finalize",   pure_virtual(&SvcBase::finalize))
        ;
}
