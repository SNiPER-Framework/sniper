/* Copyright (C) 2023
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

#include "SniperPrivate/MtsInterAlgDag.h"
#include "SniperPrivate/MtsInterAlgNode.h"
#include "SniperKernel/SniperException.h"

MtsInterAlgDag::MtsInterAlgDag(ExecUnit *task)
    : TaskWatchDog(task)
{
    long *done = evtDone();
    if (done == nullptr)
    {
        done = &m_done;
    }

    m_begin = new MtsInterAlgBeginNode(this, nullptr, done);
    m_end = new MtsInterAlgEndNode(this, nullptr, done);
}

MtsInterAlgDag::~MtsInterAlgDag()
{
    for (auto &node : m_nodes)
        delete node.second;

    delete m_begin;
    delete m_end;
}

MtsInterAlgNode *MtsInterAlgDag::node(const std::string &name)
{
    auto inode = m_nodes.find(name);
    if (inode == m_nodes.end())
    {
        auto pAlg = m_task.findAlg(name);
        if (pAlg != nullptr)
        {
            auto node = new MtsInterAlgNode(this, pAlg);
            m_nodes[name] = node;
            return node;
        }
        throw ContextMsgException(name + " not found in " + m_task.objName());
    }
    return inode->second;
}

bool MtsInterAlgDag::config()
{
    // create nodes for the algorithms that the Dag doesn't know yet
    auto &algs = algList();
    for (auto alg : algs)
    {
        if (m_nodes.find(alg->objName()) == m_nodes.end())
        {
            m_nodes[alg->objName()] = new MtsInterAlgNode(this, dynamic_cast<AlgBase *>(alg));
        }
    }

    // set the dependencies correlated to the begin and end nodes
    for (auto &node : m_nodes)
    {
        auto pNode = node.second;
        if (pNode->m_nPre == 0)
        {
            pNode->dependOnNode(m_begin);
        }
        if (pNode->m_post.empty())
        {
            m_end->dependOnNode(pNode);
        }
    }

    // if the DAG is empty, there should be a way to invoke the end node
    if (m_nodes.empty())
    {
        m_end->dependOnNode(m_begin);
    }

    if (!m_begin->validate(m_begin))
    {
        throw ContextMsgException("loop found in MtsInterAlgDag");
    }

    return TaskWatchDog::config();
}

bool MtsInterAlgDag::run_once()
{
    for (auto &node : m_nodes)
    {
        node.second->reset();
    }
    m_end->reset();

    m_begin->exec();

    return true;
}

SniperJSON MtsInterAlgDag::json()
{
    SniperJSON json = SniperJSON::loads("{}");
    for (auto &node : m_nodes)
    {
        auto &nodeName = node.second->m_alg->objName();
        for (auto post : node.second->m_post)
        {
            if (post != m_end)
            {
                json[post->m_alg->objName()].push_back(SniperJSON().from(nodeName));
            }
        }
    }

    return json;
}

void MtsInterAlgDag::eval(const SniperJSON &json)
{
    for (auto jNode = json.map_begin(); jNode != json.map_end(); ++jNode)
    {
        node(jNode->first)->dependOn(jNode->second.get<std::vector<std::string>>());
    }
}