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

#ifndef SNIPER_SNIPER_OBJ_POOL_H
#define SNIPER_SNIPER_OBJ_POOL_H

#include "SniperKernel/AtomicFlagLockGuard.h"

// A thread-safe pool manages reusable class objects to avoid frequently new and delete
template <typename T>
class SniperObjPool
{
public:
    static SniperObjPool<T> *instance();
    static void destroy();

    // set the creator function for the object creation
    void setCreator(T *(*creator)()) { m_creator = creator; }

    // pre-allocate n objects and put them in the pool
    void preAllocate(int n);

    // allocate one object, return nullptr if the pool is empty
    T *allocate();
    // allocate one object, create and return a new object if the pool is empty
    T *secureAllocate();

    // put one object to the pool for reallocation
    void deallocate(T *obj);

protected:
    SniperObjPool();
    virtual ~SniperObjPool();

    struct Slot
    {
        Slot(Slot *d, T *v) : up(nullptr), down(d), value(v) {}

        Slot *up;
        Slot *down;
        T *value;
    };

    Slot *m_top;
    T *(*m_creator)();
    std::atomic_flag m_lock;

    static SniperObjPool<T> *s_instance;

    // following interfaces are not supported
    SniperObjPool(const SniperObjPool &) = delete;
    SniperObjPool &operator=(const SniperObjPool &) = delete;
};

template <typename T>
SniperObjPool<T> *SniperObjPool<T>::s_instance = nullptr;

template <typename T>
SniperObjPool<T> *SniperObjPool<T>::instance()
{
    // suppose this function is firstly invoked at the beginning of the main thread
    // before any children threads, so mutex lock is unnecessary
    if (s_instance == nullptr)
    {
        s_instance = new SniperObjPool<T>();
    }
    return s_instance;
}

template <typename T>
void SniperObjPool<T>::destroy()
{
    // suppose this function is finally invoked at the end of the main thread
    // so mutex lock is unnecessary
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

template <typename T>
SniperObjPool<T>::SniperObjPool()
    : m_creator(nullptr),
      m_lock(ATOMIC_FLAG_INIT)
{
    m_top = new Slot(nullptr, nullptr);
    m_top->down = m_top;
}

template <typename T>
SniperObjPool<T>::~SniperObjPool()
{
    // delete the objects in the pool
    while (m_top->down != m_top)
    {
        delete m_top->value;
        m_top = m_top->down;
    }

    // delete the Slots
    while (m_top->up != nullptr)
    {
        m_top = m_top->up;
        delete m_top->down;
    }
    delete m_top;
}

template <typename T>
void SniperObjPool<T>::preAllocate(int n)
{
    for (int i = 0; i < n; ++i)
    {
        T *obj = m_creator != nullptr ? (*m_creator)() : new T();
        deallocate(obj);
    }
}

template <typename T>
T *SniperObjPool<T>::allocate()
{
    AtomicFlagLockGuard guard(m_lock);
    T *obj = m_top->value;
    m_top = m_top->down;
    return obj;
}

template <typename T>
T *SniperObjPool<T>::secureAllocate()
{
    T *obj = allocate();
    if (obj == nullptr)
    {
        obj = m_creator == nullptr ? new T() : (*m_creator)();
    }
    return obj;
}

template <typename T>
void SniperObjPool<T>::deallocate(T *obj)
{
    AtomicFlagLockGuard guard(m_lock);
    if (m_top->up != nullptr)
    {
        m_top = m_top->up;
        m_top->value = obj;
    }
    else
    {
        m_top->up = new Slot(m_top, obj);
        m_top = m_top->up;
    }
}

#endif