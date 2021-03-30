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
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/SniperException.h"
#include "SniperPrivate/DLEFactory.h"
#include <algorithm>
#include <fstream>
#include <dlfcn.h>
#include <time.h>

namespace SniperLog
{
    extern int LogLevel;
    extern int Colorful;
    extern bool ShowTime;
    std::string LogFile;
}

namespace Sniper
{
    std::vector<std::string> LoadDlls;
}

DLElement *Sniper::eval(const char *fname)
{
    std::ifstream ifs(fname);
    auto json = SniperJSON::load(ifs);

    Sniper::Config::eval(json["sniper"].str(-1));

    DLElement *pobj = DLEFactory::instance().create(json["identifier"].get<std::string>());
    pobj->eval(json);

    return pobj;
}

void Sniper::setLogLevel(int level)
{
    SniperLog::LogLevel = level;
}

void Sniper::setColorful(int level)
{
    SniperLog::Colorful = level;
}

void Sniper::setShowTime(bool flag)
{
    SniperLog::ShowTime = flag;
}

void Sniper::setLogFile(const char *fname, bool append)
{
    std::ios_base::openmode mode = std::ios_base::out;
    if (append)
    {
        mode |= std::ios::app;
    }
    else
    {
        mode |= std::ios::trunc;
    }

    LogWarn << "Now using log file: " << fname << std::endl;
    SniperLog::Logger::lock();
    SniperLog::LogStream = new std::ofstream(fname, mode);

    if (SniperLog::LogStream->good())
    {
        SniperLog::Logger::unlock();
        SniperLog::LogFile = fname;
    }
    else
    {
        delete SniperLog::LogStream;
        SniperLog::LogStream = &std::cout;
        SniperLog::Logger::unlock();
        LogWarn << "Failed to open log file, recover to cout" << std::endl;
        SniperLog::LogFile.clear();
    }
}

void Sniper::setLogStdout()
{
    SniperLog::Logger::lock();
    SniperLog::LogStream = &std::cout;
    SniperLog::Logger::unlock();
    SniperLog::LogFile.clear();
}

void Sniper::loadDll(const char *dll)
{
    void *dl_handler = dlopen(dll, RTLD_LAZY | RTLD_GLOBAL);
    if (dl_handler)
    {
        auto &dlls = Sniper::LoadDlls;
        if (std::find(dlls.begin(), dlls.end(), dll) == dlls.end())
        {
            dlls.push_back(dll);
        }
    }
    else
    {
        LogFatal << dlerror() << std::endl;
        throw ContextMsgException(std::string("Can't load DLL ") + dll);
    }
}

std::string Sniper::System::sysDate()
{
    time_t t = time(0);
    return ctime(&t);
}

std::string Sniper::Config::json_str()
{
    SniperJSON j;
    j["LogLevel"].from(SniperLog::LogLevel);
    j["Colorful"].from(SniperLog::Colorful);
    j["ShowTime"].from(SniperLog::ShowTime);
    if (!SniperLog::LogFile.empty())
    {
        j["LogFile"].from(SniperLog::LogFile);
    }
    if (!Sniper::LoadDlls.empty())
    {
        j["LoadDlls"].from(Sniper::LoadDlls);
    }

    return j.str(-9);
}

void Sniper::Config::eval(const std::string &json_str)
{
    SniperJSON j(json_str);
    if (j.find("\"LogLevel\"") != j.map_end())
    {
        Sniper::setLogLevel(j["LogLevel"].get<int>());
    }
    if (j.find("\"Colorful\"") != j.map_end())
    {
        Sniper::setColorful(j["Colorful"].get<int>());
    }
    if (j.find("\"ShowTime\"") != j.map_end())
    {
        Sniper::setShowTime(j["ShowTime"].get<bool>());
    }
    if (j.find("\"LogFile\"") != j.map_end())
    {
        auto fn = j["LogFile"].get<std::string>();
        if (!fn.empty())
        {
            Sniper::setLogFile(fn.c_str());
        }
    }
    if (j.find("\"LoadDlls\"") != j.map_end())
    {
        auto &dlls = j["LoadDlls"];
        for (auto it = dlls.vec_begin(); it != dlls.vec_end(); ++it)
        {
            Sniper::loadDll(it->get<std::string>().c_str());
        }
    }
}
