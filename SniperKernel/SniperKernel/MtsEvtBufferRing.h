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

#ifndef SNIPER_MTS_EVENT_QUEUE_H
#define SNIPER_MTS_EVENT_QUEUE_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/AtomicFlagLockGuard.h"
#include <memory>

template <typename EvtType>
class MtsEvtBufferRing : public DLElement
{
public:
    struct EvtSlot
    {
        // -1:invalid, 0:ready, 1:being processed, 2:done
        long status;
        EvtSlot *next;
        std::shared_ptr<EvtType> evt;
    };

    //MtsEvtBufferRing(int capacity, int threshold);
    MtsEvtBufferRing(const std::string &name);
    virtual ~MtsEvtBufferRing();

    virtual bool initialize() override;
    virtual bool finalize() override;

    int size() { return m_size; }
    bool empty() { return m_size == 0; }
    bool ample() { return m_size > m_threshold; }
    bool full() { return m_size == m_capacity; }

    void push_back(std::shared_ptr<EvtType> pEvt);
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
};

template <typename EvtType>
MtsEvtBufferRing<EvtType>::MtsEvtBufferRing(const std::string &name)
    : DLElement(name),
      m_size(0),
      m_lock(ATOMIC_FLAG_INIT)
{
    declProp("capacity", m_capacity = 10);
    declProp("threshold", m_threshold = 4);
}

template <typename EvtType>
MtsEvtBufferRing<EvtType>::~MtsEvtBufferRing()
{
}

template <typename EvtType>
bool MtsEvtBufferRing<EvtType>::initialize()
{
    m_store = new EvtSlot[m_capacity];
    m_begin = m_store;
    m_end = m_store + m_capacity;
    for (m_cursor = m_begin; m_cursor != m_end; ++m_cursor)
    {
        m_cursor->status = -1;
        m_cursor->next = m_cursor + 1;
        m_cursor->evt = nullptr;
    }
    (--m_cursor)->next = m_begin;
    m_cursor = m_end = m_begin;

    return true;
}

template <typename EvtType>
bool MtsEvtBufferRing<EvtType>::finalize()
{
    delete[] m_store;
    return true;
}

template <typename EvtType>
void MtsEvtBufferRing<EvtType>::push_back(std::shared_ptr<EvtType> pEvt)
{
    // should be invoked only in InputTask without concurrency
    // and full() must be checked (is false) before invoking
    m_end->evt.swap(pEvt);
    m_end->status = 0;
    m_end = m_end->next;
    ++m_size;
}

template <typename EvtType>
void MtsEvtBufferRing<EvtType>::pop_front()
{
    // should be invoked only in OutputTask without concurrency
    // and front()->status must be checked (==2) before invoking
    m_begin->evt = nullptr;
    m_begin->status = -1;
    m_begin = m_begin->next;
    --m_size;
}

template <typename EvtType>
typename MtsEvtBufferRing<EvtType>::EvtSlot *MtsEvtBufferRing<EvtType>::next()
{
    EvtSlot *pcur = nullptr;
    AtomicFlagLockGuard guard(m_lock);
    if (m_cursor->status == 0)
    {
        pcur = m_cursor;
        m_cursor = m_cursor->next;
    }
    return pcur;
}

#endif