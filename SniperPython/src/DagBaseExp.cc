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

#include "SniperKernel/AlgBase.h"
#include "SniperKernel/DagBase.h"
#include <boost/python/class.hpp>
#include <boost/python/pure_virtual.hpp>

namespace bp = boost::python;

struct DagBaseWrap : DagBase, bp::wrapper<DagBase>
{
    DagBaseWrap(const std::string& name)
        : DagBase(name)
    {
    }

    AlgBase* insertNode(const std::string& alg) {
        return this->get_override("insertNode")(alg);
    }

    bool makeEdge(const std::string& alg1, const std::string& alg2) {
        return this->get_override("makeEdge")(alg1, alg2);
    }

    bool done() {
        return this->get_override("done")();
    }
};

void export_Sniper_DagBase()
{
    bp::class_<DagBaseWrap, bp::bases<TopTask>, boost::noncopyable>
        ("DagBase", bp::init<const std::string&>())
        .def("insertNode", bp::pure_virtual(&DagBase::insertNode),
                bp::return_value_policy<bp::reference_existing_object>())
        .def("makeEdge", bp::pure_virtual(&DagBase::makeEdge))
        .def("done", bp::pure_virtual(&DagBase::done))
    ;
}
