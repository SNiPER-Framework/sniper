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

#ifndef SNIPER_SNIPER_LOG_H
#define SNIPER_SNIPER_LOG_H

#include <string>
#include <iostream>

namespace SniperLog
{
    //2:debug, 3:info, 4:warn, 5:error, 6:fatal
    int logLevel();

    //return the global scope and objName string
    const std::string &scope();
    const std::string &objName();

    //the stream for log output, std::cout by default
    extern std::ostream *LogStream;

    class Logger
    {
    public:
        static Logger Silencer;

        static void lock();
        static void unlock();

        Logger() : m_active(false) {}

        Logger(int flag,
               const std::string &scope,
               const std::string &objName,
               const char *func);

        ~Logger();

        Logger &operator<<(std::ostream &(*_f)(std::ostream &))
        {
            if (m_active)
                _f(*LogStream);
            return *this;
        }

        template <typename T>
        Logger &operator<<(const T &t)
        {
            if (m_active)
                (*LogStream) << t;
            return *this;
        }

    private:
        const bool m_active;
        bool m_colored;
    };

}

using SniperLog::logLevel;
using SniperLog::scope;
using SniperLog::objName;

#define SNIPERLOG(Flag)  SniperLog::Logger(\
        Flag,\
        scope(),\
        objName(),\
        __func__\
        )

#define LogTest   (logLevel()>0 ? SniperLog::Logger::Silencer : SNIPERLOG(0))
#define LogDebug  (logLevel()>2 ? SniperLog::Logger::Silencer : SNIPERLOG(2))
#define LogInfo   (logLevel()>3 ? SniperLog::Logger::Silencer : SNIPERLOG(3))
#define LogWarn   (logLevel()>4 ? SniperLog::Logger::Silencer : SNIPERLOG(4))
#define LogError  (logLevel()>5 ? SniperLog::Logger::Silencer : SNIPERLOG(5))
#define LogFatal  (logLevel()>6 ? SniperLog::Logger::Silencer : SNIPERLOG(6))

#endif
