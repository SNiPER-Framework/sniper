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

#include "SniperKernel/MtsEvtBufferRing.h"

MtsEvtBufferRing::MtsEvtBufferRing(int capacity, int threshold)
    : m_capacity(capacity),
      m_threshold(threshold)
{
    m_store = new EvtSlot[m_capacity];
    m_begin = m_store;
    m_end = m_store + m_capacity;
    for (m_cursor = m_begin; m_cursor != m_end; ++m_cursor)
    {
        m_cursor->status = SlotStatus::Invalid;
        m_cursor->next = m_cursor + 1;
    }
    (--m_cursor)->next = m_begin;
    m_cursor = m_end = m_begin;
}

MtsEvtBufferRing::~MtsEvtBufferRing()
{
    delete[] m_store;
}

void MtsEvtBufferRing::pop_front()
{
    // should be invoked only in OutputTask without concurrency
    // and front()->status must be checked (==SlotStatus::Done) before invoking
    m_begin->evt.reset();
    m_begin->status = SlotStatus::Invalid;
    m_begin = m_begin->next;
    --m_size;
}

MtsEvtBufferRing::EvtSlot *MtsEvtBufferRing::next()
{
    EvtSlot *pcur = nullptr;
    AtomicFlagLockGuard guard(m_lock);
    if (m_cursor->status == SlotStatus::Ready)
    {
        pcur = m_cursor;
        pcur->status = SlotStatus::BeingProcessed;
        m_cursor = m_cursor->next;
    }
    return pcur;
}
