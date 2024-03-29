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

#ifndef SNIPER_DAG_TASK_H
#define SNIPER_DAG_TASK_H

#include <string>
#include <unordered_map>
#include <vector>

#include "SniperKernel/DagBase.h"

struct AlgNode;
class SniperJSON;

class DagTask final : public DagBase {

public:
    DagTask(const std::string& name);
    virtual ~DagTask();

    virtual AlgBase* insertNode(const std::string& alg) override;
    virtual bool makeEdge(const std::string& alg1, const std::string& alg2) override;
    virtual bool done() override;

    //the json value of this object
    virtual SniperJSON json() override;
    // eval this graph object from json
    virtual void eval(const SniperJSON &json) override;

private:
    // Record the sequence of nodes being inserted.
    std::vector<std::string> m_nodes;
    // Record algs and their corresponding pointers.
    std::unordered_map<std::string, AlgNode*> m_algPtr;
    // Calculate the real sequence.
    void getSequence(std::vector<AlgNode*>& realSeq);
};

#endif