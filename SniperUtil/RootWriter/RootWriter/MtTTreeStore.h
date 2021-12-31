/* Copyright (C) 2021
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

#ifndef SNIPER_MT_TTREE_STORE_H
#define SNIPER_MT_TTREE_STORE_H

#include "SniperKernel/IDataBlock.h"
#include "RootWriter/MtTTree.h"

class MtTTreeStore : public IDataBlock
{
public:
    MtTTreeStore();
    virtual ~MtTTreeStore();

    std::vector<MtTTree *> &trees() { return m_trees; }

    void add(MtTTree *tree) { m_trees.emplace_back(tree); }
    void clear() { m_trees.clear(); }

private:
    std::vector<MtTTree*> m_trees;
};

#endif