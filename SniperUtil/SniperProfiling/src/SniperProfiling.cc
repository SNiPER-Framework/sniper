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
        : IIncidentHandler(domain), h_evtTimer(evtTimer)
    {
        m_name = "BeginEvtHandler";
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
    EndEvtHandler(ExecUnit *domain, SniperTimer* evtTimer, SniperProfiling* profiling)
        : IIncidentHandler(domain),
          h_evtTimer(evtTimer),
          m_profiling(profiling)
    {
        m_name = "EndEvtHandler";
    }

    SniperTimer* h_evtTimer;
    SniperProfiling* m_profiling;

    bool handle(Incident &incident) override;
};

bool sp::EndEvtHandler::handle(Incident &incident)
{
    h_evtTimer->stop();

    LogDebug << "The event " << "tooks " << h_evtTimer->elapsed() << "ms" << std::endl;

    if (m_profiling) {
        m_profiling->dumpDetails();
    }

    return true;
}

// time for algs
struct sp::BeginAlgHandler : public IIncidentHandler
{
    BeginAlgHandler(ExecUnit *domain,  std::map<std::string, SniperTimer*>& algTimer)
        : IIncidentHandler(domain), h_timerMap(algTimer)
    {
        m_name = "BeginAlgHandler";
    }

    std::map<std::string, SniperTimer*>& h_timerMap;

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
        : IIncidentHandler(domain), h_timerMap(algTimer)
    {
        m_name = "EndAlgHandler";
    }

    std::map<std::string, SniperTimer*>& h_timerMap;

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

SniperProfiling::SniperProfiling(const std::string &name)
    : SvcBase(name),
      m_fDetails(nullptr)
{
    declProp("SaveDetails", m_saveDetails = false);
}

SniperProfiling::~SniperProfiling()
{
}

bool SniperProfiling::initialize()
{
    Task* taskPtr = dynamic_cast<Task*>(m_par);
    SniperJSON taskJson = taskPtr->json();
    const SniperJSON& algVec = taskJson["algorithms"];

    //get event timer
    m_evtTimer = new SniperTimer("evtTimer");

    //get names of algs and store them
    for (auto it = algVec.vec_begin(); it != algVec.vec_end(); it++)
    {
        std::string idtf = (*it)["identifier"].get<std::string>();
        const std::string&& algName = idtf.substr(idtf.find('/') + 1, idtf.size());
        m_algName.emplace_back(algName);
        m_algTimer[algName] = new SniperTimer(algName);
    }

    //create and regist the handler for BeginEvent
    m_beginEvtHdl = new sp::BeginEvtHandler(m_par, m_evtTimer);
    m_beginEvtHdl->regist("BeginEvent");

    //create and regist the handler for EndEvent
    m_endEvtHdl = new sp::EndEvtHandler(m_par, m_evtTimer, m_saveDetails ? this : nullptr);
    m_endEvtHdl->regist("EndEvent");

    //create and regist the handler for BeginAlg
    m_beginAlgHdl = new sp::BeginAlgHandler(m_par, m_algTimer);
    m_beginAlgHdl->regist("BeginAlg");

    //create and regist the handler for EndAlg
    m_endAlgHdl = new sp::EndAlgHandler(m_par, m_algTimer);
    m_endAlgHdl->regist("EndAlg");

    //use the same log level in EndEvent and EndAlg handlers
    m_endEvtHdl->setLogLevel(this->logLevel());
    m_endAlgHdl->setLogLevel(this->logLevel());

    LogInfo << m_description << std::endl;

    if ( m_saveDetails ) {
        m_fname = m_par->scope() + m_par->objName() + '_' + std::to_string(getpid()) + ".profiling";
        auto pos = m_fname.find(':');
        while (pos != m_fname.npos)
        {
            m_fname.replace(pos, 1, "_");
            pos = m_fname.find(':');
        }
        m_fDetails = new std::ofstream(m_fname);
        for (const auto &it : m_algName) {
            *m_fDetails << std::left << std::setw(12) << it << " ";
        }
        *m_fDetails << "Event" << std::endl;
    }

    return true;
}

bool SniperProfiling::finalize()
{
    if ( m_fDetails ) {
        m_fDetails->close();
        delete m_fDetails;
    }

    //unregist and delete the handlers
    m_beginEvtHdl->unregist("BeginEvent");
    m_endEvtHdl->unregist("EndEvent");
    m_beginAlgHdl->unregist("BeginAlg");
    m_endAlgHdl->unregist("EndAlg");

    delete m_beginEvtHdl;
    delete m_endEvtHdl;
    delete m_beginAlgHdl;
    delete m_endAlgHdl;

    //hold the lock so that other logs will not insert into the profiling messages in multi-threading context
    SniperLog::Logger::lock();

    //print statical times
    *SniperLog::LogStream << "############################## SniperProfiling ##############################\n";

    //print time of events
    *SniperLog::LogStream << std::setiosflags(std::ios::fixed|std::ios::showpoint)
                          << std::setprecision(5)
                          << std::setw(25) << std::left << "Name"
                          << std::setw(12) << "Count"
                          << std::setw(15) << "Total(ms)"
                          << std::setw(13) << "Mean(ms)"
                          << std::setw(13) << "RMS(ms)"
                          << std::endl;

    //print time of algs
    for (const auto& it : m_algName)
    {
        *SniperLog::LogStream << std::setw(25) << std::left << it
                              << std::setw(12) << (m_algTimer[it])->number_of_measurements()
                              << std::setw(15) << (m_algTimer[it])->number_of_measurements() * (m_algTimer[it])->mean()
                              << std::setw(13) << (m_algTimer[it])->mean()
                              << std::setw(13) << (m_algTimer[it])->rms()
                              << std::endl;
    }

    *SniperLog::LogStream << std::left << "Sum of " << std::setw(18) << getParent()->objName()
                          << std::setw(12) << m_evtTimer->number_of_measurements()
                          << std::setw(15) << m_evtTimer->number_of_measurements() * m_evtTimer->mean()
                          << std::setw(13) << m_evtTimer->mean()
                          << std::setw(13) << m_evtTimer->rms()
                          << std::endl << std::defaultfloat;
    if (m_saveDetails)
    {
        *SniperLog::LogStream << "The details for each event are written to file: " << m_fname << "\n";
    }
    *SniperLog::LogStream << "#############################################################################\n";

    // release the lock for logs
    SniperLog::Logger::unlock();

    delete m_evtTimer;
    for (auto& it : m_algTimer)
        delete it.second;

    LogInfo << "finalized successfully" << std::endl;
    return true;
}

const SniperTimer* SniperProfiling::getAlgTimer(const std::string& algName) const {
    if(m_algTimer.count(algName) == 0){
        return nullptr;
    }
    return m_algTimer.at(algName);
}

void SniperProfiling::dumpDetails()
{
    for (const auto &it : m_algName)
    {
        *m_fDetails << std::setw(13) << m_algTimer[it]->elapsed();
    }
    *m_fDetails << std::setw(13) << m_evtTimer->elapsed() << std::endl;
}
