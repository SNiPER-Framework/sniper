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

#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include "SniperPrivate/DLEFactory.h"
#include "SniperPrivate/SharedElemMgr.h"

AlgBase::AlgBase(const std::string &name)
    : DLElement(name)
{
}

AlgBase::~AlgBase()
{
    m_tools.clear();
    for (auto it = m_vtools.rbegin(); it != m_vtools.rend(); ++it)
    {
        if (it->second) delete it->first;
    }
}

ToolBase *AlgBase::createTool(const std::string &toolName)
{
    DLElement *obj = DLEFactory::instance().create(toolName);
    if (obj != 0)
    {
        ToolBase *result = dynamic_cast<ToolBase *>(obj);
        if (result != 0)
        {
            if (this->addTool(result) != nullptr)
            {
                m_vtools.back().second = true;
                return result;
            }
        }
        else
        {
            LogFatal << obj->objName() << " cannot cast to ToolBase."
                     << std::endl;
        }
        delete obj;
    }
    return nullptr;
}

ToolBase *AlgBase::addTool(ToolBase *tool)
{
    if (m_tools.find(tool->objName()) == m_tools.end())
    {
        tool->setParentAlg(this);
        m_vtools.push_back(std::make_pair(tool, false));
        m_tools.insert(std::make_pair(tool->objName(), tool));
        return tool;
    }
    else
    {
        LogError << "already exist tool: " << tool->objName() << std::endl;
    }
    return nullptr;
}

ToolBase *AlgBase::findTool(const std::string &toolName)
{
    auto it = m_tools.find(toolName);
    if (it != m_tools.end())
    {
        return (*it).second;
    }
    //LogInfo << "cannot find tool: " << toolName << std::endl;
    return nullptr;
}

void AlgBase::setParent(ExecUnit *parent)
{
    DLElement::setParent(parent);
    for (auto &it : m_vtools)
    {
        it.first->setParentAlg(this);
    }
}

SniperJSON AlgBase::json()
{
    SniperJSON j = DLElement::json();

    if (!m_vtools.empty())
    {
        SniperJSON &jtools = j["tools"];

        for (auto &t : m_vtools)
        {
            jtools.push_back(t.first->json());
        }
    }

    return j;
}

void AlgBase::eval(const SniperJSON &json)
{
    //eval for base class
    DLElement::eval(json);

    //eval the tools
    if (json.find("tools") != json.map_end())
    {
        auto &tools = json["tools"];
        for (auto it = tools.vec_begin(); it != tools.vec_end(); ++it)
        {
            auto idStr = (*it)["identifier"].get<std::string>();
            if (idStr.front() != '[')
            {
                ToolBase *tool = this->createTool(idStr);
                tool->eval(*it);
            }
            else
            {
                ToolBase *tool = dynamic_cast<ToolBase *>(SharedElemMgr::get(idStr));
                this->addTool(tool);
            }
        }
    }
}
