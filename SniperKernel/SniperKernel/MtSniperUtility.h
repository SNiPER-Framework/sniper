/* Copyright (C) 2023
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

#ifndef SNIPER_MT_SNIPER_UTILITY_H
#define SNIPER_MT_SNIPER_UTILITY_H

#include <any>
#include <setjmp.h>

class MtsEvtBufferRing;

namespace MtSniperUtil
{
    extern bool MtSniperStatus;

    // prepare for the essential environment of MtSniper
    void contextInit();
    // release the resources of MtSniper
    void contextRelease();

    namespace Thread
    {
        // block current thread and wait for a signal
        void pause();

        // notify one blocked thread if there is any
        void resumeOne();

        // notify all blocked threads if there is any
        void resumeAll();

    } // namespace Thread

    namespace Worker
    {
        // create n Workers and start the same number of threads
        void spawn(int n);

        // wait for the ending of all the Workers
        void waitAll();

        // wake up another Worker in the pool
        void raiseAnother();

        // expose the Incubator internal context so that it can be accessed by a Worker
        void setIncubatorContext(jmp_buf &ctx);

        // get the Incubator context and then we can resume it
        jmp_buf &getIncubatorContext();

    } // namespace Worker

    namespace GlobalBuffer
    {
        // get the GlobalBuffer instance
        MtsEvtBufferRing *instance();

    } // namespace GlobalBuffer

} // namespace MtSniper

#endif