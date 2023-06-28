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

#ifndef SNIPER_DATA_BUFFER_H
#define SNIPER_DATA_BUFFER_H

#include "SniperKernel/IDataBlock.h"
#include <memory>
#include <deque>

template <typename EvtType>
class DataBuffer : public IDataBlock
{
public:
    typedef std::shared_ptr<EvtType> ElementPtr;
    typedef typename std::deque<ElementPtr>::iterator Iterator;

    // return the iterator as requested
    inline Iterator begin();
    inline Iterator current();
    inline Iterator end();

    // find the iterator that holds the data
    inline Iterator find(EvtType *data);

    // the total size of this buffer
    inline int size();

    // return the current Data instance
    inline EvtType *curEvt();

    // constructor and destructor
    DataBuffer();
    virtual ~DataBuffer();

protected:
    void clear();

    int m_iCur;
    std::deque<ElementPtr> m_dBuf;

private:
    // following interfaces are not supported
    DataBuffer(const DataBuffer &);
    DataBuffer &operator=(const DataBuffer &);
};

template <typename EvtType>
typename DataBuffer<EvtType>::Iterator DataBuffer<EvtType>::begin()
{
    return m_dBuf.begin();
}

template <typename EvtType>
typename DataBuffer<EvtType>::Iterator DataBuffer<EvtType>::current()
{
    return m_dBuf.begin() + m_iCur;
}

template <typename EvtType>
typename DataBuffer<EvtType>::Iterator DataBuffer<EvtType>::end()
{
    return m_dBuf.end();
}

template <typename EvtType>
typename DataBuffer<EvtType>::Iterator DataBuffer<EvtType>::find(EvtType *data)
{
    for (Iterator it = m_dBuf.begin(); it != m_dBuf.end(); ++it)
    {
        if ((*it).get() == data)
            return it;
    }
    return m_dBuf.end();
}

template <typename EvtType>
EvtType *DataBuffer<EvtType>::curEvt()
{
    return m_dBuf[m_iCur].get();
}

template <typename EvtType>
int DataBuffer<EvtType>::size()
{
    return m_dBuf.size();
}

template <typename EvtType>
void DataBuffer<EvtType>::clear()
{
    m_dBuf.clear();
}

template <typename EvtType>
DataBuffer<EvtType>::DataBuffer()
    : m_iCur(-1)
{
    // std::cout << "Initail DataBuffer ..." << std::endl;
}

template <typename EvtType>
DataBuffer<EvtType>::~DataBuffer()
{
    clear();
}

#endif
