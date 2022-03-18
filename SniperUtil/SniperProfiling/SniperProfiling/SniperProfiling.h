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

#ifndef SNIPER_PROFILING_H
#define SNIPER_PROFILING_H

#include "SniperKernel/SvcBase.h"
#include "SniperKernel/SniperTimerSvc.h"
#include <vector>
#include <map>
#include <string>

namespace SnierProfilingNS
{
    struct BeginEvtHandler;
    struct EndEvtHandler;
    struct BeginAlgHandler;
    struct EndAlgHandler;
}
namespace sp = SnierProfilingNS;
class SniperProfiling : public SvcBase
{
public:
    SniperProfiling(const std::string& name) : SvcBase(name) {}
    ~SniperProfiling() {}

    bool initialize();
    bool finalize();

private:
    SniperTimer* m_evtTimer;
    std::vector<std::string> m_algName;
    std::map<std::string, SniperTimer*> m_algTimer;

    sp::BeginEvtHandler* m_beginEvtHdl;
    sp::EndEvtHandler*   m_endEvtHdl;
    sp::BeginAlgHandler* m_beginAlgHdl;
    sp::EndAlgHandler*   m_endAlgHdl;
};

#endif