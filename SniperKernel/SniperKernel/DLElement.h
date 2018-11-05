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

#ifndef SNIPER_DL_ELEMENT_H
#define SNIPER_DL_ELEMENT_H

#include "SniperKernel/NamedElement.h"
#include "SniperKernel/PropertyMgr.h"
#include <string>

class Task;

// The base class of Dynamically Loadable Element
class DLElement : public NamedElement
{
    public :

        DLElement(const std::string& name);
        virtual ~DLElement();

        // pure virtual interfaces
        virtual bool initialize() = 0;
        virtual bool finalize() = 0;

        // return the tag flag: should be the derived concrete class name
        const std::string& tag() { return m_tag; }

        // get the parent (Task) pointer
        Task* getParent() { return m_par; }

        // get the root node (Task) pointer of the DLElements' tree
        Task* getRoot();

        // set the tag
        void setTag(const std::string& tag) { m_tag = tag; }

        // set the parent (Task) pointer
        void setParent(Task* parent);

        // get a property via its key-name
        Property* property(const std::string& key);

        // show its information
        void show();

        // for deep level object, show information with indent
        virtual void show(int indent);


    protected :

        template<typename Type>
        bool declProp(const std::string& key, Type& var);

        // offer a better look for show()
        void make_indent(int indent);

        //data members
        Task*         m_par;  //parent
        std::string   m_tag;
        PropertyMgr   m_pmgr;

        // following interfaces are not supported
        DLElement() = delete;
        DLElement(const DLElement&) = delete;
        DLElement& operator=(const DLElement&) = delete;
};

template<typename Type>
bool DLElement::declProp(const std::string& key, Type& var)
{
    return m_pmgr.addProperty(key, var);
}

#endif
