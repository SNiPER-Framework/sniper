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
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/DeclareDLE.h"
#include <random>
#include <cmath>
#include <memory>
#include <fstream>

typedef SniperJSON JsonEvent;
typedef std::map<std::string, std::any> MappedEvent;

class IFillGlobalBufSvc
{
public:
    virtual void fill(std::shared_ptr<JsonEvent> &pevt) = 0;
    virtual void stop() = 0;
};

class IGetGlobalBufSvc
{
public:
    virtual MappedEvent &get() = 0;
    virtual MappedEvent &pop() = 0;
    virtual void done() = 0;
};

////////////////////////////////////////////////////////////////////////////////
class FillGlobalBufSvc : public SvcBase, public IFillGlobalBufSvc
{
public:
    FillGlobalBufSvc(const std::string &name) : SvcBase(name) {}
    virtual ~FillGlobalBufSvc() = default;

    virtual bool initialize() override { return true; }
    virtual bool finalize() override { return true; }

    virtual void fill(std::shared_ptr<JsonEvent> &pevt) override;
    virtual void stop() override;
};
SNIPER_DECLARE_DLE(FillGlobalBufSvc);

void FillGlobalBufSvc::fill(std::shared_ptr<JsonEvent> &pevt)
{
    static auto gbptr = mt_sniper_context->global_buffer;
    //gbptr->push_back(pevt);
    static const std::string key{"event"};
    MappedEvent emap{{key, pevt}};
    gbptr->push_back(emap);
}

void FillGlobalBufSvc::stop()
{
    mt_sniper_context->global_buffer->deVigorous();
    getParent()->stop();
}

////////////////////////////////////////////////////////////////////////////////
class GetGlobalBufSvc : public SvcBase, public IGetGlobalBufSvc
{
public:
    GetGlobalBufSvc(const std::string &name) : SvcBase(name) {}
    virtual ~GetGlobalBufSvc() = default;

    virtual bool initialize() override { return true; }
    virtual bool finalize() override { return true; }

    virtual MappedEvent &get() override;
    virtual MappedEvent &pop() override;
    virtual void done() override {}
};
SNIPER_DECLARE_DLE(GetGlobalBufSvc);

MappedEvent &GetGlobalBufSvc::get()
{
    return std::any_cast<MappedEvent &>(*(mt_sniper_context->current_event));
}

MappedEvent &GetGlobalBufSvc::pop()
{
    return std::any_cast<MappedEvent &>(*(mt_sniper_context->current_event));
}

////////////////////////////////////////////////////////////////////////////////
class FillGlobalBufAlg : public AlgBase
{
public:
    FillGlobalBufAlg(const std::string &name);
    virtual ~FillGlobalBufAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override { return true; }

private:
    long m_max;
    IFillGlobalBufSvc *m_svc;
    std::mt19937_64 m_gen{std::random_device()()};
    //std::fisher_f_distribution<double> m_random{25.0 /* m */, 2.0 /* n */};
    std::normal_distribution<double> m_random{0.8, 4.0};
};
SNIPER_DECLARE_DLE(FillGlobalBufAlg);

FillGlobalBufAlg::FillGlobalBufAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("MaxEvtNum", m_max = 0x7FFFFFFFFFFFFFFF);
    m_gen.seed(35711);
}

bool FillGlobalBufAlg::initialize()
{
    m_svc = SniperPtr<IFillGlobalBufSvc>(getParent(), "FillGlobalBufSvc").data();
    return true;
}

bool FillGlobalBufAlg::execute()
{
    static std::map<std::string, long> cppEvt{{"EventID", -1}};
    static auto ievt = cppEvt.find("EventID");

    if (++(ievt->second) >= m_max) // increase the EventID by 1
    {
        m_svc->stop();
        return true;
    }

    auto pevt = std::make_shared<JsonEvent>(cppEvt);

    // generate and set the input value
    auto input = m_random(m_gen) * 100;
    while (input > 2000. || input < 1.)
    {
        input = m_random(m_gen) * 100;
    }
    (*pevt)["input"].from(input);

    m_svc->fill(pevt);

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

    IGetGlobalBufSvc *m_svc;
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
    m_svc = SniperPtr<IGetGlobalBufSvc>(getParent(), "GetGlobalBufSvc").data();
    return true;
}

bool PruneGlobalBufAlg::execute()
{
    auto &emap = m_svc->pop();
    auto &pevt = std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

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
class EventProcessedHandler : public IIncidentHandler
{
public:
    EventProcessedHandler(ExecUnit *domain, IGetGlobalBufSvc *svc)
        : IIncidentHandler(domain),
          m_svc(svc) {}
    virtual ~EventProcessedHandler() = default;

    virtual bool handle(Incident &incident) override;

    IGetGlobalBufSvc *m_svc;
};

bool EventProcessedHandler::handle(Incident &Incident)
{
    m_svc->done();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
class ITimeConsumeTool
{
public:
    virtual double numberIntegral4Sin(double x) = 0;
protected:
    const double step{0.0002};
};

////////////////////////////////////////////////////////////////////////////////
class TimeConsumeTool : public ToolBase, public ITimeConsumeTool
{
public:
    TimeConsumeTool(const std::string &name) : ToolBase(name) {}
    virtual ~TimeConsumeTool() = default;

    virtual double numberIntegral4Sin(double x) override;
};
SNIPER_DECLARE_DLE(TimeConsumeTool);

double TimeConsumeTool::numberIntegral4Sin(double x)
{
    static const double halfStep = step * 0.5;
    const double pend = x - step;

    double result = 0.;
    double cs = 0.;
    while (cs < pend)
    {
        result += (std::sin(cs) + std::sin(cs+step))*halfStep;
        cs += step;
    }
    result += (std::sin(cs) + std::sin(x)) * (x - cs) * 0.5;

    return result;
}

////////////////////////////////////////////////////////////////////////////////
class MtTimeConsumeTool : public ToolBase, public ITimeConsumeTool
{
public:
    MtTimeConsumeTool(const std::string &name) : ToolBase(name) {}
    virtual ~MtTimeConsumeTool() = default;

    virtual double numberIntegral4Sin(double x) override;
};
SNIPER_DECLARE_DLE(MtTimeConsumeTool);

double MtTimeConsumeTool::numberIntegral4Sin(double x)
{
    double result = 0.;
    return result;
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
    ITimeConsumeTool *m_tool;
    IGetGlobalBufSvc *m_svc;
    EventProcessedHandler *m_handler;
};
SNIPER_DECLARE_DLE(TimeConsumeAlg);

TimeConsumeAlg::TimeConsumeAlg(const std::string &name)
    : AlgBase(name)
{
}

bool TimeConsumeAlg::initialize()
{
    m_tool = tool<ITimeConsumeTool>("TimeConsumeTool");
    m_svc = SniperPtr<IGetGlobalBufSvc>(getParent(), "GetGlobalBufSvc").data();
    m_handler = new EventProcessedHandler(getParent(), m_svc);
    m_handler->regist("EndEvent");

    return true;
}

bool TimeConsumeAlg::execute()
{
    auto &emap = m_svc->get();

    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

    auto input = evt["input"].get<double>();
    auto result = m_tool->numberIntegral4Sin(input);

    evt["result"].from(result);

    return true;
}

bool TimeConsumeAlg::finalize()
{
    m_handler->unregist("EndEvent");
    delete m_handler;
    return true;
}