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

#ifndef SNIPER_SNIPER_CONTEXT_H
#define SNIPER_SNIPER_CONTEXT_H

#include <list>
#include <string>

namespace Sniper
{
    typedef unsigned short SysModeInt;

    // platform (hardware & system software) related environment
    enum class SysMode : SysModeInt
    {
        BASIC = 0x01, //classical serialized mode
        MT = 0x02,    //multi-threads
        MPI = 0x04,
        GPU = 0x08 //and more...
    };

    class Context final
    {
    public:
        Context();
        ~Context();

        const std::string &hostName();

        void set(const SysMode &mode);
        void set_threads(short nt);
        bool check(const SysMode &mode);

        std::string sys_info();
        std::string summary();

        void reg_msg(const std::string &msg);

    private:
        bool m_greeting;
        bool m_summary;
        SysModeInt m_mode;
        short m_nt;
        std::string m_hostName;
        std::list<std::string> m_msg;

        Context(const Context &) = delete;
        Context &operator=(const Context &) = delete;
    };
}

extern Sniper::Context *sniper_context;

#endif
