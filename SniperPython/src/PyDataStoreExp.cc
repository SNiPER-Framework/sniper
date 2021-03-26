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

#include "SniperPython/PyDataStore.h"
#include <boost/python.hpp>
#include "boost/make_shared.hpp"

void export_SniperPython_PyDataStore()
{
    using namespace boost::python;

    class_<PyDataStore, boost::shared_ptr<PyDataStore>, bases<IDataBlock>, boost::noncopyable>
        ("PyDataStore", no_init)
        .def("data",      &PyDataStore::data,
                return_value_policy<copy_non_const_reference>())
        .def("clear",     &PyDataStore::clear)
        ;
}
