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

AlgBase::AlgBase(const std::string &name)
    : DLElement(name)
{
}

AlgBase::~AlgBase()
{
    for (auto &t : m_tools)
    {
        delete t.second;
    }
    m_tools.clear();
}

ToolBase *AlgBase::createTool(const std::string &toolName)
{
    DLElement *obj = DLEFactory::instance().create(toolName);
    if (obj != 0)
    {
        ToolBase *result = dynamic_cast<ToolBase *>(obj);
        if (result != 0)
        {
            if (0 == this->findTool(result->objName()))
            {
                result->setParent(this->getParent());
                m_tools.insert(std::make_pair(result->objName(), result));
                return result;
            }
            else
            {
                LogError << "already exist tool: " << result->objName()
                         << std::endl;
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

SniperJSON AlgBase::json()
{
    SniperJSON j = DLElement::json();

    if (!m_tools.empty())
    {
        SniperJSON &jtools = j["tools"];

        for (auto &t : m_tools)
        {
            jtools.push_back(t.second->json());
        }
    }

    return j;
}

void AlgBase::eval(const SniperJSON &json)
{
    //eval for base class
    DLElement::eval(json);

    //eval the tools
    if (json.find("\"tools\"") != json.map_end())
    {
        auto &tools = json["tools"];
        for (auto it = tools.vec_begin(); it != tools.vec_end(); ++it)
        {
            ToolBase *tool = this->createTool((*it)["identifier"].get<std::string>());
            tool->eval(*it);
        }
    }
}
