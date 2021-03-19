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

#ifndef SNIPER_PROPERTY_H
#define SNIPER_PROPERTY_H

#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/SniperException.h"

class Property
{
public:
    Property(const std::string &key);

    virtual ~Property();

    //return the property key name
    const std::string &key() { return m_key; }

    //set property value as var
    virtual bool set(const std::string &var) = 0;

    //append var to vector/map property
    virtual bool append(const std::string &var);

    //the json value of the associated variable
    virtual SniperJSON json() = 0;

    //show the value of the property
    virtual void show();

protected:
    const std::string m_key;
};

template <typename T>
class SniperProperty : public Property
{
public:
    SniperProperty(const std::string &key_, T &var_)
        : Property(key_), m_var(var_) {}

    bool set(const std::string &var)
    {
        try
        {
            m_var = SniperJSON(var).get<T>();
        }
        catch (SniperJSON::Exception &e)
        {
            throw ContextMsgException("set property '" + m_key + "' failed\n" + e.what());
        }
        return true;
    }

    SniperJSON json()
    {
        return SniperJSON().from(m_var);
    }

private:
    T &m_var;
};

template <typename T>
class SniperProperty<std::vector<T>> : public Property
{
public:
    SniperProperty(const std::string &key_, std::vector<T> &var_)
        : Property(key_), m_var(var_) {}

    bool set(const std::string &var)
    {
        m_var.clear();
        return this->append(var);
    }

    bool append(const std::string &var)
    {
        SniperJSON json(var);
        if (json.isVector())
        {
            std::vector<T> vCpp;
            try
            {
                vCpp = json.get<std::vector<T>>();
            }
            catch (SniperJSON::Exception &e)
            {
                throw ContextMsgException("set property '" + m_key + "' failed\n" + e.what());
            }
            m_var.insert(m_var.end(), vCpp.begin(), vCpp.end());
        }
        else
        {
            try
            {
                m_var.push_back(json.get<T>());
            }
            catch (SniperJSON::Exception &e)
            {
                throw ContextMsgException("set property '" + m_key + "' failed\n" + e.what());
            }
        }
        return true;
    }

    SniperJSON json()
    {
        return SniperJSON().from(m_var);
    }

private:
    std::vector<T> &m_var;
};

template <typename K, typename V>
class SniperProperty<std::map<K, V>> : public Property
{
public:
    SniperProperty(const std::string &key_, std::map<K, V> &var_)
        : Property(key_), m_var(var_) {}

    bool set(const std::string &var)
    {
        try
        {
            m_var = SniperJSON(var).get<std::map<K, V>>();
        }
        catch (SniperJSON::Exception &e)
        {
            throw ContextMsgException("set property '" + m_key + "' failed\n" + e.what());
        }
        return true;
    }

    bool append(const std::string &var)
    {
        std::map<K, V> vCpp;
        try
        {
            vCpp = SniperJSON(var).get<std::map<K, V>>();
        }
        catch (SniperJSON::Exception &e)
        {
            throw ContextMsgException("set property '" + m_key + "' failed\n" + e.what());
        }
        m_var.insert(vCpp.begin(), vCpp.end());
        return true;
    }

    SniperJSON json()
    {
        return SniperJSON().from(m_var);
    }

private:
    std::map<K, V> &m_var;
};

#endif
