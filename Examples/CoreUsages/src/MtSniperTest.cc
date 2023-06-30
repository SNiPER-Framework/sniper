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

#include "SniperKernel/MtSniperContext.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include "SniperKernel/DeclareDLE.h"
#include <memory>
#include <fstream>

typedef SniperJSON JsonEvent;

////////////////////////////////////////////////////////////////////////////////
class FillGlobalBufAlg : public AlgBase
{
public:
    FillGlobalBufAlg(const std::string &name);
    virtual ~FillGlobalBufAlg() = default;

    virtual bool initialize() override { return true; }
    virtual bool execute() override;
    virtual bool finalize() override { return true; }

private:
    long m_max;
};
SNIPER_DECLARE_DLE(FillGlobalBufAlg);

FillGlobalBufAlg::FillGlobalBufAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("MaxEvtNum", m_max = 0x7FFFFFFFFFFFFFFF);
}

bool FillGlobalBufAlg::execute()
{
    static auto gbptr = mt_sniper_context->global_buffer;
    static std::map<std::string, long> cppEvt{{"EventID", -1}};
    static auto ievt = cppEvt.find("EventID");

    if (++(ievt->second) >= m_max) //;  //increase the EventID by 1
    {
        gbptr->deVigorous();
        return true;
    }

    auto pevt = std::make_shared<JsonEvent>(cppEvt);

    // set the input value
    double input = 9.9;
    (*pevt)["input"].from(input);

    gbptr->push_back(pevt);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
class PruneGlobalBufAlg : public AlgBase
{
public:
    PruneGlobalBufAlg(const std::string &name);
    virtual ~PruneGlobalBufAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override;

private:
    std::string m_ofname;
    std::ofstream m_ofs;
};
SNIPER_DECLARE_DLE(PruneGlobalBufAlg);

PruneGlobalBufAlg::PruneGlobalBufAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("OutputFile", m_ofname);
}

bool PruneGlobalBufAlg::initialize()
{
    m_ofs.open(m_ofname, std::ios::trunc);
    return true;
}

bool PruneGlobalBufAlg::execute()
{
    auto &pevt = std::any_cast<std::shared_ptr<JsonEvent> &>(*(mt_sniper_context->current_event));

    auto res = pevt->str(-1);
    m_ofs.write(res.c_str(), res.size());
    m_ofs.put('\n');

    return true;
}

bool PruneGlobalBufAlg::finalize()
{
    m_ofs.close();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
class TimeConsumeAlg : public AlgBase
{
public:
    TimeConsumeAlg(const std::string &name);
    virtual ~TimeConsumeAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override;

private:
    /*data*/
};
SNIPER_DECLARE_DLE(TimeConsumeAlg);

TimeConsumeAlg::TimeConsumeAlg(const std::string &name)
    : AlgBase(name)
{
}

bool TimeConsumeAlg::initialize()
{
    return true;
}

bool TimeConsumeAlg::execute()
{
    auto &pevt = std::any_cast<std::shared_ptr<JsonEvent> &>(*(mt_sniper_context->current_event));

    (*pevt)["result"].from(99.9);

    return true;
}

bool TimeConsumeAlg::finalize()
{
    return true;
}