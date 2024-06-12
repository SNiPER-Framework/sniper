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

#include "SniperPrivate/MtsInterAlgNode.h"
#include "SniperPrivate/MtsInterAlgDag.h"
#include "SniperKernel/MtsMicroTaskQueue.h"

MtsInterAlgNode::MtsInterAlgNode(MtsInterAlgDag *dag, AlgBase *alg)
    : m_dag(dag),
      m_alg(alg),
      m_beginAlg("BeginAlg"),
      m_endAlg("EndAlg")
{
    m_name = "MtsInterAlgNode";

    m_beginAlg.load(m_alg);
    m_endAlg.load(m_alg);
}

MtsInterAlgNode::~MtsInterAlgNode()
{
}

MtsMicroTask::Status MtsInterAlgNode::exec()
{
    // TODO: try & catch exceptions
    bool status;
    {
        ScopedIncidentsPair sis{m_beginAlg, m_endAlg, m_dag->host()};
        status = m_alg->execute();
    }

    return status ? spawnPost() : Status::Failed;
}

void MtsInterAlgNode::dependOn(MtsInterAlgNode *node)
{
    node->m_post.push_back(this);
    ++m_nPre;
}

bool MtsInterAlgNode::validate(MtsInterAlgNode *node)
{
    for (auto post : m_post)
    {
        if (post == node || !post->validate(node) || !post->validate(post))
        {
            LogDebug << "in loop node: " << m_alg->objName() << std::endl;
            return false;
        }
    }
    return true;
}

MtsMicroTask::Status MtsInterAlgNode::spawnPost()
{
    int nEggs = 0;
    for (auto post : m_post)
    {
        if (--post->m_nPreLeft == 0)
        {
            m_postEggs.push(post);
            ++nEggs;
        }
    }

    if (nEggs > 1)
    {
        static auto *queue = MtsMicroTaskQueue::instance();
        queue->enqueue(m_postEggs);
    }
    else if (nEggs == 1)
    {
        return m_postEggs.pop()->exec();
    }

    return Status::OK;
}

MtsInterAlgBeginNode::MtsInterAlgBeginNode(MtsInterAlgDag *dag, AlgBase *alg, long *done)
    : MtsInterAlgNode(dag, alg),
      m_done(*done),
      m_beginEvt("BeginEvent")
{
}

MtsMicroTask::Status MtsInterAlgBeginNode::exec()
{
    m_beginEvt.load(m_done).fire(m_dag->host());
    return spawnPost();
}

MtsInterAlgEndNode::MtsInterAlgEndNode(MtsInterAlgDag *dag, AlgBase *alg, long *done)
    : MtsInterAlgNode(dag, alg),
      m_done(*done),
      m_endEvt("EndEvent")
{
}

MtsMicroTask::Status MtsInterAlgEndNode::exec()
{
    m_endEvt.load(m_done).fire(m_dag->host());

    if (!m_dag->isErr())
    {
        ++m_done;
        return Status::OK;
    }
    return Status::Failed;
}