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

class DLElement;

namespace Sniper
{
    //create a DLElement from a json string
    DLElement *create(const std::string &json_str);
    //eval a SNiPER job from a json config file
    DLElement *eval(const char *fname);

    void setLogLevel(int level);
    void setColorful(int level);
    void setShowTime(bool flag);
    void setLogFile(const char *fname, bool append = false);
    void setLogStdout();

    //load a dll into SNiPER
    void loadDll(const char *dll);
    //the names of loaded DLLs (the return value is in json format)
    std::string dlls();
    //the names of valid DLEs (the return value is in json format)
    std::string validTypes();

    namespace System
    {
        ///
        std::string sysDate();
    }

    namespace Config
    {
        std::string json_str();
        void eval(const std::string &json_str);
    }
}

#endif
