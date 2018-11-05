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

#include "SniperKernel/DLElement.h"
#include "SniperKernel/Task.h"
#include <boost/python.hpp>

namespace bp = boost::python;

struct DLElementWrap : DLElement, bp::wrapper<DLElement>
{
    DLElementWrap(const std::string& name)
        : DLElement(name)
    {
    }

    bool initialize() {
        return this->get_override("initialize")();
    }

    bool finalize() {
        return this->get_override("finalize")();
    }

    void setLogLevel(int level) {
        if ( bp::override f = this->get_override("setLogLevel") ) {
            f(level);
            return;
        }
        return DLElement::setLogLevel(level);
    }

    void default_setLogLevel(int level) {
        return this->DLElement::setLogLevel(level);
    }
};

void export_Sniper_DLElement()
{
    using namespace boost::python;

    void (DLElement::*showf)() = &DLElement::show;

    class_<DLElementWrap, boost::noncopyable>("DLElement", no_init)
        .def("logLevel",     &DLElement::logLevel)
        .def("scope",        &DLElement::scope,
                return_value_policy<copy_const_reference>())
        .def("objName",      &DLElement::objName,
                return_value_policy<copy_const_reference>())
        .def("setLogLevel",  &DLElement::setLogLevel,
                &DLElementWrap::default_setLogLevel)
        .def("initialize",   pure_virtual(&DLElement::initialize))
        .def("finalize",     pure_virtual(&DLElement::finalize))
        .def("getParent",     &DLElement::getParent,
                return_value_policy<reference_existing_object>())
        //.def("setParent",     &DLElement::setParent)
        .def("property",     &DLElement::property,
                return_value_policy<reference_existing_object>())
        .def("show",         showf)
        ;
}
