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

#include "SniperKernel/SniperLog.h"

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

const std::string& SniperLog::scope()
{
    static const std::string NonScope = "SNiPER:";
    return NonScope;
}

const std::string& SniperLog::objName()
{
    static const std::string NonDLE = "NonDLE";
    return NonDLE;
}

using SniperLog::LogHelper;

static const char* _LogHelper_colors[] = {
    "",
    0,
    "\033[36m",         // DEBUG: Cyan
    "\033[32m",         // INFO:  Green
    "\033[33m",         // WARN:  Yellow
    "\033[31m",         // ERROR: Red
    "\033[1m\033[31m"   // FATAL: Bold Red
};

static const char* _LogHelper_flags[] = {
    "  TEST: ",
    0,
    " DEBUG: ",
    "  INFO: ",
    "  WARN: ",
    " ERROR: ",
    " FATAL: "
};

static std::atomic_flag _LogHelper_lock_log = ATOMIC_FLAG_INIT;
static std::atomic_flag _LogHelper_lock_ctime = ATOMIC_FLAG_INIT;

static bool _LogHelper_color_on = false;

LogHelper::LogHelper(int flag,
                     int level,
                     const std::string& scope,
                     const std::string& objName,
                     const char* func
                     )
    : m_active(flag >= level)
{
    if ( m_active ) {
        int sKeep = 30;
        std::string prefix;

        if ( ShowTime ) {
            sKeep = 56;
            prefix.reserve(63);
            time_t t = time(0);
            while(_LogHelper_lock_ctime.test_and_set());
            prefix = ctime(&t);  // ctime() is not thread safe
            _LogHelper_lock_ctime.clear();
            prefix.replace(24, 1, "  ");
        }
        else {
            prefix.reserve(35);
        }

        prefix += scope;
        prefix += objName;
        prefix += '.';
        prefix += func;

        while(_LogHelper_lock_log.test_and_set());
        if ( (_LogHelper_color_on = (flag >= Colorful)) ) {
            (*LogStream) << _LogHelper_colors[flag];
        }
        (*LogStream) << std::setiosflags(std::ios::left) << std::setw(sKeep)
            << prefix << _LogHelper_flags[flag];
    }
}

LogHelper::~LogHelper()
{
    if ( m_active ) {
        if ( _LogHelper_color_on ) {
            (*LogStream) << "\033[0m";
        }
        _LogHelper_lock_log.clear();
    }
}

LogHelper& LogHelper::operator<<(std::ostream& (*_f)(std::ostream&))
{
    if ( m_active ) _f(*LogStream);
    return *this;
}
