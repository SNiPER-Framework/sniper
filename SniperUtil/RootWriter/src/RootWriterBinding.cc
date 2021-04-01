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

#include "RootWriter/RootWriter.h"
#include "TPython.h"
#include <boost/python.hpp>

namespace bp = boost::python;

namespace PyRootWriter
{
  bool attach(RootWriter &rw, const std::string &fullDirs, bp::object obj)
  {
    TDirectory *pDir = rw.getDir(fullDirs);
#if ROOT_VERSION_CODE < ROOT_VERSION(6, 22, 0)
    // for the legacy TPython
    PyObject *dir = TPython::ObjectProxy_FromVoidPtr((void *)pDir, "TDirectory");
#elif ROOT_VERSION_CODE >= ROOT_VERSION(6, 22, 0)
    // for the new TPython
    PyObject *dir = TPython::CPPInstance_FromVoidPtr((void *)pDir, "TDirectory");
#endif
    boost::python::object o(boost::python::handle<>(boost::python::borrowed(dir)));
    obj.attr("SetDirectory")(o);

    return (pDir != 0);
  }
}

BOOST_PYTHON_MODULE(libRootWriter)
{
  bp::class_<RootWriter, boost::shared_ptr<RootWriter>, bp::bases<SvcBase>, boost::noncopyable>("RootWriter", bp::init<std::string>())
      .def("attach", PyRootWriter::attach);
}
