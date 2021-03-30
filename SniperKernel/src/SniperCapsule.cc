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

#include "SniperKernel/Sniper.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperContext.h"
#include "SniperKernel/SniperLog.h"
#include "SniperPrivate/IncidentMgr.h"
#include "SniperPrivate/DLEFactory.h"
#include <fstream>
#include <signal.h>

//global variables
Sniper::Context *sniper_context;

//the function decalarations for library initialize and finalize
void sniper_kernel_initialize() __attribute__((constructor));
void sniper_kernel_finalize() __attribute__((destructor));

//the function decalaration for signal handling
void sniper_sig_handler(int signum);

//definition of sniper_kernel_initialize
void sniper_kernel_initialize()
{
    sniper_context = new Sniper::Context();
    //TODO: set SniperLog::LogStream, and do not use std::cout directly anymore
    std::cout << "**************************************************\n"
              << "***             Welcome to SNiPER              ***\n"
              << "**************************************************\n"
              << "Running @ " << sniper_context->hostName()
              << " on " << Sniper::System::sysDate();

    sigset(SIGINT, sniper_sig_handler);
}

//definition of sniper_kernel_finalize
void sniper_kernel_finalize()
{
    IncidentMgr::release();
    DLEFactory::release();
    std::cout << std::endl
              << "**************************************************\n"
              << "Terminating @ " << sniper_context->hostName()
              << " on " << Sniper::System::sysDate()
              << sniper_context->sys_info() << std::endl
              << sniper_context->summary() << std::endl;

    if (dynamic_cast<std::ofstream *>(SniperLog::LogStream))
    {
        delete SniperLog::LogStream;
        SniperLog::LogStream = &std::cout;
    }
    delete sniper_context;
}

//definition of sniper_sig_handler
void sniper_sig_handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        sniper_context->reg_msg("SNiPER is stopped by Ctrl+C");
        std::cout << "SNiPER is stopped by Ctrl+C" << std::endl;
        exit(0);
    default:; //pass
    }
}

extern "C"
{
    bool run_from_json(const char *filename)
    {
        auto pobj = Sniper::eval(filename);

        //try Task
        auto pTask = dynamic_cast<Task *>(pobj);
        if (pTask != nullptr)
        {
            return pTask->run();
        }

        return false;
    }
}
