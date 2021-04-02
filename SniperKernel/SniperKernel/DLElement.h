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

#ifndef SNIPER_DL_ELEMENT_H
#define SNIPER_DL_ELEMENT_H

#include "SniperKernel/NamedElement.h"
#include "SniperKernel/PropertyMgr.h"
#include <string>

class IExecUnit;

// The base class of Dynamically Loadable Element
class DLElement : public NamedElement
{
public:
    DLElement(const std::string &name);
    virtual ~DLElement();

    // pure virtual interfaces
    virtual bool initialize() = 0;
    virtual bool finalize() = 0;

    // return the tag flag: should be the derived concrete class name
    const std::string &tag() { return m_tag; }

    // get the parent (ExecUnit implementation) pointer
    IExecUnit *getParent() { return m_par; }

    // get the root node (ExecUnit implementation) pointer of the DLElements' tree
    IExecUnit *getRoot();

    // set the tag
    void setTag(const std::string &tag_) { m_tag = tag_; }

    // set the description
    void setDescription(const std::string &desc) { m_description = desc; }

    // set the parent (ExecUnit implementation) pointer
    void setParent(IExecUnit *parent);

    // get a property via its key-name
    Property *property(const std::string &key);

    // the json value of this object
    virtual SniperJSON json();

    // eval this object from json
    virtual void eval(const SniperJSON &json);

    // show its information
    void show();

protected:
    template <typename Type>
    bool declProp(const std::string &key, Type &var);

    //data members
    IExecUnit *m_par; //parent
    std::string m_tag;
    std::string m_description;
    PropertyMgr m_pmgr;

    // following interfaces are not supported
    DLElement() = delete;
    DLElement(const DLElement &) = delete;
    DLElement &operator=(const DLElement &) = delete;
};

template <typename Type>
bool DLElement::declProp(const std::string &key, Type &var)
{
    return m_pmgr.addProperty(key, var);
}

#endif
