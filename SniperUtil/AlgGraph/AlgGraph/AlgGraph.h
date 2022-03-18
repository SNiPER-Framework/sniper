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

#ifndef SNIPER_ALG_GRAPH_H
#define SNIPER_ALG_GRAPH_H

#include <string>
#include <unordered_map>

#include "AlgGraphBase.h"

struct AlgNode;
class AlgBase;
class SniperJSON;

class AlgGraph : public AlgGraphBase {

public:
    AlgGraph(const std::string& name);
    virtual ~AlgGraph();

    AlgBase* insertNode(const std::string& alg) override;
    bool makeEdge(const std::string& alg1, const std::string& alg2) override;
    bool done() override;

    //the json value of this object
    virtual SniperJSON json() override;
    // eval this graph object from json
    virtual void eval(const SniperJSON &json) override;

protected:
    virtual bool config() override;
    virtual bool initialize() override;
    virtual bool finalize() override;
    virtual bool execute() override;

private:
    // Record the sequence of nodes being inserted.
    std::vector<std::string> m_nodes;
    // Record algs and their corresponding pointers.
    std::unordered_map<std::string, AlgNode*> m_algPtr;
    // Calculate the real sequence.
    void getSequence(std::vector<AlgNode*>& realSeq);
};

#endif