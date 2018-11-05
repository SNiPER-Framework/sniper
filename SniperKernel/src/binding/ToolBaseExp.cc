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

#include "SniperKernel/ToolBase.h"
#include <boost/python.hpp>

namespace bp = boost::python;

struct ToolBaseWarp : ToolBase, bp::wrapper<ToolBase>
{
    ToolBaseWarp(const std::string& name)
        : ToolBase(name)
    {
    }

    bool initialize() {
        if ( bp::override f = this->get_override("initialize") ) return f();
        return ToolBase::initialize();
    }

    bool default_initialize() {
        return this->ToolBase::initialize();
    }

    bool finalize() {
        if ( bp::override f = this->get_override("finalize") ) return f();
        return ToolBase::finalize();
    }

    bool default_finalize() {
        return this->ToolBase::finalize();
    }
};

void export_Sniper_ToolBase()
{
    using namespace boost::python;

    class_<ToolBaseWarp, bases<DLElement>, boost::noncopyable>
        ("ToolBase", init<const std::string&>())
        .def("initialize", &ToolBase::initialize, &ToolBaseWarp::default_initialize)
        .def("finalize",   &ToolBase::finalize, &ToolBaseWarp::default_finalize)
        ;
}
