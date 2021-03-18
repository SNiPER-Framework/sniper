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

#include "SniperKernel/Property.h"
#include <iostream>

Property::Property(const std::string &key_)
    : m_key(key_)
{
}

Property::~Property()
{
}

bool Property::append(const std::string & /*var*/)
{
    throw ContextMsgException(m_key + " : unable to append property");
}

void Property::make_indent(int indent)
{
    if (indent != 0)
    {
        for (int i = 0; i < indent - 1; ++i)
        {
            std::cout << "   |  ";
        }
        std::cout << "   +--";
    }
}
