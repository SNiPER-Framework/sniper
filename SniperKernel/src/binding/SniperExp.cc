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

#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/Task.h"
#include <boost/python.hpp>

extern boost::python::object &BoostPyJsonModule();

namespace SniperExp
{
    static auto json = BoostPyJsonModule().attr("dumps");

    void setLogLevel(boost::python::object &var)
    {
        auto jstr = boost::python::extract<std::string>(json(var))();
        Sniper::setLogLevel(SniperJSON(jstr).get<int>());
    }

    void setColorful(boost::python::object &var)
    {
        auto jstr = boost::python::extract<std::string>(json(var))();
        Sniper::setColorful(SniperJSON(jstr).get<int>());
    }

    void setShowTime(boost::python::object &var)
    {
        auto jstr = boost::python::extract<std::string>(json(var))();
        Sniper::setShowTime(SniperJSON(jstr).get<bool>());
    }

    void setLogFile1(char *fname)
    {
        Sniper::setLogFile(fname);
    }

    void setLogFile2(char *fname, boost::python::object &var)
    {
        auto jstr = boost::python::extract<std::string>(json(var))();
        Sniper::setLogFile(fname, SniperJSON(jstr).get<bool>());
    }
}

void export_Sniper_Sniper()
{
    using namespace boost::python;

    def("setLogLevel", SniperExp::setLogLevel);
    def("setColorful", SniperExp::setColorful);
    def("setShowTime", SniperExp::setShowTime);
    def("setLogFile", SniperExp::setLogFile1);
    def("setLogFile", SniperExp::setLogFile2);
    def("setLogStdout", &Sniper::setLogStdout);
    def("loadDll", &Sniper::loadDll);
}
