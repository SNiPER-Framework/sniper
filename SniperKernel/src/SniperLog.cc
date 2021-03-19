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

#include "SniperKernel/SniperLog.h"
#include <atomic>

namespace SniperLog{
    int           LogLevel  = 3;
    int           Colorful  = 9;
    bool          ShowTime  = false;
    std::ostream* LogStream = &std::cout;
}

int SniperLog::logLevel()
{
    return LogLevel;
}

const std::string &SniperLog::scope()
{
    static const std::string NonScope = "SNiPER:";
    return NonScope;
}

const std::string &SniperLog::objName()
{
    static const std::string NonDLE = "Unknown";
    return NonDLE;
}

using SniperLog::Logger;

Logger Logger::Silencer;

static const char *_Logger_colors[]{
    "",
    0,
    "\033[36m",       // DEBUG: Cyan
    "\033[32m",       // INFO:  Green
    "\033[33m",       // WARN:  Yellow
    "\033[31m",       // ERROR: Red
    "\033[1m\033[31m" // FATAL: Bold Red
};

static const char *_Logger_flags[]{
    "  TEST: ",
    0,
    " DEBUG: ",
    "  INFO: ",
    "  WARN: ",
    " ERROR: ",
    " FATAL: "
};

static std::atomic_flag _Logger_lock_log = ATOMIC_FLAG_INIT;

void Logger::lock()
{
    while(_Logger_lock_log.test_and_set());
}

void Logger::unlock()
{
    _Logger_lock_log.clear();
}

Logger::Logger(int flag,
               const std::string &scope,
               const std::string &objName,
               const char *func)
    : m_active(true)
{
    std::string prefix;
    prefix.reserve(80);
    int plen = 0;
    if ((m_colored = (flag >= Colorful)))
    {
        prefix = _Logger_colors[flag];
        plen = prefix.length();
    }
    if (ShowTime)
    {
        time_t t = time(0);
        char buf[32];
        ctime_r(&t, buf);
        prefix += buf;
        plen = prefix.length() - 1;
        prefix.replace(plen, 1, "  ");
    }

    prefix += scope;
    prefix += objName;
    prefix += '.';
    prefix += func;

    plen -= prefix.length() - 30;
    if (plen > 0)
    {
        prefix.append(plen, ' ');
    }

    prefix += _Logger_flags[flag];

    while(_Logger_lock_log.test_and_set());
    (*LogStream) << prefix;
}

Logger::~Logger()
{
    if (m_active)
    {
        if (m_colored)
        {
            (*LogStream) << "\033[0m";
        }
        _Logger_lock_log.clear();
    }
}
