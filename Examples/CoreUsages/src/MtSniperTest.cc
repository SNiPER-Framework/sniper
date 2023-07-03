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
#include "SniperKernel/MtsIncubator.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
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

class IFillResultTool
{
public:
    virtual void fill(MappedEvent &emap) = 0;
};

class IWriteResultTool
{
public:
    virtual void save(MappedEvent &emap) = 0;
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
    static const std::string key{"event"};

    MappedEvent emap{{key, pevt}};
    gbptr->push_back(emap);
}

void FillGlobalBufSvc::stop()
{
    mt_sniper_context->global_buffer->deVigorous();
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
class WriteAsciiTool : public ToolBase, public IWriteResultTool
{
public:
    WriteAsciiTool(const std::string &name);
    virtual ~WriteAsciiTool() = default;

    virtual bool initialize() override;
    virtual bool finalize() override;

    virtual void save(MappedEvent &emap) override;

private:
    std::string m_ofname;
    std::ofstream m_ofs;
};
SNIPER_DECLARE_DLE(WriteAsciiTool);

WriteAsciiTool::WriteAsciiTool(const std::string &name)
    : ToolBase(name)
{
    declProp("OutputFile", m_ofname);
}

bool WriteAsciiTool::initialize()
{
    m_ofs.open(m_ofname, std::ios::trunc);
    return true;
}

bool WriteAsciiTool::finalize()
{
    m_ofs.close();
    return true;
}

void WriteAsciiTool::save(MappedEvent &emap)
{
    auto &pevt = std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);
    auto res = pevt->str(-1);
    m_ofs.write(res.c_str(), res.size());
    m_ofs.put('\n');
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
    IWriteResultTool *m_wtool;
    IGetGlobalBufSvc *m_svc;
};
SNIPER_DECLARE_DLE(PruneGlobalBufAlg);

PruneGlobalBufAlg::PruneGlobalBufAlg(const std::string &name)
    : AlgBase(name)
{
}

bool PruneGlobalBufAlg::initialize()
{
    m_wtool = tool<IWriteResultTool>("WriteResultTool");
    dynamic_cast<ToolBase*>(m_wtool)->initialize();
    m_svc = SniperPtr<IGetGlobalBufSvc>(getParent(), "GetGlobalBufSvc").data();
    return true;
}

bool PruneGlobalBufAlg::execute()
{
    auto &emap = m_svc->pop();
    m_wtool->save(emap);

    return true;
}

bool PruneGlobalBufAlg::finalize()
{
    return dynamic_cast<ToolBase*>(m_wtool)->finalize();
}

////////////////////////////////////////////////////////////////////////////////
class ITimeConsumeTool
{
public:
    virtual double numberIntegral4Sin(double x0, double x1) = 0;
protected:
    const double step{0.0002};
};

////////////////////////////////////////////////////////////////////////////////
class TimeConsumeTool : public ToolBase, public ITimeConsumeTool
{
public:
    TimeConsumeTool(const std::string &name) : ToolBase(name) {}
    virtual ~TimeConsumeTool() = default;

    virtual double numberIntegral4Sin(double x0, double x1) override;
};
SNIPER_DECLARE_DLE(TimeConsumeTool);

double TimeConsumeTool::numberIntegral4Sin(double x0, double x1)
{
    static const double halfStep = step * 0.5;

    double result = 0.;
    double cs = 0.;
    while (cs < x1)
    {
        result += (std::sin(cs) + std::sin(cs+step))*halfStep;
        cs += step;
    }
    result += (std::sin(cs) + std::sin(x1)) * (x1 - cs) * 0.5;

    return result;
}

////////////////////////////////////////////////////////////////////////////////
class TimeConsumeTask : public MtsMicroTask
{
public:
    TimeConsumeTask() { tool = toolPool->secureAllocate(); }
    virtual ~TimeConsumeTask() { toolPool->deallocate(tool); }

    virtual Status exec() override;

    double x0;
    double x1;
    double *result;
    TimeConsumeTool *tool;

    static SniperObjPool<TimeConsumeTool>* toolPool;
};

SniperObjPool<TimeConsumeTool> *TimeConsumeTask::toolPool = nullptr;

TimeConsumeTask::Status TimeConsumeTask::exec()
{
    *result = tool->numberIntegral4Sin(x0, x1);
    return Status::OK;
}

////////////////////////////////////////////////////////////////////////////////
class MtTimeConsumeTool : public ToolBase, public ITimeConsumeTool
{
public:
    MtTimeConsumeTool(const std::string &name)
        : ToolBase(name)
    {
        if (m_n == 0)
        {
            TimeConsumeTask::toolPool =
                SniperObjPool<TimeConsumeTool>::instance([]()
                                                         { return new TimeConsumeTool("TimeConsumeTool"); });
        }
        ++m_n;
    }

    virtual ~MtTimeConsumeTool()
    {
        m_incubator.cleanup();
        if (--m_n == 0)
        {
            TimeConsumeTask::toolPool->destroy();
        }
    }

    virtual double numberIntegral4Sin(double x0, double x1) override;

private:
    MtsIncubator<TimeConsumeTask> m_incubator;
    double m_resStore[200];

    static std::atomic_int m_n;
};
SNIPER_DECLARE_DLE(MtTimeConsumeTool);

std::atomic_int MtTimeConsumeTool::m_n{0};

double MtTimeConsumeTool::numberIntegral4Sin(double x0, double x1)
{
    static const double tstep = 10.;
    int n = (x1 - x0) / tstep;
    double _x = x0;
    for (int i = 0; i < n; ++i)
    {
        auto pt = m_incubator.allocate();
        pt->x0 = _x;
        _x += tstep;
        pt->x1 = _x;
        pt->result = m_resStore + i;
    }
    {
        //the last
        auto pt = m_incubator.allocate();
        pt->x0 = _x;
        pt->x1 = x1;
        pt->result = m_resStore + n;
        ++n;
    }
    m_incubator.wait();
    double result = 0.;
    for (int i = 0; i < n; ++i)
    {
        result += m_resStore[i];
    }
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
    ITimeConsumeTool *m_calcTool;
    IFillResultTool *m_fillTool;
    IGetGlobalBufSvc *m_svc;
};
SNIPER_DECLARE_DLE(TimeConsumeAlg);

TimeConsumeAlg::TimeConsumeAlg(const std::string &name)
    : AlgBase(name)
{
}

bool TimeConsumeAlg::initialize()
{
    m_calcTool = tool<ITimeConsumeTool>("TimeConsumeTool");
    m_fillTool = tool<IFillResultTool>("FillResultTool");
    if (m_fillTool != nullptr)
    {
        dynamic_cast<ToolBase*>(m_fillTool)->initialize();
    }
    m_svc = SniperPtr<IGetGlobalBufSvc>(getParent(), "GetGlobalBufSvc").data();

    return true;
}

bool TimeConsumeAlg::execute()
{
    auto &emap = m_svc->get();

    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

    auto input = evt["input"].get<double>();
    auto result = m_calcTool->numberIntegral4Sin(0, input);

    evt["result"].from(result);

    if (m_fillTool != nullptr)
    {
        m_fillTool->fill(emap);
    }

    m_svc->done();

    return true;
}

bool TimeConsumeAlg::finalize()
{
    return true;
}