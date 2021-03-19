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

#include "SniperKernel/Property.h"
#include <boost/python.hpp>

namespace bp = boost::python;

namespace PropertyExp
{
    static auto json = bp::import("json").attr("dumps");

    bool set(Property &prop, bp::object &var)
    {
        return prop.set(bp::extract<std::string>(json(var)));
    }

    bool append(Property &prop, bp::object &var)
    {
        return prop.append(bp::extract<std::string>(json(var)));
    }
}

void export_Sniper_Property()
{
    using namespace boost::python;

    void (Property::*showf)() = &Property::show;

    class_<Property, boost::noncopyable>("Property", no_init)
        .def("set", PropertyExp::set)
        .def("append", PropertyExp::append)
        .def("show", showf);
}
