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

#ifndef SNIPER_ATOMIC_FLAG_LOCK_GUARD_H
#define SNIPER_ATOMIC_FLAG_LOCK_GUARD_H

#include <atomic>

template <bool LockInternally>
class AtomicFlagLockGuard final
{
public:
    AtomicFlagLockGuard(std::atomic_flag &src)
        : m_ref(src)
    {
        while (m_ref.test_and_set())
            continue;
    }

    ~AtomicFlagLockGuard() { m_ref.clear(); }

private:
    std::atomic_flag &m_ref;

    AtomicFlagLockGuard() = delete;
    AtomicFlagLockGuard(const AtomicFlagLockGuard &) = delete;
    AtomicFlagLockGuard &operator=(const AtomicFlagLockGuard &) = delete;
};

template </*LockExternally*/>
class AtomicFlagLockGuard<false> final
{
public:
    AtomicFlagLockGuard(std::atomic_flag &src)
        : m_ref(src)
    {
    }

    ~AtomicFlagLockGuard() { m_ref.clear(); }

private:
    std::atomic_flag &m_ref;

    AtomicFlagLockGuard() = delete;
    AtomicFlagLockGuard(const AtomicFlagLockGuard &) = delete;
    AtomicFlagLockGuard &operator=(const AtomicFlagLockGuard &) = delete;
};

#endif