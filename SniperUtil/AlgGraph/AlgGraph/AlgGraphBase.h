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

#ifndef SNIPER_ALG_GRAPH_BASE_H
#define SNIPER_ALG_GRAPH_BASE_H

#include <string>

#include "SniperKernel/Task.h"

class AlgBase;

class AlgGraphBase : public Task {

public:
    AlgGraphBase(const std::string& name);
    virtual ~AlgGraphBase();

    virtual AlgBase* insertNode(const std::string& alg) = 0;
    virtual bool makeEdge(const std::string& alg1, const std::string& alg2) = 0;
    virtual bool done() = 0;
};

#endif