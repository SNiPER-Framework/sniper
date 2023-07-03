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

#ifndef SNIPER_MTS_SYNC_ASSISTANT_H
#define SNIPER_MTS_SYNC_ASSISTANT_H

#include <mutex>
#include <condition_variable>

class MtsSyncAssistant
{
public:
    MtsSyncAssistant() = default;
    ~MtsSyncAssistant() = default;

    bool active() { return m_active; }
    void deactive() { m_active = false; }

    bool paused() { return m_paused; }

    void pause()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_paused = true;
        m_cv.wait(lock, [this]
                  { return !m_paused; });
    }

    void notifyOne()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_paused = false;
        m_cv.notify_one();
    }

    void notifyAll()
    {
        while (m_paused)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_paused = false;
            m_cv.notify_all();
        }
    }

private:
    bool m_active{true};
    bool m_paused{false};
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif