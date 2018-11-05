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

#include "SniperKernel/Sniper.h"
#include "SniperKernel/Task.h"
#include <boost/python.hpp>

void export_Sniper_Sniper()
{
    using namespace boost::python;

    def("setLogLevel", &Sniper::setLogLevel);
    def("setColorful", &Sniper::setColorful);
    def("setShowTime", &Sniper::setShowTime);
    def("setLogFile",  &Sniper::setLogFile);
    def("loadDll",     &Sniper::loadDll);
}
