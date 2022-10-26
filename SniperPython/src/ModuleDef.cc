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

#include <boost/python/object.hpp>
#include <boost/python/import.hpp>
#include <boost/python/module.hpp>

boost::python::object &BoostPyJsonModule()
{
    static auto jmod = new boost::python::object(boost::python::import("json"));
    return *jmod;
}

void export_Sniper_Incident();
void export_Sniper_IIncidentHandler();
void export_Sniper_Property();
void export_Sniper_IDataBlock();
void export_Sniper_DLElement();
void export_Sniper_SvcBase();
void export_Sniper_AlgBase();
void export_Sniper_ToolBase();
void export_Sniper_DataMemSvc();
void export_Sniper_MappedDataSvc();
void export_Sniper_ExecUnit();
void export_Sniper_Task();
void export_Sniper_Workflow();
void export_Sniper_TopTask();
void export_Sniper_TaskWatchDog();
void export_Sniper_RunState();
void export_Sniper_Sniper();
void export_Sniper_SniperLog();
void export_SniperPython_PyDataStore();
void export_Sniper_Timer();
void export_Sniper_TimerSvc();
void export_Sniper_DagTask();
void export_Sniper_DagBase();
void export_Sniper_SniperJSON();

BOOST_PYTHON_MODULE(libSniperPython)
{
    export_Sniper_Incident();
    export_Sniper_IIncidentHandler();
    export_Sniper_Property();
    export_Sniper_IDataBlock();
    export_Sniper_DLElement();
    export_Sniper_SvcBase();
    export_Sniper_AlgBase();
    export_Sniper_ToolBase();
    export_Sniper_DataMemSvc();
    export_Sniper_MappedDataSvc();
    export_Sniper_ExecUnit();
    export_Sniper_Task();
    export_Sniper_Workflow();
    export_Sniper_TopTask();
    export_Sniper_TaskWatchDog();
    export_Sniper_RunState();
    export_Sniper_Sniper();
    export_Sniper_SniperLog();
    export_SniperPython_PyDataStore();
    export_Sniper_Timer();
    export_Sniper_TimerSvc();
    export_Sniper_DagBase();
    export_Sniper_DagTask();
    export_Sniper_SniperJSON();
}
