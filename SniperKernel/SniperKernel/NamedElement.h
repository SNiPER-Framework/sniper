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

#ifndef SNIPER_NAMED_ELEMENT_H
#define SNIPER_NAMED_ELEMENT_H

#include <string>

class NamedElement
{
    public :

        NamedElement(const std::string& name);
        NamedElement(const std::string& scope, const std::string& name);
        virtual ~NamedElement() = default;

        // get log level
        int  logLevel() { return m_logLevel; }

        // scope name (empty in case of in top task)
        const std::string& scope() { return m_scope; }

        // return the name of dynamically loadable object
        const std::string& objName() { return m_name; }

        // set log level
        virtual void setLogLevel(int level) { m_logLevel = level; }


    protected :

        int           m_logLevel;
        std::string   m_scope;
        std::string   m_name;


        NamedElement() = delete;
};

#endif
