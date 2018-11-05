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

#include "SniperKernel/DLElement.h"
#include "SniperKernel/Task.h"

DLElement::DLElement(const std::string& name)
    : NamedElement(name),
      m_par(nullptr),
      m_pmgr(this)
{
}

DLElement::~DLElement()
{
}

Task* DLElement::getRoot()
{
    DLElement* p = this;
    while ( p->m_par != nullptr ) {
        p = p->m_par;
    }
    return dynamic_cast<Task*>(p);
}

void DLElement::setParent(Task* par)
{
    m_logLevel = par->logLevel();
    m_scope = par->scope() + par->objName() + ':';
    m_par   = par;
}

Property* DLElement::property(const std::string& key)
{
    return m_pmgr.property(key);
}

void DLElement::show()
{
    this->show(0);
}

void DLElement::show(int indent)
{
    make_indent(indent);
    std::cout << '[' << m_tag << ']';
    if ( indent == 0 ) std::cout << m_scope;
    std::cout << m_name << std::endl;

    make_indent(indent+1);
    std::cout << "[ATR]LogLevel   = " << m_logLevel << std::endl;

    for ( auto& p : m_pmgr.properties() ) {
        p.second->show(indent+1);
    }
}

void DLElement::make_indent(int indent)
{
    if ( indent != 0 ) {
        for ( int i = 0; i < indent-1; ++i ) {
            std::cout << "   |  ";
        }
        std::cout << "   +--";
    }
}
