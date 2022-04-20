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

#include <queue>

#include "SniperKernel/AlgBase.h"
#include "SniperKernel/DagTask.h"
#include "SniperKernel/DeclareDLE.h"
#include "SniperKernel/DLElement.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/SniperLog.h"
#include "SniperPrivate/AlgNode.h"
#include "SniperPrivate/DLEFactory.h"

SNIPER_DECLARE_DLE(DagTask);

DagTask::DagTask(const std::string& name) : TopTask(name) {
    m_tag = "DagTask";
}

DagTask::~DagTask() {
    for (auto p : m_algPtr)
        delete p.second;
}

AlgBase* DagTask::insertNode(const std::string& alg) {
    std::string name;
    // Create AlgBase* to return
    DLElement* obj = DLEFactory::instance().create(alg);
    if (obj != nullptr) {
        AlgBase* result = dynamic_cast<AlgBase*>(obj);
        if (result != nullptr) {
            name = result->objName();
            m_nodes.emplace_back(name);
            m_algPtr.insert({name, new AlgNode(result)});
            return result;
        }
    }
    return nullptr;
}

bool DagTask::makeEdge(const std::string& alg1, const std::string& alg2) {
    if (m_algPtr.find(alg1) == m_algPtr.end() || m_algPtr.find(alg2) == m_algPtr.end()) {
        LogError << "Can't create edge between " << alg1 << "and" << alg2
                 << ". Please check algs has been inserted." << std::endl;
        return false;
    }
    AlgNode* p1 = m_algPtr.at(alg1);
    AlgNode* p2 = m_algPtr.at(alg2);
    p1->nextNodes.emplace_back(p2);
    p2->preNum++;
    return true;
}

SniperJSON DagTask::json() {
    SniperJSON j = TopTask::json();
    j["ordered_keys"].push_back(SniperJSON().from("nodes"));
    j["ordered_keys"].push_back(SniperJSON().from("edges"));
    if (!m_nodes.empty()) {
        SniperJSON& jnodes = j["nodes"];
        SniperJSON& jedges = j["edges"];
        for (auto it = m_nodes.rbegin(); it != m_nodes.rend(); ++it) {
            AlgNode* cur = m_algPtr.at(*it);
            jnodes.push_back(cur->realAlg->json());
            for (auto e : cur->nextNodes)
                jedges.push_back('"' + cur->realAlg->objName()
                                 + '-' + e->realAlg->objName() + '"');
        }
    }
    auto& jalgs = j["algorithms"];
    jalgs = SniperJSON::loads("[]");
    return j;
}

void DagTask::eval(const SniperJSON& json) {
    // eval for base class
    TopTask::eval(json);

    // eval nodes and edges
    auto& nodes = json["nodes"];
    for (auto it = nodes.vec_begin(); it != nodes.vec_end(); ++it) {
        auto idStr = (*it)["identifier"].get<std::string>();
        if (idStr.front() != '[') {
            auto alg = this->insertNode(idStr);
            alg->eval(*it);
        }
    }

    auto& edges = json["edges"];
    for (auto it = edges.vec_begin(); it != edges.vec_end(); ++it) {
        auto edge = (*it).get<std::string>();
        std::string::size_type p = 0;
        while (edge[p] != '-')
            p++;
        this->makeEdge(edge.substr(0, p), edge.substr(p + 1, edge.size() - p - 1));
    }

    this->done();
}

bool DagTask::config() {
    bool stat = TopTask::config();
    if (!stat)
        m_snoopy.setErr();
    return stat;
}

bool DagTask::initialize() {
    bool stat = TopTask::initialize();
    if (!stat)
        m_snoopy.setErr();
    return stat;
}

bool DagTask::finalize() {
    bool stat = TopTask::finalize();
    if (!stat)
        m_snoopy.setErr();
    return stat;
}

bool DagTask::execute() {

    bool stat = TopTask::execute();
    if (!stat)
        m_snoopy.setErr();
    return stat;
}

bool DagTask::done() {
    bool flag = true;
    // The real order in which the algorithm is executed.
    std::vector<AlgNode*> realSeq;
    this->getSequence(realSeq);

    for (auto p : realSeq) {
        if (this->addAlg( m_algPtr.at(p->realAlg->objName())->realAlg ) == nullptr)
            flag = false;
    }
    return flag;
}

void DagTask::getSequence(std::vector<AlgNode*>& realSeq) {
    // AlgNodes that can be traversed now.
    std::queue<AlgNode*> zeroInQue;
    // Get starting node
    for (const auto& it : m_algPtr) {
        if (it.second->preNum == 0)
            zeroInQue.emplace(it.second);
    }
    AlgNode* cur = nullptr;
    while (!zeroInQue.empty()) {
        cur = zeroInQue.front();
        zeroInQue.pop();
        realSeq.emplace_back(cur);
        for (AlgNode* next : cur->nextNodes) {
            if (--next->preNum == 0)
                zeroInQue.push(next);
        }
    }
}