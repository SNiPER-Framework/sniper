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

#ifndef SNIPER_TASK_PROPERTY_H
#define SNIPER_TASK_PROPERTY_H

#include "SniperKernel/Property.h"
#include <iostream>

class TaskProperty : public SniperProperty<std::vector<std::string> >
{
    public :

        typedef SniperProperty<std::vector<std::string> > BaseType;

        TaskProperty(const std::string&key, Task* domain)
            : BaseType(key, m_names), m_domain(domain)
        {
            if ( key == "algs") {
                pclear  = &Task::clearAlgs;
                pcreate = &TaskProperty::createAlg;
                padd    = &TaskProperty::addAlg;
            }
            else if ( key == "svcs" ) {
                pclear  = &Task::clearSvcs;
                pcreate = &TaskProperty::createSvc;
                padd    = &TaskProperty::addSvc;
            }
            else {
                throw ContextMsgException( key + " : invalid TaskProperty Key");
            }
        }

        bool set(bp::object& var) {
            if ( ! m_names.empty() ) {
                std::cout << "WARNING :: " << m_key
                          << " is not empty, potential ERRORS"
                          << std::endl;
                m_names.clear();
                (m_domain->*pclear)();
            }
            return this->append(var);
        }

        bool append(bp::object& var) {
            bp::extract<DLElement*> _var(var);
            if ( _var.check() ) { //python alg/svc/task object
                DLElement* obj = _var();
                return (this->*padd)(obj);
            }
            unsigned int begin = m_names.size();
            if ( !BaseType::append(var) ) {
                return false;
            }
            for (unsigned int i = begin; i < m_names.size(); ++i ) {
                if ( ! (this->*pcreate)(m_names[i]) ) {
                    return false;
                }
            }
            return true;
        }

    private :

        bool createAlg(const std::string& name) {
            return m_domain->createAlg(name) != 0;
        }
        bool createSvc(const std::string& name) {
            return m_domain->createSvc(name) != 0;
        }
        bool addAlg(DLElement* alg) {
            return m_domain->addAlg(dynamic_cast<AlgBase*>(alg)) != 0;
        }
        bool addSvc(DLElement* svc) {
            return m_domain->addSvc(dynamic_cast<SvcBase*>(svc)) != 0;
        }

        void (Task::*pclear)();
        bool (TaskProperty::*pcreate)(const std::string&);
        bool (TaskProperty::*padd)(DLElement*);

        Task*  m_domain;
        std::vector<std::string>  m_names;
};

#endif
