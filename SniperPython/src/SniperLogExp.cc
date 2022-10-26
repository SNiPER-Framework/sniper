/* Copyright (C) 2021
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

#include "SniperKernel/SniperLog.h"
#include <boost/python/class.hpp>
#include <boost/python/str.hpp>

namespace SniperLogExp
{
    void msg_print(SniperLog::Logger &log, boost::python::object &msg)
    {
        log << boost::python::extract<std::string>(boost::python::str(msg))();
    }
}

void export_Sniper_SniperLog()
{
    using namespace boost::python;

    class_<SniperLog::Logger>("SniperLog", init<int, const std::string &, const std::string &, const char *>())
        .def("msg", SniperLogExp::msg_print);
}
