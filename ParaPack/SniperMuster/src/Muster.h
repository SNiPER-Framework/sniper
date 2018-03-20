/* Copyright (C) 2018 Jiaheng Zou <zoujh@ihep.ac.cn>
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

#ifndef SNIPER_MUSTER_H
#define SNIPER_MUSTER_H

#include <list>

namespace boost { namespace python { namespace api { class object; } } }
class Task;
class TaskSupervisor;

class Muster
{
    public:

        Muster();
        virtual ~Muster();

        // set the maximun events number to run
        void setEvtMax(int evtMax);

        // will execute the corresponding task maximumly in nthreads
        bool config(boost::python::api::object& functor, unsigned int nthreads);

        // append a task without additional thread
        bool append(boost::python::api::object& functor);

        // spawn n threads for parallel execution
        bool run();

        const char* objName() { return "Muster"; }
 
    private:

        int             m_threads;
        TaskSupervisor* m_supervisor;

        // hold the Python instances until terminating
        std::list<boost::python::api::object> m_children;

        // following interfaces are not supported
        Muster(const Muster&) = delete;
        Muster& operator=(const Muster&) = delete;
};

#endif
