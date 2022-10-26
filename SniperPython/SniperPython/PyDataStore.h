/* Copyright (C) 2018-2021
   Institute of High Energy Physics and Shandong University
   This file is part of mt.sniper.
 
   mt.sniper is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
 
   mt.sniper is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License
   along with mt.sniper.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef PY_DATA_STORE_H
#define PY_DATA_STORE_H

#include "SniperKernel/IDataBlock.h"
#include <boost/python/dict.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <vector>
#include <map>

class PyDataStore : public IDataBlock
{
public:
    PyDataStore() {}
    virtual ~PyDataStore();

    boost::python::dict &data() { return m_store; }

    void clear() { m_store.clear(); }

    //Set a cpp object to the PyDataStore
    template <typename Type>
    void set(const std::string &name, const Type &val);

    //Set a cpp vector to the PyDataStore
    template <typename Type>
    void set(const std::string &name, const std::vector<Type> &vec);

    //Set a cpp map to the PyDataStore
    template <typename Key, typename Value>
    void set(const std::string &name, const std::map<Key, Value> &map);

    //Get a cpp object from the PyDataStore
    template <typename Type>
    void get(const std::string &name, Type &val);

    //Get a cpp vector from the PyDataStore
    template <typename Type>
    void get(const std::string &name, std::vector<Type> &vec);

    //Get a cpp map from the PyDataStore
    template <typename Key, typename Value>
    void get(const std::string &name, std::map<Key, Value> &map);

private:
    boost::python::dict m_store;
};

template <typename Type>
void PyDataStore::set(const std::string &name, const Type &val)
{
    m_store[name] = val;
}

template <typename Type>
void PyDataStore::set(const std::string &name, const std::vector<Type> &vec)
{
    boost::python::list py_list;
    for (auto &it : vec)
        py_list.append(it);
    m_store[name] = py_list;
}

template <typename Key, typename Value>
void PyDataStore::set(const std::string &name, const std::map<Key, Value> &map)
{
    boost::python::dict py_dict;
    for (auto &it : map)
        py_dict[it.first] = it.second;
    m_store[name] = py_dict;
}

template <typename Type>
void PyDataStore::get(const std::string &name, Type &val)
{
    val = boost::python::extract<Type>(m_store[name]);
}

template <typename Type>
void PyDataStore::get(const std::string &name, std::vector<Type> &vec)
{
    vec.clear();
    const boost::python::object &py_list = m_store[name];
    int size = boost::python::len(py_list);
    for (int i = 0; i < size; ++i)
        vec.push_back(boost::python::extract<Type>(py_list[i]));
}

template <typename Key, typename Value>
void PyDataStore::get(const std::string &name, std::map<Key, Value> &map)
{
    map.clear();
    const boost::python::object &py_dict = m_store[name];
    boost::python::list _list = boost::python::dict(py_dict).items();
    for (int i = 0; i < boost::python::len(_list); ++i)
    {
        map.insert(std::make_pair(
            boost::python::extract<Key>(_list[i][0])(),
            boost::python::extract<Value>(_list[i][1])()));
    }
}

#endif
