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
    int LogLevel = 3;
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

std::atomic_flag LogHelper::lock = ATOMIC_FLAG_INIT;

static const char* _LogHelper_flags[] = {
    "  TEST: ",
    0,
    " DEBUG: ",
    "  INFO: ",
    "  WARN: ",
    " ERROR: ",
    " FATAL: "
};

LogHelper::LogHelper(int flag,
                     int level,
                     const std::string& scope,
                     const std::string& objName,
                     const char* func
                     )
    : m_active(flag >= level)
{
    if ( m_active ) {
        std::string prefix = scope;
        prefix += objName;
        prefix += '.';
        prefix += func;
        while(lock.test_and_set());
        std::cout << std::setiosflags(std::ios::left) << std::setw(30)
                  << prefix << _LogHelper_flags[flag];
    }
}

LogHelper::~LogHelper()
{
    if ( m_active ) lock.clear();
}

LogHelper& LogHelper::operator<<(std::ostream& (*_f)(std::ostream&))
{
    if ( m_active ) _f(std::cout);
    return *this;
}
