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

#ifndef SNIPER_SNIPER_H
#define SNIPER_SNIPER_H

#include <string>

namespace Sniper
{
    void setLogLevel(int level);
    void setColorful(int level);
    void setShowTime(bool flag);
    void setLogFile(char *fname, bool append = false);
    void setLogStdout();

    void loadDll(char *dll);

    namespace System
    {
        ///
        const std::string &hostName();

        ///
        std::string sysDate();
    }

    namespace Config
    {
        std::string json_str();
    }
}

#endif
