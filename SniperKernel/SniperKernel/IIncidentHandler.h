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

#ifndef SNIPER_I_INCIDENT_HANDLER_H
#define SNIPER_I_INCIDENT_HANDLER_H

#include <string>
#include <list>

class Task;
class Incident;

class IIncidentHandler
{
    public :

        IIncidentHandler(Task& domain);
        IIncidentHandler(Task* domain);

        virtual ~IIncidentHandler();

        virtual bool handle(Incident& incident) = 0;

        bool regist(const std::string& incident);

        void unregist(const std::string& incident);

        void listening();

    protected :

        Task&                   m_domain;

    private :

        long                    m_id;
        std::list<std::string>  m_msg;
};

#endif
