#include <unistd.h>
#include <iomanip>
#include <iostream>
#include "SniperProfiling/SniperProfiling.h"
#include "SniperKernel/SvcFactory.h"
#include "SniperKernel/Incident.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/Task.h"
#include "SniperKernel/SniperJSON.h"

DECLARE_SERVICE(SniperProfiling);

namespace sp = SnierProfilingNS;

//time for events
struct sp::BeginEvtHandler : public IIncidentHandler
{
    BeginEvtHandler(ExecUnit *domain, SniperTimer* evtTimer)
        : IIncidentHandler(domain)
    {
        m_name = "BeginEvtHandler";
        h_evtTimer = evtTimer;
    }

    SniperTimer* h_evtTimer;

    bool handle(Incident &incident) override;
};

bool sp::BeginEvtHandler::handle(Incident &incident)
{   
    h_evtTimer->start();

    return true;
}

struct sp::EndEvtHandler : public IIncidentHandler
{
    EndEvtHandler(ExecUnit *domain, SniperTimer* evtTimer)
        : IIncidentHandler(domain)
    {
        m_name = "EndEvtHandler";
        h_evtTimer = evtTimer;
    }

    SniperTimer* h_evtTimer;

    bool handle(Incident &incident) override;
};

bool sp::EndEvtHandler::handle(Incident &incident)
{
    h_evtTimer->stop();

    LogDebug << "The event " << "tooks " << h_evtTimer->elapsed() << "ms" << std::endl;

    return true;
}

// time for algs
struct sp::BeginAlgHandler : public IIncidentHandler
{
    BeginAlgHandler(ExecUnit *domain,  std::map<std::string, SniperTimer*>& algTimer)
        : IIncidentHandler(domain)
    {
        m_name = "BeginAlgHandler";
        h_timerMap = algTimer;
    }

    std::map<std::string, SniperTimer*> h_timerMap;

    bool handle(Incident &incident) override;
};

bool sp::BeginAlgHandler::handle(Incident &incident)
{
    //get the name of the algorithm
    auto iPtr = dynamic_cast<Incident_T<AlgBase*>*>(&incident);
    AlgBase* algPtr = iPtr->payload();
    const std::string& key = algPtr->objName();

    h_timerMap[key]->start();

    return true;
}

struct sp::EndAlgHandler : public IIncidentHandler
{
    EndAlgHandler(ExecUnit *domain, std::map<std::string, SniperTimer*>& algTimer)
        : IIncidentHandler(domain)
    {
        m_name = "EndAlgHandler";
        h_timerMap = algTimer;
    }

    std::map<std::string, SniperTimer*> h_timerMap;

    bool handle(Incident &incident) override;
};

bool sp::EndAlgHandler::handle(Incident &incident)
{
    //get the name of the algorithm
    auto iPtr = dynamic_cast<Incident_T<AlgBase*>*>(&incident);
    AlgBase* algPtr = iPtr->payload();
    const std::string& key = algPtr->objName();

    //get algTimer
    h_timerMap[key]->stop();
    LogDebug << "The algorithm " << key << " tooks " << h_timerMap[key]->elapsed() << "ms" << std::endl;

    return true;
}

bool SniperProfiling::initialize()
{
    Task* taskPtr = dynamic_cast<Task*>(m_par);
    const SniperJSON& taskJson = taskPtr->json();
    const SniperJSON& algVec = taskJson["algorithms"];

    //get event timer
    m_evtTimer = new SniperTimer("evtTimer");

    //get names of algs and store them
    for (auto it = algVec.vec_begin(); it != algVec.vec_end(); it++)
    {
        const std::string& idtf = (*it)["identifier"].get<std::string>();
        const std::string&& algName = idtf.substr(idtf.find('/') + 1, idtf.size());
        m_algName.emplace_back(algName);
        m_algTimer[algName] = new SniperTimer(algName);
    }

    //create and regist the handler for BeginEvent
    m_beginEvtHdl = new sp::BeginEvtHandler(m_par, m_evtTimer);
    m_beginEvtHdl->regist("BeginEvent");

    // //create and regist the handler for EndEvent
    m_endEvtHdl = new sp::EndEvtHandler(m_par, m_evtTimer);
    m_endEvtHdl->regist("EndEvent");

    //create and regist the handler for BeginAlg
    m_beginAlgHdl = new sp::BeginAlgHandler(m_par, m_algTimer);
    m_beginAlgHdl->regist("BeginAlg");

    //create and regist the handler for EndAlg
    m_endAlgHdl = new sp::EndAlgHandler(m_par, m_algTimer);
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
    for (const auto& it : m_algName)
    {
        *SniperLog::LogStream << std::setw(15) << std::left << it
                              << std::setw(15) << (m_algTimer[it])->number_of_measurements()
                              << std::setw(15) << (m_algTimer[it])->number_of_measurements() * (m_algTimer[it])->mean()
                              << std::setw(15) << (m_algTimer[it])->mean()
                              << std::setw(15) << (m_algTimer[it])->rms()
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