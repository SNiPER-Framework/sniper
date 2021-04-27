/* Copyright (C) 2018-2021
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

#ifndef SNIPER_INCIDENT_H
#define SNIPER_INCIDENT_H

#include <string>

class IExecUnit;

class Incident
{
public:
    /// return the number of handlers for the incident, -1 in case of error
    static int fire(IExecUnit &domain, const std::string &key);

    Incident(const std::string &key)
        : m_key(key) {}

    virtual ~Incident() {}

    /// return the number of handlers for the incident, -1 in case of error
    virtual int fire(IExecUnit &domain);

    const std::string &name() { return m_key; }

protected:
    std::string m_key;

    //not provided
    Incident() = delete;
};

//fire a pair of incidents at the beginning and ending of a scope respectively
struct ScopedIncidentsPair
{
    ScopedIncidentsPair(Incident &begin,
                      Incident &end,
                      IExecUnit &domain)
        : m_end(end),
          m_domain(domain)
    {
        begin.fire(domain);
    }

    ~ScopedIncidentsPair()
    {
        m_end.fire(m_domain);
    }

    Incident &m_end;
    IExecUnit &m_domain;
};

//templated Incident that can ship additional data
template <typename T>
class Incident_T : public Incident
{
public:
    Incident_T(const std::string &key)
        : Incident(key) {}

    Incident_T<T> &load(const T &data);

    const T &payload() { return m_data; }

private:
    T m_data;
};

template <typename T>
Incident_T<T> &Incident_T<T>::load(const T &data)
{
    m_data = data;
    return *this;
}

#endif
