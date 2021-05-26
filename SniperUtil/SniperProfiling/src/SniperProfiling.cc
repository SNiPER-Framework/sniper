#include <unistd.h>
#include <iomanip>
#include <iostream>
#include "SniperProfiling/SniperProfiling.h"
#include "SniperKernel/SniperTimerSvc.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/AlgBase.h"

DECLARE_SERVICE(SniperProfiling);

//time for events
struct BeginEvtHandler : public IIncidentHandler
{
    BeginEvtHandler(ExecUnit *domain)
        : IIncidentHandler(domain)
    {
        m_name = "BeginEvtHandler";
    }

    bool handle(Incident &incident) override;
};

bool BeginEvtHandler::handle(Incident &incident)
{
    SniperPtr<SniperTimerSvc> timerSvc(m_domain, "SniperTimerSvc");
    auto evtTimer = timerSvc->get("evtTimer");

    //store evtTimer
    SniperPtr<SniperProfiling> profiler(m_domain, "SniperProfiling");
    profiler->storeEvtTimer(evtTimer);
    
    evtTimer->start();

    return true;
}

struct EndEvtHandler : public IIncidentHandler
{
    EndEvtHandler(ExecUnit *domain)
        : IIncidentHandler(domain)
    {
        m_name = "EndEvtHandler";
    }

    bool handle(Incident &incident) override;
};

bool EndEvtHandler::handle(Incident &incident)
{
    //get the evtTimer
    SniperPtr<SniperProfiling> profiler(m_domain, "SniperProfiling");
    auto evtTimer = profiler->getEvtTimer();
    evtTimer->stop();

    LogDebug << "The event " << "tooks " << evtTimer->elapsed() << "ms" << std::endl;

    return true;
}

// time for algs
struct BeginAlgHandler : public IIncidentHandler
{
    BeginAlgHandler(ExecUnit *domain)
        : IIncidentHandler(domain)
    {
        m_name = "BeginAlgHandler";
    }

    bool handle(Incident &incident) override;
};

bool BeginAlgHandler::handle(Incident &incident)
{
    //get the name of the algorithm
    auto iPtr = dynamic_cast<Incident_T<AlgBase*>*>(&incident);
    AlgBase* algPtr = iPtr->payload();
    const std::string& key = algPtr->objName();

    //get the algTimer
    SniperPtr<SniperTimerSvc> timerSvc(m_domain, "SniperTimerSvc");
    auto algTimer = timerSvc->get(key);

    //store algTimer in profiling
    SniperPtr<SniperProfiling> profiler(m_domain, "SniperProfiling");
    profiler->storeAlgTimer(key, algTimer);

    algTimer->start();

    return true;
}

struct EndAlgHandler : public IIncidentHandler
{
    EndAlgHandler(ExecUnit *domain)
        : IIncidentHandler(domain)
    {
        m_name = "EndAlgHandler";
    }

    bool handle(Incident &incident) override;
};

bool EndAlgHandler::handle(Incident &incident)
{
    //get the name of the algorithm
    auto iPtr = dynamic_cast<Incident_T<AlgBase*>*>(&incident);
    AlgBase* algPtr = iPtr->payload();
    const std::string& key = algPtr->objName();

    //get algTimer
    SniperPtr<SniperProfiling> profiler(m_domain, "SniperProfiling");
    auto algTimer = profiler->getAlgTimer(key);

    algTimer->stop();
    LogDebug << "The algorithm " << key << " tooks " << algTimer->elapsed() << "ms" << std::endl;

    return true;
}

bool SniperProfiling::initialize()
{
    SniperPtr<SniperTimerSvc> timerSvc(m_par, "SniperTimerSvc");

    //check the validity of SniperTimerSvc
    m_timerSvc = timerSvc.data();

    //create and regist the handler for BeginEvent
    m_beginEvtHdl = new BeginEvtHandler(m_par);
    m_beginEvtHdl->regist("BeginEvent");

    // //create and regist the handler for EndEvent
    m_endEvtHdl = new EndEvtHandler(m_par);
    m_endEvtHdl->regist("EndEvent");

    //create and regist the handler for BeginAlg
    m_beginAlgHdl = new BeginAlgHandler(m_par);
    m_beginAlgHdl->regist("BeginAlg");

    //create and regist the handler for EndAlg
    m_endAlgHdl = new EndAlgHandler(m_par);
    m_endAlgHdl->regist("EndAlg");

    LogInfo << m_description << std::endl;

    return true;
}

bool SniperProfiling::finalize()
{
    //unregist and delete the handlers
    m_beginEvtHdl->unregist("BeginEvent");
    m_endEvtHdl->unregist("EndEvent");
    m_beginAlgHdl->unregist("BeginAlg");
    m_endAlgHdl->unregist("EndAlg");

    delete m_beginEvtHdl;
    delete m_endEvtHdl;
    delete m_beginAlgHdl;
    delete m_endAlgHdl;

    //print statical times
    *SniperLog::LogStream << "########################## SniperProfiling ##########################\n";

    //print time of events
    *SniperLog::LogStream << std::setiosflags(std::ios::fixed|std::ios::showpoint)
                          << std::setprecision(5)
                          << std::setw(15) << std::left << "Name"
                          << std::setw(15) << "Num of calls" 
                          << std::setw(15) << "Total(ms)" 
                          << std::setw(15) << "Mean(ms)"
                          << std::setw(15) << "RMS(ms)"
                          << std::endl;

    //print time of algs
    for (const auto& it : m_algTimers)
    {
        *SniperLog::LogStream << std::setw(15) << std::left << it.first
                              << std::setw(15) << (it.second)->number_of_measurements()
                              << std::setw(15) << (it.second)->number_of_measurements() * (it.second)->mean()
                              << std::setw(15) << (it.second)->mean()
                              << std::setw(15) << (it.second)->rms()
                              << std::endl;
    }

    *SniperLog::LogStream << std::setw(15) << std::left << "event"
                          << std::setw(15) << m_evtTimer->number_of_measurements()
                          << std::setw(15) << m_evtTimer->number_of_measurements() * m_evtTimer->mean()
                          << std::setw(15) << m_evtTimer->mean()
                          << std::setw(15) << m_evtTimer->rms()
                          << std::endl << std::defaultfloat;
    *SniperLog::LogStream << "#####################################################################\n";
    
    LogInfo << "finalized successfully" << std::endl;
    return true;
}

bool SniperProfiling::storeAlgTimer(const std::string& algName, SniperTimer* algTimer)
{
    auto it = m_algTimers.find(algName);
    if (it == m_algTimers.end())
        m_algTimers[algName] = algTimer;

    return true;
}

SniperTimer* SniperProfiling::getAlgTimer(const std::string& algName)
{
    return m_algTimers[algName];
}

bool SniperProfiling::storeEvtTimer(SniperTimer* evtTimer)
{
    if (m_evtTimer != evtTimer)
        m_evtTimer = evtTimer;

    return true;
}

SniperTimer* SniperProfiling::getEvtTimer()
{
    return m_evtTimer;
}