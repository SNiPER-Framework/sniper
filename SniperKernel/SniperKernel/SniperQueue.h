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

#ifndef SNIPER_SNIPER_QUEUE_H
#define SNIPER_SNIPER_QUEUE_H

#include "SniperKernel/SniperObjPool.h"
#include "SniperKernel/AtomicFlagLockGuard.h"

namespace Sniper
{
    // A simple FIFO (without random accessing) queue that
    // 1. store the value copy inside
    // 2. has thread-safe concurrent interfaces
    template <typename T>
    class Queue
    {
    public:
        Queue(const T& guarantee);
        virtual ~Queue();

        T &front() { return m_head->value; }

        // thread-safe interfaces
        T concurrentPop();
        void concurrentPush(const T &value);

        // thread-unsafe but faster interfaces
        T pop();
        void push(const T &value);

        struct QueueSlot
        {
            T value;
            QueueSlot *next;
        };
        typedef SniperObjPool<QueueSlot> SlotPool;

    protected:
        QueueSlot *m_head;
        QueueSlot *m_last;
        QueueSlot *m_tail;
        std::atomic_flag m_lock;

        static SlotPool *s_slotPool;
    };

    template <typename T>
    typename Queue<T>::SlotPool *Queue<T>::s_slotPool = Queue<T>::SlotPool::instance([]()
                                                                                     { return new QueueSlot(); });

    template <typename T>
    Queue<T>::Queue(const T &guarantee)
        : m_last(nullptr),
          m_lock(ATOMIC_FLAG_INIT)
    {
        m_tail = s_slotPool->secureAllocate();
        m_tail->value = guarantee;
        m_tail->next = m_tail;
        m_head = m_tail;
    }

    template <typename T>
    Queue<T>::~Queue()
    {
        while (m_head != m_tail)
        {
            auto tmp = m_head;
            m_head = m_head->next;
            s_slotPool->deallocate(tmp);
        }
        s_slotPool->deallocate(m_tail);
    }

    template <typename T>
    T Queue<T>::concurrentPop()
    {
        while (m_lock.test_and_set())
            continue;
        T v = m_head->value;
        if (m_head != m_tail)
        {
            auto tmp = m_head;
            m_head = m_head->next;
            m_lock.clear();
            s_slotPool->deallocate(tmp);
        }
        else
        {
            m_lock.clear();
        }
        return v;
    }

    template <typename T>
    void Queue<T>::concurrentPush(const T &value)
    {
        auto last = s_slotPool->secureAllocate();
        last->value = value;
        last->next = m_tail;
        AtomicFlagLockGuard<true> guard(m_lock);
        if (m_head != m_tail)
        {
            m_last->next = last;
        }
        else
        {
            m_head = last;
        }
        m_last = last;
    }

    template <typename T>
    T Queue<T>::pop()
    {
        T v = m_head->value;
        if (m_head != m_tail)
        {
            auto tmp = m_head;
            m_head = m_head->next;
            s_slotPool->deallocate(tmp);
        }
        return v;
    }

    template <typename T>
    void Queue<T>::push(const T &value)
    {
        auto last = s_slotPool->secureAllocate();
        last->value = value;
        last->next = m_tail;
        if (m_head != m_tail)
        {
            m_last->next = last;
        }
        else
        {
            m_head = last;
        }
        m_last = last;
    }
} // namespace Sniper

#endif