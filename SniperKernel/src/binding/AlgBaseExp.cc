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

#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include <boost/python.hpp>

namespace bp = boost::python;

struct AlgBaseWarp : AlgBase, bp::wrapper<AlgBase>
{
    AlgBaseWarp(const std::string& name)
        : AlgBase(name)
    {
    }

    bool initialize() {
        return this->get_override("initialize")();
    }

    bool execute() {
        return this->get_override("execute")();
    }

    bool finalize() {
        return this->get_override("finalize")();
    }
};

void export_Sniper_AlgBase()
{
    using namespace boost::python;

    class_<AlgBaseWarp, bases<DLElement>, boost::noncopyable>
        ("AlgBase", init<const std::string&>())
        .def("initialize", pure_virtual(&AlgBase::initialize))
        .def("execute",    pure_virtual(&AlgBase::execute))
        .def("finalize",   pure_virtual(&AlgBase::finalize))
        .def("createTool", &AlgBase::createTool,
                return_value_policy<reference_existing_object>())
        .def("findTool",   &AlgBase::findTool,
                return_value_policy<reference_existing_object>())
        ;
}
