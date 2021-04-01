/* Copyright (C) 2021
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

#ifndef DUMMY_DATA_STORE_H
#define DUMMY_DATA_STORE_H

#include "SniperKernel/IDataBlock.h"

//In this example, only an integer in stored
//It can be any data in real cases

class DummyDataStore : public IDataBlock
{
public:
    DummyDataStore() : m_data(-1) {}

    //update the data in data store
    void update(int newdata)
    {
        m_data = newdata;
    }

    //get the data in data store
    int get()
    {
        return m_data;
    }

private:
    int m_data;
};

#endif