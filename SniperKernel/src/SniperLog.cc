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

LogHelper::LogHelper(int flag,
                     int level,
                     const std::string& scope,
                     const std::string& objName,
                     const char* func
                     )
    : m_active(flag >= level)
{
    if ( m_active ) {
        std::string prefix;
        prefix.reserve(71);
        int plen = 0;
        if ( (m_colored = (flag >= Colorful)) ) {
            prefix = _LogHelper_colors[flag];
            plen = prefix.length();
        }
        if ( ShowTime ) {
            time_t t = time(0);
            while(_LogHelper_lock_ctime.test_and_set());
            prefix += ctime(&t);  // ctime() is not thread safe
            _LogHelper_lock_ctime.clear();
            plen = prefix.length() - 1;
            prefix.replace(plen, 1, "  ");
        }

        prefix += scope;
        prefix += objName;
        prefix += '.';
        prefix += func;

        plen -= prefix.length() - 30;
        if ( plen > 0 ) {
            prefix.append(plen, ' ');
        }

        prefix += _LogHelper_flags[flag];

        while(_LogHelper_lock_log.test_and_set());
        (*LogStream) << prefix;
    }
}

LogHelper::~LogHelper()
{
    if ( m_active ) {
        if ( m_colored ) {
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
