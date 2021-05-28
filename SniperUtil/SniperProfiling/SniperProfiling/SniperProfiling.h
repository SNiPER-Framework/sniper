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

private:
    SniperTimer* m_evtTimer;
    std::vector<std::string> m_algName;
    std::map<std::string, SniperTimer*> m_algTimer;

    sp::BeginEvtHandler* m_beginEvtHdl;
    sp::EndEvtHandler*   m_endEvtHdl;
    sp::BeginAlgHandler* m_beginAlgHdl;
    sp::EndAlgHandler*   m_endAlgHdl;
};

#endif