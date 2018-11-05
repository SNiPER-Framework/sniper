/* Copyright (C) 2018
   Jiaheng Zou <zoujh@ihep.ac.cn> Tao Lin <lintao@ihep.ac.cn>
   Weidong Li <liwd@ihep.ac.cn> Xingtao Huang <huangxt@sdu.edu.cn>
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

struct PropertyWrap : Property, bp::wrapper<Property>
{
    bool set(bp::object& var) {
        return this->get_override("set")(var);
    }

    bool append(bp::object& var) {
        if ( bp::override f = this->get_override("append") )
            return f(var);
        return Property::append(var);
    }

    bool default_append(bp::object& var) {
        return this->Property::append(var);
    }
};

void export_Sniper_Property()
{
    using namespace boost::python;

    void (Property::*showf)() = &Property::show;

    class_<PropertyWrap, boost::noncopyable>("Property", no_init)
        .def("set",    pure_virtual(&Property::set))
        .def("append", &Property::append, &PropertyWrap::default_append)
        .def("show",   showf)
        ;
}
