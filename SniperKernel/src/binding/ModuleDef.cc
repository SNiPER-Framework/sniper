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

#include "NonUserIf/PythonCapsul.h"
#include <boost/python.hpp>

void export_Sniper_Incident();
void export_Sniper_IIncidentHandler();
void export_Sniper_Property();
void export_Sniper_DLElement();
void export_Sniper_SvcBase();
void export_Sniper_AlgBase();
void export_Sniper_ToolBase();
void export_Sniper_Task();
void export_Sniper_TopTask();
void export_Sniper_TaskWatchDog();
void export_Sniper_RunState();
void export_Sniper_Sniper();

BOOST_PYTHON_MODULE(libSniperPython)
{
    export_Sniper_Incident();
    export_Sniper_IIncidentHandler();
    export_Sniper_Property();
    export_Sniper_DLElement();
    export_Sniper_SvcBase();
    export_Sniper_AlgBase();
    export_Sniper_ToolBase();
    export_Sniper_Task();
    export_Sniper_TopTask();
    export_Sniper_TaskWatchDog();
    export_Sniper_RunState();
    export_Sniper_Sniper();

    Sniper::python_capsul();
}
