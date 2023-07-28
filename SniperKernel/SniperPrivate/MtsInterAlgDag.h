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

#ifndef SNIPER_MTS_INTER_ALG_DAG_H
#define SNIPER_MTS_INTER_ALG_DAG_H

#include "SniperKernel/TaskWatchDog.h"
#include "SniperKernel/ExecUnit.h"
//#include "SniperKernel/SniperJSON.h"

class MtsInterAlgNode;

class MtsInterAlgDag final : public TaskWatchDog
{
public:
    MtsInterAlgDag(ExecUnit *task);
    virtual ~MtsInterAlgDag();

    // get (or create if not exist) a node by its alg name
    MtsInterAlgNode *node(const std::string &name);

    // override interfaces in the base class
    virtual bool config() override;
    virtual bool run_once() override;

    //// the json value of this object
    // virtual SniperJSON json();
    //// eval this graph object from json
    // virtual void eval(const SniperJSON &json);

private:
    // the event number has been processed
    long m_done{0};

    // the unique begin and end node of the dag
    MtsInterAlgNode *m_begin;
    MtsInterAlgNode *m_end;

    // algorithm nodes in the dag
    std::map<std::string, MtsInterAlgNode *> m_nodes;
};

#endif