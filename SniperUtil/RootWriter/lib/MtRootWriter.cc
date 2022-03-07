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

#include "RootWriter/MtRootWriter.h"
#include "RootWriter/MtTTree.h"
#include "RootWriter/MtTTreeStore.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SvcFactory.h"
#include "TROOT.h"

DECLARE_SERVICE(MtRootWriter);

MtRootWriter::MtRootWriter(const std::string &name)
    : SharedElem<RootWriter>(name)
{
    ROOT::EnableThreadSafety();
}

MtRootWriter::~MtRootWriter()
{
}

TTree *MtRootWriter::bookTree(ExecUnit &domain, const std::string &fullPath, const std::string &title)
{
    // create the MtTTree object in current thread (and domain)
    std::string::size_type lSeg = fullPath.rfind('/');
    std::string name = fullPath.substr(lSeg + 1, std::string::npos);
    std::string fullDirs = fullPath.substr(0, lSeg);
    MtTTree *result = new MtTTree(name.c_str(), title.c_str());
    result->SetDirectory(nullptr);

    // TTree::CloneTree() for the destination TTree must be invoked after all branches have been created
    // so only a temporary placeholder with the right TDirectory is saved for each fullPath at this time
    // the proper value will replace the placeholder in the first invoking of MtTTree::DoFillOne()
    {
        const std::lock_guard<std::mutex> lock(m_uniqdest);
        auto it = m_dests.find(fullPath);
        if (it == m_dests.end())
        {
            auto dest = result->CloneEmptyTree();  // No branch has been created yet
            dest->SetDirectory(getDir(fullDirs));
            auto r = m_dests.insert(std::make_pair(fullPath, dest));
            result->m_dest = &(r.first->second);
        }
        else
        {
            result->m_dest = &(it->second);
        }
    }

    // get (or create) and set the MtTTreeStore
    auto dSvc = dynamic_cast<DataMemSvc*>(domain.findSvc("DataMemSvc"));
    auto store = dynamic_cast<MtTTreeStore*>(dSvc->find("MtTTreeStore"));
    if (store == nullptr)
    {
        store = new MtTTreeStore();
        dSvc->regist("MtTTreeStore", store);
    }
    result->m_store = store;

    return result;
}
