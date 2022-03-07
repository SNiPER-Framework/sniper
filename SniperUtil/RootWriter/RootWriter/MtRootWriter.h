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

#ifndef SNIPER_MT_ROOT_WRITER_H
#define SNIPER_MT_ROOT_WRITER_H

#include "RootWriter/RootWriter.h"
#include "SniperKernel/SharedElemFactory.h"

/* This is a RootWriter that running in the context of multithreading.
   It can be thread-safe for:
      - TTrees that booked by this RootWriter
   but not thread-safe for other TObjects that explicitly attached by users
   Limits:
      - MtTTree::DoFillOne() must be invoked serially somewhere (such as an output service)
      - events(entry) order can be keeped in cooperation with the GlobalBuffer in mt.sniper
*/

class MtRootWriter : public SharedElem<RootWriter>
{
public:
    MtRootWriter(const std::string &name);
    virtual ~MtRootWriter();

    // for the thread-safe TTree
    virtual TTree *bookTree(ExecUnit &domain, const std::string &fullPath, const std::string &title) override;

private:
    std::mutex m_uniqdest;
    std::map<std::string, TTree *> m_dests;
};

#endif