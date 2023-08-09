/* Copyright (C) 2018-2023
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

#ifndef SNIPER_ALG_BASE_H
#define SNIPER_ALG_BASE_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "SniperKernel/SniperLog.h"

class ToolBase;

class AlgBase : public DLElement
{
public:
    AlgBase(const std::string &name);

    virtual ~AlgBase();

    virtual bool execute() = 0;

    // for handling tools if needed
    ToolBase *createTool(const std::string &toolName);
    ToolBase *addTool(ToolBase *tool);
    ToolBase *findTool(const std::string &toolName);

    // template version of retrieving a Tool instance
    template <typename Type>
    Type *tool(const std::string &toolName);

    // set the parent (ExecUnit implementation) pointer
    virtual void setParent(ExecUnit *parent) override;

    // Declared in base class DLElement
    // virtual bool initialize() = 0;
    // virtual bool finalize() = 0;

    // the data dependencies
    const std::vector<std::string> &inputs() { return m_inputs; }
    const std::vector<std::string> &outputs() { return m_outputs; }

    // the json value of this object
    virtual SniperJSON json() override;
    // eval this Algorithm from json
    virtual void eval(const SniperJSON &json) override;

protected:
    std::map<std::string, ToolBase *> m_tools;
    std::vector<std::pair<ToolBase *, bool>> m_vtools;

    // calculate dependencies automatically in inter-alg MtSniper
    std::vector<std::string> m_inputs;
    std::vector<std::string> m_outputs;
};

template <typename Type>
Type *AlgBase::tool(const std::string &toolName)
{
    return dynamic_cast<Type *>(this->findTool(toolName));
}

#endif
