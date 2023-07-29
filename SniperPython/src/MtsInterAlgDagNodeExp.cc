/* Copyright (C) 2018-2023
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

#include "SniperPrivate/MtsInterAlgDag.h"
#include "SniperPrivate/MtsInterAlgNode.h"
#include <boost/python/class.hpp>

void export_Sniper_MtsInterAlgDagNode()
{
    using namespace boost::python;

    void (MtsInterAlgNode::*f1dependOn)(const std::string &) = &MtsInterAlgNode::dependOn;
    void (MtsInterAlgNode::*f2dependOn)(const std::vector<std::string> &) = &MtsInterAlgNode::dependOn;

    class_<MtsInterAlgNode, boost::noncopyable>("MtsInterAlgNode", no_init)
        .def("dependOn", f1dependOn)
        .def("dependOn", f2dependOn);

    class_<MtsInterAlgDag, boost::noncopyable>("MtsInterAlgDag", no_init)
        .def("node", &MtsInterAlgDag::node, return_value_policy<reference_existing_object>())
        .def("json", &MtsInterAlgDag::json);
}