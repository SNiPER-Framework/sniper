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

#ifndef SNIPER_SINGLE_THREAD_SNIPER_H
#define SNIPER_SINGLE_THREAD_SNIPER_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/Task.h"

/// This is a simple engine for single thread Sniper
/// As contrast, MtSniper is the engine for multithreading computing

class SingleThreadSniper final : public DLElement
{
public:
    SingleThreadSniper(const std::string &name = "Sniper");
    ~SingleThreadSniper();

    Task *createTask(const std::string &identifier);
    bool run();

    // the interfaces for json
    virtual SniperJSON json() override;
    virtual void eval(const SniperJSON &json) override;

private:
    // implement the pure virtual interfaces in DLElement
    virtual bool initialize() override;
    virtual bool finalize() override;

    Task *m_task{nullptr};
};

#endif