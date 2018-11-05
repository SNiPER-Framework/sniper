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

#ifndef SNIPER_ALG_BASE_H
#define SNIPER_ALG_BASE_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SniperLog.h"

class ToolBase;

class AlgBase : public DLElement
{
    public :

        AlgBase(const std::string& name);

        virtual ~AlgBase();

        virtual bool execute() = 0;

        //for handling tools if needed
        ToolBase* createTool(const std::string& toolName);
        ToolBase* findTool(const std::string& toolName);

        //template version of retrieving a Tool instance
        template<typename Type>
        Type* tool(const std::string& toolName);

        //Declared in base class DLElement
        //virtual bool initialize() = 0;
        //virtual bool finalize() = 0;

        virtual void show(int indent) override;

    protected :

        std::map<std::string, ToolBase*>  m_tools;
};

template<typename Type>
Type* AlgBase::tool(const std::string& toolName)
{
    return dynamic_cast<Type*>(this->findTool(toolName));
}

#endif
