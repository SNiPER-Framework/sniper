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

#include "SniperKernel/MtSniperUtility.h"
#include "SniperKernel/MtsIncubator.h"
#include "SniperKernel/MtsEvtBufferRing.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/ToolBase.h"
#include "SniperKernel/DataStore.h"
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
class SerialFillGetBufSvc : public SvcBase, public IFillGlobalBufSvc, public IGetGlobalBufSvc
{
public:
    SerialFillGetBufSvc(const std::string &name) : SvcBase(name) {}
    virtual ~SerialFillGetBufSvc() = default;

    virtual bool initialize() override { return true; }
    virtual bool finalize() override { return true; }

    virtual void fill(std::shared_ptr<JsonEvent> &pevt) override { s_evt["event"] = pevt; }
    virtual void stop() override {}
    virtual MappedEvent &get() override { return s_evt; }
    virtual MappedEvent &pop() override { return s_evt; }

private:
    static MappedEvent s_evt;
};
MappedEvent SerialFillGetBufSvc::s_evt;
typedef SerialFillGetBufSvc SerialFillBufSvc;
typedef SerialFillGetBufSvc SerialGetBufSvc;
SNIPER_DECLARE_DLE(SerialFillBufSvc);
SNIPER_DECLARE_DLE(SerialGetBufSvc);

////////////////////////////////////////////////////////////////////////////////
class FillGlobalBufSvc : public SvcBase, public IFillGlobalBufSvc
{
public:
    FillGlobalBufSvc(const std::string &name)
        : SvcBase(name)
    {
        m_gb = MtSniperUtil::GlobalBuffer::instance();
    }
    virtual ~FillGlobalBufSvc() = default;

    virtual bool initialize() override { return true; }
    virtual bool finalize() override { return true; }

    virtual void fill(std::shared_ptr<JsonEvent> &pevt) override;
    virtual void stop() override;

private:
    MtsEvtBufferRing *m_gb;
};
SNIPER_DECLARE_DLE(FillGlobalBufSvc);

void FillGlobalBufSvc::fill(std::shared_ptr<JsonEvent> &pevt)
{
    static const std::string key{"event"};

    MappedEvent emap{{key, pevt}};
    m_gb->push_back(emap);
}

void FillGlobalBufSvc::stop()
{
    m_gb->deVigorous();
}

////////////////////////////////////////////////////////////////////////////////
class GetGlobalBufSvc : public SvcBase, public IGetGlobalBufSvc
{
public:
    GetGlobalBufSvc(const std::string &name)
        : SvcBase(name)
    {
    }
    virtual ~GetGlobalBufSvc() = default;

    virtual bool initialize() override
    {
        m_ds = getData<Sniper::DataStore<std::any *>>("GBEVENT");
        return m_ds != nullptr;
    }
    virtual bool finalize() override { return true; }

    virtual MappedEvent &get() override;
    virtual MappedEvent &pop() override;

private:
    Sniper::DataStore<std::any *> *m_ds;
};
SNIPER_DECLARE_DLE(GetGlobalBufSvc);

MappedEvent &GetGlobalBufSvc::get()
{
    return std::any_cast<MappedEvent &>(*(m_ds->get()));
}

MappedEvent &GetGlobalBufSvc::pop()
{
    return std::any_cast<MappedEvent &>(*(m_ds->get()));
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
    LogDebug << res << std::endl;
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
    double input = 2200.; // set a big input for every 10 events
    if (ievt->second % 10 != 6)
    {
        input = m_random(m_gen) * 100;
        while (input > 2000. || input < 1.)
        {
            input = m_random(m_gen) * 100;
        }
    }
    (*pevt)["input"].from(input);

    m_svc->fill(pevt);

    LogDebug << "filled event: " << ievt->second << std::endl;

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
    double cs = x0;
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
    TimeConsumeTask() { tool = new TimeConsumeTool("TimeConsumeTool"); }
    virtual ~TimeConsumeTask() { delete tool; }

    virtual Status exec() override;

    double x0;
    double x1;
    double *result;
    TimeConsumeTool *tool;
};

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
        declProp("MicroStep", m_mstep);
    }

    virtual ~MtTimeConsumeTool() = default;

    virtual double numberIntegral4Sin(double x0, double x1) override;

private:
    double m_mstep{10.};
    double m_resStore[4000];
    MtsIncubator<TimeConsumeTask> m_incubator;
};
SNIPER_DECLARE_DLE(MtTimeConsumeTool);

double MtTimeConsumeTool::numberIntegral4Sin(double x0, double x1)
{
    int n = (x1 - x0) / m_mstep;
    double _x = x0;
    double *endStore = m_resStore + n;
    for (auto iStore = m_resStore; iStore < endStore; ++iStore)
    {
        // allocate and reuse a TimeConsumeTask in the pool
        auto pt = m_incubator.allocate();
        pt->x0 = _x;
        _x += m_mstep;
        pt->x1 = _x;
        pt->result = iStore;
        *iStore = 0.;
    }
    {
        //the left beyond
        auto pt = m_incubator.allocate();
        pt->x0 = _x;
        pt->x1 = x1;
        pt->result = endStore;
        *endStore = 0.;
        ++n;
    }
    LogDebug << &m_incubator << " incubator wating children: " << n << std::endl;
    m_incubator.wait();
    LogDebug << &m_incubator << " incubator come back" << std::endl;
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
    double m_timeScale;

    std::string m_input;
    std::string m_output;
};
SNIPER_DECLARE_DLE(TimeConsumeAlg);

TimeConsumeAlg::TimeConsumeAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("TimeScale", m_timeScale = 1.0);
    declProp("Input", m_input);
    declProp("Output", m_output);
}

bool TimeConsumeAlg::initialize()
{
    m_calcTool = tool<ITimeConsumeTool>("TimeConsumeTool");
    m_fillTool = tool<IFillResultTool>("FillResultTool");
    if (m_fillTool != nullptr)
    {
        dynamic_cast<ToolBase*>(m_fillTool)->initialize();
    }
    m_svc = get<IGetGlobalBufSvc>("GetGlobalBufSvc");

    return m_svc != nullptr;
}

bool TimeConsumeAlg::execute()
{
    auto &emap = m_svc->get();
    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

    auto eid = evt["EventID"].str(-1);
    LogDebug << "begin event: " << eid << std::endl;

    auto input = evt[m_input].get<double>() * m_timeScale;
    auto output = m_calcTool->numberIntegral4Sin(0, input);

    evt[m_output].from(output);

    if (m_fillTool != nullptr)
    {
        m_fillTool->fill(emap);
    }

    LogDebug << "end event: " << eid << std::endl;

    return true;
}

bool TimeConsumeAlg::finalize()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
class FanOutAlg : public AlgBase
{
public:
    FanOutAlg(const std::string &name);
    virtual ~FanOutAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override;

private:
    IFillResultTool *m_fillTool;
    IGetGlobalBufSvc *m_svc;

    std::vector<std::string> m_keys;
};
SNIPER_DECLARE_DLE(FanOutAlg);

FanOutAlg::FanOutAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("InitKeys", m_keys);
}

bool FanOutAlg::initialize()
{
    m_fillTool = tool<IFillResultTool>("FillResultTool");
    if (m_fillTool != nullptr)
    {
        dynamic_cast<ToolBase*>(m_fillTool)->initialize();
    }
    m_svc = get<IGetGlobalBufSvc>("GetGlobalBufSvc");
    return m_svc != nullptr;
}

bool FanOutAlg::execute()
{
    static double initValue = 0.0;

    auto &emap = m_svc->get();
    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

    auto eid = evt["EventID"].str(-1);
    LogDebug << "begin event: " << eid << std::endl;

    for (auto &k : m_keys)
    {
        evt[k].from(initValue);
    }

    if (m_fillTool != nullptr)
    {
        m_fillTool->fill(emap);
    }

    LogDebug << "end event: " << eid << std::endl;

    return true;
}

bool FanOutAlg::finalize()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
class FanInAlg : public AlgBase
{
public:
    FanInAlg(const std::string &name);
    virtual ~FanInAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override;

private:
    IFillResultTool *m_fillTool;
    IGetGlobalBufSvc *m_svc;

    std::string m_output;
    std::vector<std::string> m_inputs;
};
SNIPER_DECLARE_DLE(FanInAlg);

FanInAlg::FanInAlg(const std::string &name)
    : AlgBase(name)
{
    declProp("Output", m_output);
    declProp("Inputs", m_inputs);
}

bool FanInAlg::initialize()
{
    m_fillTool = tool<IFillResultTool>("FillResultTool");
    if (m_fillTool != nullptr)
    {
        dynamic_cast<ToolBase *>(m_fillTool)->initialize();
    }
    m_svc = get<IGetGlobalBufSvc>("GetGlobalBufSvc");

    return m_svc != nullptr;
}

bool FanInAlg::execute()
{
    auto &emap = m_svc->get();
    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

    auto eid = evt["EventID"].str(-1);
    LogDebug << "begin event: " << eid << std::endl;

    double output = 0.0;
    for (auto &i : m_inputs)
    {
        output += evt[i].get<double>();
    }
    evt[m_output].from(output);

    if (m_fillTool != nullptr)
    {
        m_fillTool->fill(emap);
    }

    LogDebug << "end event: " << eid << std::endl;

    return true;
}

bool FanInAlg::finalize()
{
    return true;
}
