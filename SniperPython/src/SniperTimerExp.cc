#include <boost/python.hpp>
#include "SniperKernel/SniperTimer.h"

namespace bp = boost::python;

BOOST_PYTHON_MODULE(libSniperTimer)
{
    bp::class_<SniperTimer, boost::shared_ptr<SniperTimer> >
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
