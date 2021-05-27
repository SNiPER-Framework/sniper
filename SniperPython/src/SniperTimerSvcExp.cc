#include <boost/python.hpp>
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/SniperTimer.h"
#include "SniperKernel/SniperTimerSvc.h"

namespace bp = boost::python;

void export_Sniper_TimerSvc()
{
    bp::class_<SniperTimerSvc, boost::shared_ptr<SniperTimerSvc>, bp::bases<SvcBase>, boost::noncopyable>
        ("SniperTimerSvc", bp::init<const std::string&>())
        .def("get", &SniperTimerSvc::get,
                bp::return_value_policy<bp::reference_existing_object>())
    ;
}
