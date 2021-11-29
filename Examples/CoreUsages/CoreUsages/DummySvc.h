/* Copyright (C) 2021
   Institute of High Energy Physics and Shandong University
   This file is part of mt.sniper.
 
   mt.sniper is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   mt.sniper is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with mt.sniper.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef SNIPER_DUMMY_SVC_H
#define SNIPER_DUMMY_SVC_H

#include "SniperKernel/SvcBase.h"
#include <atomic>

class DummySvc : public SvcBase
{
public:
    DummySvc(const std::string &name);
    virtual ~DummySvc();

    bool initialize() override;
    bool finalize() override;

    void doSomeThing();

private:
    std::atomic_long m_count;
};

#endif
