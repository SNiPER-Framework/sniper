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

#ifndef SNIPER_MTS_INTER_ALG_NODE_H
#define SNIPER_MTS_INTER_ALG_NODE_H

#include "SniperKernel/MtsMicroTask.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/SniperQueue.h"
#include <atomic>

class MtsInterAlgDag;

class MtsInterAlgNode : public MtsMicroTask
{
public:
    MtsInterAlgNode(MtsInterAlgDag *dag, AlgBase *alg);
    virtual ~MtsInterAlgNode();

    // implement the pure virtual method in MtsMicroTask
    virtual Status exec() override;

    // set which algorithm(s) this node depends on
    void dependOn(const std::string &name);
    void dependOn(const std::vector<std::string> &names);

    // check whether there is any loop in the DAG
    bool validate(MtsInterAlgNode *node);

    // prepare for next execution
    void reset() { m_nPreLeft = m_nPre; }

protected:
    friend class MtsInterAlgDag;

    void dependOnNode(MtsInterAlgNode *node);
    Status spawnPost();

    MtsInterAlgDag *m_dag;
    AlgBase *m_alg;

    int m_nPre{0};
    std::atomic_int m_nPreLeft{0};
    std::vector<MtsInterAlgNode *> m_post;

    Sniper::Queue<MtsMicroTask *> m_postEggs{nullptr};

    // incidents
    Incident_T<AlgBase *> m_beginAlg;
    Incident_T<AlgBase *> m_endAlg;
};

class MtsInterAlgBeginNode : public MtsInterAlgNode
{
public:
    MtsInterAlgBeginNode(MtsInterAlgDag *dag, AlgBase *alg, long *done);
    virtual ~MtsInterAlgBeginNode() = default;

    virtual Status exec() override;

private:
    long &m_done;
    Incident_T<int> m_beginEvt;
};

class MtsInterAlgEndNode : public MtsInterAlgNode
{
public:
    MtsInterAlgEndNode(MtsInterAlgDag *dag, AlgBase *alg, long *done);
    virtual ~MtsInterAlgEndNode() = default;

    virtual Status exec() override;

private:
    long &m_done;
    Incident_T<int> m_endEvt;
};

#endif