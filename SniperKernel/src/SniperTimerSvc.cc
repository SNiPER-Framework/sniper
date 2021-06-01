#include "SniperKernel/SniperTimerSvc.h"
#include "SniperKernel/SniperTimer.h"

#include "SniperKernel/SvcBase.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(SniperTimerSvc);

SniperTimerSvc::SniperTimerSvc(const std::string& name)
    : SvcBase(name)
{

}

SniperTimerSvc::~SniperTimerSvc()
{
    for (auto& it : m_name2timer)
        delete it.second;
}

bool SniperTimerSvc::initialize()
{
    return true;
}

bool SniperTimerSvc::finalize()
{
    return true;
}

SniperTimer* SniperTimerSvc::get(const std::string& name)
{
    std::map<std::string, SniperTimer*>::iterator it = m_name2timer.find(name);
    if (it != m_name2timer.end()) {
        return it->second;
    }
    // create the timer if not exists
    m_name2timer[name] = new SniperTimer(name);
    return m_name2timer[name];
}
