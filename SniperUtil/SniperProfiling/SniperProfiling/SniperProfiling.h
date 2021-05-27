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

    bool storeAlgTimer(const std::string& algName, SniperTimer* algTimer);
    SniperTimer* getAlgTimer(const std::string& algName);

    bool storeEvtTimer(SniperTimer* evtTimer);
    SniperTimer* getEvtTimer();

private:

    SniperTimerSvc* m_timerSvc;
    sp::BeginEvtHandler* m_beginEvtHdl;
    sp::EndEvtHandler*   m_endEvtHdl;
    sp::BeginAlgHandler* m_beginAlgHdl;
    sp::EndAlgHandler*   m_endAlgHdl;
    
    SniperTimer* m_evtTimer;
    std::map<std::string, SniperTimer*> m_algTimers;
};

#endif