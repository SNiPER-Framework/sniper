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

#ifndef SNIPER_SNIPER_JSON_H
#define SNIPER_SNIPER_JSON_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <typeinfo>
#include <exception>

class SniperJSON
{
public:
    typedef std::vector<SniperJSON>::const_iterator vec_iterator;
    typedef std::map<std::string, SniperJSON>::const_iterator map_iterator;

    // default constructor
    SniperJSON();

    // construct from a json string
    SniperJSON(const std::string &jstr);

    // whether this is a valid json
    bool valid() const { return m_type > 0; }

    // whether this is a scalar or string
    bool isScalar() const { return m_type > 2; }

    // whether this is a vector
    bool isVector() const { return m_type == 2; }

    // whether this is a map
    bool isMap() const { return m_type == 1; }

    /* get a C++ object from this element, T can be
     * type[1]: C++ scalar and std::string
     * type[2]: std::vector< type[1,2,3] >
     * type[3]: std::map< type[1], type[1,2,3] >
     */
    template <typename T>
    T get() const;

    // set this element from a C++ object
    template <typename T>
    void set(const T &var);

    // construct from a cpp variable
    template <typename T>
    SniperJSON &from(const T &var);

    // push_back an element in case of a vector
    bool push_back(const SniperJSON &var);

    // insert a key/value pair in case of a map
    bool insert(const std::string &key, const SniperJSON &val);

    // erase one element in a map
    map_iterator erase(map_iterator it) { return m_jmap.erase(it); }

    // erase the element by key in a map
    int erase(const std::string &key) { return m_jmap.erase(key); }

    // clear all the contents of this element
    void reset();

    // get the array/vector or object/map size
    int size() const;

    // get the vector begin iterator
    vec_iterator vec_begin() const { return m_jvec.cbegin(); }
    // get the vector end iterator
    vec_iterator vec_end() const { return m_jvec.cend(); }

    // get the object in vector via index
    SniperJSON &operator[](int index) { return m_jvec.at(index); }
    // get the object in vector via index
    const SniperJSON &operator[](int index) const { return m_jvec.at(index); }

    // get the map begin iterator
    map_iterator map_begin() const { return m_jmap.cbegin(); }
    // get the map end iterator
    map_iterator map_end() const { return m_jmap.cend(); }
    // find the iterator via key (note that the key has double quotes like "\"key\"")
    map_iterator find(const std::string &key) const { return m_jmap.find(key); }

    // get the object in map via key
    SniperJSON &operator[](const std::string &key);
    // get the object in map via key
    const SniperJSON &operator[](const std::string &key) const;

    // set the string format flag
    SniperJSON &format(bool flag);

    // convert the json to a string
    std::string str(int indent = 4, unsigned flags = 0) const;

    // load json from an input stream
    static SniperJSON load(std::istream &is);

    // load json from a string
    static SniperJSON loads(const std::string &jstr);

    // dump the json as a string to an output stream
    static void dump(const SniperJSON &element, std::ostream &os, int indent = 4, unsigned flags = 0);

    // dump the json as a string
    static std::string dumps(const SniperJSON &element, int indent = 4, unsigned flags = 0);

    // get a friendly type string from a typeid
    static std::string typestr(const std::type_info &tid);
   
   // an inner class for any exception while json parsing
    class Exception : public std::exception
    {
    public:
        Exception(const std::string &msg);
        Exception(const std::string &jstr, int cursor);
        virtual ~Exception() throw();
        const char *what() const throw();

    private:
        std::string m_msg;
    };

private:
    typedef std::string::size_type StrCursor;

    // str() format -> true: in multi lines, false: in one line
    bool m_format;

    // 0:invalid, 1:object/map, 2:array/vector, 3:string, 9:other scalars
    int m_type;
    // scalar or std::string element holder
    std::string m_jvar;
    // array/vector element holder
    std::vector<SniperJSON> m_jvec;
    // object/map element holder
    std::map<std::string, SniperJSON> m_jmap;

    // construct a sub-element from a json string
    SniperJSON(const std::string &jstr, StrCursor &cursor);

    // initialize the json element from a string
    void init(const std::string &jstr, StrCursor &cursor);

    // get next valid charactor
    char getValidChar(const std::string &jstr, StrCursor &cursor);

    // read the content of a json object/map to m_jmap;
    void readObjectMap(const std::string &jstr, StrCursor &cursor);

    // read the content of a json array/vector to m_jvec
    void readArrayVec(const std::string &jstr, StrCursor &cursor);

    // read the content of a json string to m_jvar
    void readStringStr(const std::string &jstr, StrCursor &cursor);

    // read the content of other scalars to m_jvar
    void readScalarStr(const std::string &jstr, StrCursor &cursor);

    // function template helps to access type[1]
    template <typename T>
    inline void toCppVar(T &var) const;

    // function template helps to access type[2]
    template <typename T>
    inline void toCppVar(std::vector<T> &var) const;

    // function template helps to access type[3]
    template <typename K, typename V>
    inline void toCppVar(std::map<K, V> &var) const;

    // function template helps to make json from type[1]
    template <typename T>
    inline void fromCppVar(const T &var);

    // helps to make json from a char*
    inline void fromCppVar(const char *var);

    // function template helps to make json from type[2]
    template <typename T>
    inline void fromCppVar(const std::vector<T> &var);

    // function template helps to make json from type[3]
    template <typename K, typename V>
    inline void fromCppVar(const std::map<K, V> &var);
};

template <typename T>
T SniperJSON::get() const
{
    T v;
    toCppVar(v);
    return v;
}

template <typename T>
void SniperJSON::set(const T &var)
{
    reset();
    fromCppVar(var);
}

template <typename T>
SniperJSON &SniperJSON::from(const T &var)
{
    reset();
    fromCppVar(var);
    return *this;
}

template <typename T>
inline void SniperJSON::toCppVar(T &var) const
{
    if (m_type == 9)
    {
        std::stringstream ss;
        ss << m_jvar;
        ss >> var;

        if (!ss.fail())
        {
            return;
        }
    }

    throw Exception(
        std::string("cannot set <") + typestr(typeid(T)) + "> with <" + m_jvar + '>');
}

template <>
inline void SniperJSON::toCppVar<bool>(bool &var) const
{
    if (m_type == 9)
    {
        if (m_jvar == "true")
        {
            var = true;
            return;
        }
        else if (m_jvar == "false")
        {
            var = false;
            return;
        }
    }

    throw Exception(std::string("cannot set <bool> with <") + m_jvar + '>');
}

template <>
inline void SniperJSON::toCppVar<std::string>(std::string &var) const
{
    if (m_type == 3)
    {
        var = m_jvar.substr(1, m_jvar.size() - 2);
        return;
    }

    throw Exception(std::string("cannot set <std::string> with <") + m_jvar + '>');
}

template <typename T>
inline void SniperJSON::toCppVar(std::vector<T> &var) const
{
    if (m_type == 2)
    {
        for (const auto &it : m_jvec)
        {
            var.push_back(it.get<T>());
        }
        return;
    }

    throw Exception(std::string("not a valid vector\n") + this->str());
}

template <typename K, typename V>
inline void SniperJSON::toCppVar(std::map<K, V> &var) const
{
    if (m_type == 1)
    {
        for (const auto &it : m_jmap)
        {
            var.insert(std::make_pair(
                SniperJSON(
                    std::is_same<K, std::string>::value ? it.first : it.first.substr(1, it.first.size() - 2))
                    .get<K>(),
                it.second.get<V>()));
        }
        return;
    }

    throw Exception(std::string("not a valid map\n") + this->str());
}

template <typename T>
inline void SniperJSON::fromCppVar(const T &var)
{
    std::stringstream ss;
    ss << var;
    m_jvar = ss.str();
    m_type = 9;
}

template <>
inline void SniperJSON::fromCppVar<bool>(const bool &var)
{
    m_jvar = var ? "true" : "false";
    m_type = 9;
}

template <>
inline void SniperJSON::fromCppVar<std::string>(const std::string &var)
{
    m_jvar = '"' + var + '"';
    m_type = 3;
}

inline void SniperJSON::fromCppVar(const char *var)
{
    m_jvar = '"' + std::string(var) + '"';
    m_type = 3;
}

template <typename T>
inline void SniperJSON::fromCppVar(const std::vector<T> &var)
{
    for (const auto &it : var)
    {
        m_jvec.emplace_back(SniperJSON().from(it));
    }
    m_type = 2;
}

template <typename K, typename V>
inline void SniperJSON::fromCppVar(const std::map<K, V> &var)
{
    for (const auto &it : var)
    {
        std::stringstream key;
        key << '"' << it.first << '"';
        m_jmap.insert(std::make_pair(key.str(), SniperJSON().from(it.second)));
    }
    m_type = 1;
}

#endif
