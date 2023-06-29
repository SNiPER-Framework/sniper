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

#ifndef SNIPER_MTS_EVT_BUFFER_RING_H
#define SNIPER_MTS_EVT_BUFFER_RING_H

#include "SniperKernel/AtomicFlagLockGuard.h"
#include <any>

class MtsEvtBufferRing
{
public:
    struct EvtSlot
    {
        // -1:invalid, 0:ready, 1:being processed, 2:done
        long status;
        EvtSlot *next;
        std::any evt;
    };

    MtsEvtBufferRing(int capacity, int threshold);
    virtual ~MtsEvtBufferRing();

    int size() { return m_size; }
    bool empty() { return m_size == 0; }
    bool ample() { return m_size > m_threshold; }
    bool full() { return m_size == m_capacity; }

    //template <typename EvtType>
    //void push_back(EvtType evt);
    void push_back(std::any evt);
    void pop_front();
    EvtSlot *front() { return m_begin; }
    EvtSlot *next();

private:
    EvtSlot *m_begin;
    EvtSlot *m_end;
    EvtSlot *m_cursor;

    EvtSlot *m_store;

    int m_capacity;
    int m_threshold;
    std::atomic_int m_size;
    std::atomic_flag m_lock;

    MtsEvtBufferRing() = delete;
};

//template <typename EvtType>
//void MtsEvtBufferRing::push_back(EvtType evt)
//{
//    // should be invoked only in InputTask without concurrency
//    // and full() must be checked (is false) before invoking
//    m_end->evt = evt;
//    m_end->status = 0;
//    m_end = m_end->next;
//    ++m_size;
//}

#endif