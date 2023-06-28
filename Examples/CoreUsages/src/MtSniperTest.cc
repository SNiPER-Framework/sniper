/* Copyright (C) 2023
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

#include "SniperKernel/MtsEvtBufferRing.h"
#include "SniperKernel/MtSniperContext.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include "SniperKernel/DeclareDLE.h"

typedef SniperJSON JsonEvent;

typedef MtsEvtBufferRing<JsonEvent> GlobalBuffer4JsonEvent;
SNIPER_DECLARE_DLE(GlobalBuffer4JsonEvent);

////////////////////////////////////////////////////////////////////////////////
class FillGlobalBufAlg : public AlgBase
{
public:
    FillGlobalBufAlg(const std::string &name) : AlgBase(name) {}
    virtual ~FillGlobalBufAlg() = default;

    virtual bool initialize() override { return true; }
    virtual bool execute() override;
    virtual bool finalize() override { return true; }
};
SNIPER_DECLARE_DLE(FillGlobalBufAlg);

bool FillGlobalBufAlg::execute()
{
    static auto gbptr = dynamic_cast<GlobalBuffer4JsonEvent *>(mt_sniper_context->global_buffer);
    static std::map<std::string, long> cppEvt{{"Event", -1}};
    static auto ievt = cppEvt.find("Event");

    ++(ievt->second);

    JsonEvent* pevt = new JsonEvent();
    pevt->from(cppEvt);

    gbptr->push_back(std::make_shared<JsonEvent>(pevt));

    return true;
}

////////////////////////////////////////////////////////////////////////////////