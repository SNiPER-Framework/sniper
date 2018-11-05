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

#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include "NonUserIf/DLEFactory.h"

AlgBase::AlgBase(const std::string& name)
    : DLElement(name)
{
}

AlgBase::~AlgBase()
{
    for ( auto& t : m_tools ) {
        delete t.second;
    }
    m_tools.clear();
}

ToolBase* AlgBase::createTool(const std::string& toolName)
{
    DLElement* obj = DLEFactory::instance().create(toolName);
    if ( obj != 0 ) {
        ToolBase* result = dynamic_cast<ToolBase*>(obj);
        if ( result != 0 ) {
            if ( 0 == this->findTool( result->objName() ) ) {
                result->setParent(this->getParent());
                m_tools.insert(std::make_pair(result->objName(), result));
                return result;
            }
            else {
                LogError << "already exist tool: " << result->objName()
                         << std::endl;
            }
        }
        else {
            LogFatal << obj->objName() << " cannot cast to ToolBase."
                     << std::endl;
        }
        delete obj;
    }
    return nullptr;
}

ToolBase* AlgBase::findTool(const std::string& toolName)
{
    auto it = m_tools.find(toolName);
    if ( it != m_tools.end() ) {
        return (*it).second;
    }
    //LogInfo << "cannot find tool: " << toolName << std::endl;
    return nullptr;
}

void AlgBase::show(int indent)
{
    DLElement::show(indent);

    for ( auto& t : m_tools ) {
        t.second->show(indent+1);
    }
}
