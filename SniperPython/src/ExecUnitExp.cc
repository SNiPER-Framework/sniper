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
#include "SniperKernel/ExecUnit.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/AlgBase.h"

namespace bp = boost::python;

struct ExecUnitWarp : ExecUnit, bp::wrapper<ExecUnit>
{
    ExecUnitWarp(const std::string& name)
        : ExecUnit(name)
    {
    }

    DLElement *find(const std::string &name) {
         if (bp::override f = this->get_override("find")) return f(name);
         return ExecUnit::find(name);
    }
    DLElement *default_find(const std::string &name) {
         return this->ExecUnit::find(name);
    }

    DLElement *create(const std::string &type, const std::string &name) {
         if (bp::override f = this->get_override("create")) return f(type, name);
         return ExecUnit::create(type, name);
    }
    DLElement *default_create(const std::string &type, const std::string &name) {
         return this->ExecUnit::create(type, name);
    }

    void remove(const std::string &name) {
         if (bp::override f = this->get_override("remove")) {
              f(name);
              return;
         }
         return ExecUnit::remove(name);
    }
    void default_remove(const std::string &name) {
         return this->ExecUnit::remove(name);
    }

    void reset() {
         if (bp::override f = this->get_override("reset")) {
              f();
              return;
         }
         return ExecUnit::reset();
    }
    void default_reset() {
         return this->ExecUnit::reset();
    }
};

void export_Sniper_ExecUnit()
{
    using namespace boost::python;

    class_<ExecUnit, bases<DLElement>, boost::noncopyable>("ExecUnit", no_init)
        .def("isRoot", &ExecUnit::isRoot)
        .def("findSvc", &ExecUnit::findSvc, return_value_policy<reference_existing_object>())
        .def("findAlg", &ExecUnit::findAlg, return_value_policy<reference_existing_object>())
        .def("createSvc", &ExecUnit::createSvc, return_value_policy<reference_existing_object>())
        .def("createAlg", &ExecUnit::createAlg, return_value_policy<reference_existing_object>())
        .def("addSvc", &ExecUnit::addSvc, return_value_policy<reference_existing_object>())
        .def("addAlg", &ExecUnit::addAlg, return_value_policy<reference_existing_object>())
        .def("clearSvcs", &ExecUnit::clearSvcs)
        .def("clearAlgs", &ExecUnit::clearAlgs)
        .def("find", &ExecUnit::find, &ExecUnitWarp::default_find,
             return_value_policy<reference_existing_object>())
        .def("create", &ExecUnit::create, &ExecUnitWarp::default_create,
             return_value_policy<reference_existing_object>())
        .def("remove", &ExecUnit::remove, &ExecUnitWarp::default_remove)
        .def("reset", &ExecUnit::reset, &ExecUnitWarp::default_reset);
}
