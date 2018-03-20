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

#include "SniperKernel/Sniper.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperException.h"
#include <dlfcn.h>
#include <unistd.h>
#include <time.h>

namespace SniperLog {
    extern int LogLevel;
}

void Sniper::setLogLevel(int level)
{
    LogTest << "Set TopTask/default LogLevel" << std::endl;
    SniperLog::LogLevel = level;
}

void Sniper::loadDll(char* dll)
{
    void *dl_handler = dlopen(dll, RTLD_LAZY|RTLD_GLOBAL);
    if ( !dl_handler ) {
        LogFatal << dlerror() << std::endl;
        throw ContextMsgException(std::string("Can't load DLL ") + dll);
    }
}

const std::string& Sniper::System::hostName()
{
    static std::string hName;
    if ( hName.empty() ) {
        char name[128];
        int status = gethostname(name, 128);
        hName = status ? "?" : name;
    }
    return hName;
}

std::string Sniper::System::sysDate()
{
    time_t t = time(0);
    return ctime(&t);
}
