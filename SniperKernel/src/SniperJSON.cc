/* Copyright (C) 2021
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

#include "SniperKernel/SniperJSON.h"

const std::string SniperJSON::SPACES(" \n\t\r");
const std::string SniperJSON::DELIMITS(", \n]}\t\r");

SniperJSON::SniperJSON()
    : m_format(true),
      m_type(0)
{
}

SniperJSON::SniperJSON(const std::string &jstr)
    : m_format(true),
      m_type(0)
{
    StrCursor cursor = 0;
    init(jstr, cursor);

    cursor = jstr.find_first_not_of(SniperJSON::SPACES, cursor);
    if (cursor != std::string::npos)
    {
        throw Exception(jstr, cursor);
    }
}

SniperJSON::SniperJSON(const std::string &jstr, StrCursor &cursor)
    : m_format(true),
      m_type(0)
{
    init(jstr, cursor);
}

bool SniperJSON::push_back(const SniperJSON &var)
{
    if (m_type == 2 || m_type == 0)
    {
        m_jvec.emplace_back(var);
        m_type = 2;
        return true;
    }
    return false;
}

bool SniperJSON::insert(const std::string &key, const SniperJSON &val)
{
    if (m_type == 1 || m_type == 0)
    {
        std::string _key = '"' + key + '"';
        m_jmap.insert(std::make_pair(_key, val));
        m_type = 1;
        return true;
    }
    return false;
}

void SniperJSON::reset()
{
    m_type = 0;
    m_jvar.clear();
    m_jvec.clear();
    m_jmap.clear();
}

int SniperJSON::size() const
{
    if (m_type == 1)
    {
        return m_jmap.size();
    }
    else if (m_type == 2)
    {
        return m_jvec.size();
    }

    return -1;
}

SniperJSON &SniperJSON::operator[](const std::string &key)
{
    m_type = 1;
    return m_jmap['"' + key + '"'];
}

const SniperJSON &SniperJSON::operator[](const std::string &key) const
{
    return m_jmap.at('"' + key + '"');
}

SniperJSON &SniperJSON::format(bool flag)
{
    m_format = flag;
    return *this;
}

std::string SniperJSON::str(int indent, unsigned flags) const
{
    static const unsigned int levelMask = 0xFF;
    static const unsigned int mapValueBit = 0x200;

    if (!m_format)
    {
        indent = -9;
    }

    unsigned level = flags & levelMask;
    bool isMapValue = flags & mapValueBit;
    std::string prefix = (indent < 0) ? "" : std::string().assign(indent * level, ' ');
    std::string firstPrefix = isMapValue ? "" : prefix;

    std::ostringstream oss;

    if (isScalar())
    {
        oss << firstPrefix << m_jvar;
    }
    else if (isVector())
    {
        oss << firstPrefix << "[";
        if (!m_jvec.empty())
        {
            char linefeed = (indent < 0) ? ' ' : '\n';
            unsigned _level = level + 1;
            auto it = vec_begin();
            oss << linefeed << it->str(indent, _level);
            while (++it != vec_end())
            {
                oss << "," << linefeed << it->str(indent, _level);
            }
            oss << linefeed << prefix;
        }
        oss << "]";
    }
    else if (isMap())
    {
        oss << firstPrefix << "{";
        if (!m_jmap.empty())
        {
            char linefeed = (indent < 0) ? ' ' : '\n';
            std::string fixedPrefix = (indent < 0) ? "" : std::string().assign(indent * level + indent, ' ');
            unsigned _level = (level + 1) | mapValueBit;
            auto it = map_begin();
            oss << linefeed << fixedPrefix << it->first
                << ": " << it->second.str(indent, _level);
            while (++it != map_end())
            {
                oss << "," << linefeed << fixedPrefix << it->first
                    << ": " << it->second.str(indent, _level);
            }
            oss << linefeed << prefix;
        }
        oss << "}";
    }

    return oss.str();
}

SniperJSON SniperJSON::load(std::istream &is)
{
    std::ostringstream oss;
    oss << is.rdbuf();

    return loads(oss.str());
}

SniperJSON SniperJSON::loads(const std::string &jstr)
{
    return SniperJSON(jstr);
}

void SniperJSON::dump(const SniperJSON &element, std::ostream &os, int indent, unsigned flags)
{
    os << element.str(indent, flags);
}

std::string SniperJSON::dumps(const SniperJSON &element, int indent, unsigned flags)
{
    return element.str(indent, flags);
}

void SniperJSON::init(const std::string &jstr, StrCursor &cursor)
{
    switch (getValidChar(jstr, cursor))
    {
    case '{': //object
        readObjectMap(jstr, cursor);
        m_type = 1;
        break;
    case '[': //array
        readArrayVec(jstr, cursor);
        m_type = 2;
        break;
    case '"': //string
        readStringStr(jstr, cursor);
        m_type = 3;
        break;
    default: //number, true, false or null
        readScalarStr(jstr, cursor);
        m_type = 9;
    }
}

char SniperJSON::getValidChar(const std::string &jstr, StrCursor &cursor)
{
    cursor = jstr.find_first_not_of(SPACES, cursor);
    if (cursor != std::string::npos)
    {
        return jstr.at(cursor);
    }
    throw Exception(jstr, cursor);
}

void SniperJSON::readObjectMap(const std::string &jstr, StrCursor &cursor)
{
    bool status = true;

    if (getValidChar(jstr, ++cursor) != '}')
    {
        --cursor; //reset the cursor just before the first key
        do
        {
            readStringStr(jstr, ++cursor);
            const std::string &key = m_jvar;
            if (key.size() == 2 || getValidChar(jstr, cursor) != ':')
            {
                status = false;
                break;
            }
            m_jmap.insert(std::make_pair(key, SniperJSON(jstr, ++cursor)));
        } while (getValidChar(jstr, cursor) == ',');
    }

    m_jvar.clear();

    if (status && getValidChar(jstr, cursor) == '}')
    {
        ++cursor;
        return;
    }

    throw Exception(jstr, cursor);
}

void SniperJSON::readArrayVec(const std::string &jstr, StrCursor &cursor)
{
    if (getValidChar(jstr, ++cursor) != ']')
    {
        --cursor; //reset the cursor just before the first variable
        do
        {
            m_jvec.emplace_back(SniperJSON(jstr, ++cursor));
        } while (getValidChar(jstr, cursor) == ',');
    }

    if (getValidChar(jstr, cursor) == ']')
    {
        ++cursor;
        return;
    }

    throw Exception(jstr, cursor);
}

void SniperJSON::readStringStr(const std::string &jstr, StrCursor &cursor)
{
    if (getValidChar(jstr, cursor) == '"')
    {
        StrCursor pstart = cursor;
        cursor = jstr.find('"', pstart + 1);
        if (cursor != std::string::npos)
        {
            ++cursor;
            m_jvar = jstr.substr(pstart, cursor - pstart);
            return;
        }
    }

    throw Exception(jstr, cursor);
}

void SniperJSON::readScalarStr(const std::string &jstr, StrCursor &cursor)
{
    StrCursor pstart = cursor;
    cursor = jstr.find_first_of(DELIMITS, pstart + 1);
    if (cursor != std::string::npos || pstart == 0)
    {
        m_jvar = jstr.substr(pstart, cursor - pstart);
        return;
    }

    throw Exception(jstr, cursor);
}

SniperJSON::Exception::Exception(const std::string &msg)
    : m_msg("json error: ")
{
    m_msg += msg;
}

SniperJSON::Exception::Exception(const std::string &jstr, int cursor)
    : m_msg("invalid json:\n")
{
    m_msg += jstr.substr(0, cursor + 1);
    m_msg += " <<< parsing error";
}

SniperJSON::Exception::~Exception() throw()
{
}

const char *SniperJSON::Exception::what() const throw()
{
    return m_msg.c_str();
}
