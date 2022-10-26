#include "SniperKernel/SniperTimer.h"
#include <boost/python/class.hpp>
#include <boost/python/copy_const_reference.hpp>

namespace bp = boost::python;

void export_Sniper_Timer()
{
    bp::class_<SniperTimer, std::shared_ptr<SniperTimer> >
        ("SniperTimer", bp::init<std::string>())
        .def("start", &SniperTimer::start)
        .def("stop", &SniperTimer::stop)
        .def("pause", &SniperTimer::pause)
        .def("resume", &SniperTimer::resume)
        .def("reset", &SniperTimer::reset)
        .def("name", &SniperTimer::name,
                bp::return_value_policy<bp::copy_const_reference>())
        .def("elapsed", &SniperTimer::elapsed)
        .def("mean", &SniperTimer::mean)
        .def("rms", &SniperTimer::rms)
        .def("number_of_measurements", &SniperTimer::number_of_measurements)
    ;
}
