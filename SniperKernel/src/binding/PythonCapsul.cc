/* Copyright (C) 2018 Jiaheng Zou <zoujh@ihep.ac.cn>
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

#include "SniperKernel/Task.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperLog.h"
#include "NonUserIf/PythonCapsul.h"
#include "NonUserIf/IncidentMgr.h"
#include "NonUserIf/WhiteBoard.h"
#include "NonUserIf/DLEFactory.h"
#include <iostream>
#include <fstream>

void Sniper::python_capsul()
{
    static struct PythonCapsul
    {
        PythonCapsul() {
            std::cout << "**************************************************\n"
                      << "***          Welcome to SNiPER Python          ***\n"
                      << "**************************************************\n"
                      << "Running @ " << System::hostName()
                      << " on " << System::sysDate();
        }

        ~PythonCapsul() {
            WhiteBoard::clear();
            IncidentMgr::release();
            DLEFactory::release();
            std::cout << std::endl
                      << "**************************************************\n"
                      << "Terminating @ " << System::hostName()
                      << " on " << System::sysDate();
            sniper_context.sys_info();
            sniper_context.summary();
            std::cout << std::endl;
            if ( dynamic_cast<std::ofstream*>(SniperLog::LogStream) ) {
                delete SniperLog::LogStream;
            }
        }
    } aCapsul;
}
