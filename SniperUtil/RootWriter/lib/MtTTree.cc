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

#include "RootWriter/MtTTree.h"
#include "RootWriter/MtTTreeStore.h"

MtTTree::MtTTree(const char *name, const char *title)
    : TTree(name, title),
      m_first(true)
{
}

MtTTree::~MtTTree()
{
    for (auto clone : m_freeSlots)
    {
        clone->Delete();
    }
    for (auto clone : m_srcSlots)
    {
        clone->Delete();
    }
}

Int_t MtTTree::Fill()
{
    TTree* clone = nullptr;

    // reuse a free clone if exist
    {
        const std::lock_guard<std::mutex> lock(m_freeMutex);
        if (!m_freeSlots.empty())
        {
            clone = m_freeSlots.back();
            m_freeSlots.pop_back();
        }
    }
    // create a clone if no free is available
    if (clone == nullptr)
    {
        clone = CloneEmptyTree();  //this->CloneTree(0);
        clone->SetDirectory(nullptr);
    }

    // fill the clone and put it to the source queue
    auto res = clone->Fill();
    {
        const std::lock_guard<std::mutex> lock(m_srcMutex);
        m_srcSlots.emplace_back(clone);
    }

    // it is added to the store each time when a new entry is filled
    m_store->add(this);

    return res;
}

TTree *MtTTree::CloneEmptyTree()
{
    const std::lock_guard<std::mutex> lock(m_cloneMutex);
    return this->CloneTree(0);
}

Int_t MtTTree::DoFillOne()
{
    if (m_first)
    {
        const std::lock_guard<std::mutex> lock(m_firstMutex);
        if (m_first && (*m_dest)->GetNbranches() == 0)
        {
            auto dest = CloneEmptyTree();  //this->CloneTree(0);
            dest->ResetBranchAddresses();
            dest->SetDirectory((*m_dest)->GetDirectory());
            (*m_dest)->SetDirectory(nullptr);  //(*m_dest)->Delete() will break the program
            (*m_dest) = dest;
            m_first = false;
        }
    }

    TTree* clone = nullptr;
    {
        const std::lock_guard<std::mutex> lock(m_srcMutex);
        clone = m_srcSlots.front();
        m_srcSlots.pop_front();
    }

    (*m_dest)->CopyAddresses(clone);
    clone->GetEntry(0);
    auto res = (*m_dest)->Fill();

    clone->Reset();
    this->CopyAddresses(clone);
    {
        const std::lock_guard<std::mutex> lock(m_freeMutex);
        m_freeSlots.emplace_back(clone);
    }

    return res;
}