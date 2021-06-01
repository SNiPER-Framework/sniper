#ifndef SniperTimerSvc_h
#define SniperTimerSvc_h

#include "SniperKernel/SvcBase.h"
#include "SniperKernel/SniperTimer.h"
#include <map>

class SniperTimerSvc: public SvcBase
{
    public:
        SniperTimerSvc(const std::string& name);
        ~SniperTimerSvc();
    public:
        bool initialize();
        bool finalize();

    public:
        SniperTimer* get(const std::string& name);

    private:
        std::map<std::string, SniperTimer*> m_name2timer;
};

#endif
