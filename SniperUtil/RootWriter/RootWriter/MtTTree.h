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

#ifndef SNIPER_MT_TTREE_H
#define SNIPER_MT_TTREE_H

#include "TTree.h"
#include <deque>
#include <mutex>

class MtTTreeStore;

class MtTTree : public TTree
{
public:
    friend class MtRootWriter;

    MtTTree(const char *name, const char *title);
    virtual ~MtTTree();

    virtual Int_t Fill() override;

    TTree* CloneEmptyTree();

    Int_t DoFillOne();

private:
    bool m_first;
    TTree **m_dest;
    MtTTreeStore *m_store;

    std::deque<TTree *> m_freeSlots;
    std::deque<TTree *> m_srcSlots;

    std::mutex m_firstMutex;
    std::mutex m_cloneMutex;
    std::mutex m_freeMutex;
    std::mutex m_srcMutex;
};

#endif