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

#ifndef DUMMY_TOOL_H
#define DUMMY_TOOL_H

#include "SniperKernel/ToolBase.h"

class DummyTool : public ToolBase
{
public:
    DummyTool(const std::string &name);
    virtual ~DummyTool();

    bool initialize() override;
    bool finalize() override;

    void doSomeThing();

private:
    int m_count;
};

#endif
