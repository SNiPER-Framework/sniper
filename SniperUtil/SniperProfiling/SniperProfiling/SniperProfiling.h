#ifndef SNIPER_PROFILING_H
#define SNIPER_PROFILING_H

#include "SniperKernel/SvcBase.h"
#include "SniperKernel/SniperTimerSvc.h"
#include <vector>
#include <map>
#include <string>

struct BeginEvtHandler;
struct EndEvtHandler;
struct BeginAlgHandler;
struct EndAlgHandler;

class SniperProfiling : public SvcBase
{
public:
    SniperProfiling(const std::string& name) : SvcBase(name) {}
    ~SniperProfiling() {}

    bool initialize();
    bool finalize();

    bool storeAlgTimer(const std::string& algName, SniperTimer* algTimer);
    SniperTimer* getAlgTimer(const std::string& algName);

    bool storeEvtTimer(SniperTimer* evtTimer);
    SniperTimer* getEvtTimer();

private:

    SniperTimerSvc* m_timerSvc;
    BeginEvtHandler* m_beginEvtHdl;
    EndEvtHandler*   m_endEvtHdl;
    BeginAlgHandler* m_beginAlgHdl;
    EndAlgHandler*   m_endAlgHdl;
    
    SniperTimer* m_evtTimer;
    std::map<std::string, SniperTimer*> m_algTimers;
};

#endif