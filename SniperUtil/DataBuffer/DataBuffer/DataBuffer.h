/* Copyright (C) 2018
   Jiaheng Zou <zoujh@ihep.ac.cn> Tao Lin <lintao@ihep.ac.cn>
   Weidong Li <liwd@ihep.ac.cn> Xingtao Huang <huangxt@sdu.edu.cn>
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

template<typename Data>
class DataBuffer : public IDataBlock
{
    public :

        typedef std::shared_ptr<Data> ElementPtr;
        typedef typename std::deque<ElementPtr>::iterator Iterator;

        //return the iterator as requested
        inline Iterator begin();
        inline Iterator current();
        inline Iterator end();

        //find the iterator that holds the data
        inline Iterator find(Data* data);

        //the total size of this buffer
        inline int      size();

        //return the current Data instance
        inline Data*    curEvt();

        //constructor and destructor
        DataBuffer();
        virtual ~DataBuffer();


    protected :

        void  clear();

        int  m_iCur;
        std::deque<ElementPtr>  m_dBuf;


    private :
        // following interfaces are not supported
        DataBuffer(const DataBuffer&);
        DataBuffer& operator=(const DataBuffer&);
};

template<typename Data>
typename DataBuffer<Data>::Iterator DataBuffer<Data>::begin()
{
    return m_dBuf.begin();
}

template<typename Data>
typename DataBuffer<Data>::Iterator DataBuffer<Data>::current()
{
    return m_dBuf.begin()+m_iCur;
}

template<typename Data>
typename DataBuffer<Data>::Iterator DataBuffer<Data>::end()
{
    return m_dBuf.end();
}

template<typename Data>
typename DataBuffer<Data>::Iterator DataBuffer<Data>::find(Data* data)
{
    for ( Iterator it = m_dBuf.begin(); it != m_dBuf.end(); ++it ) {
        if ( (*it).get() == data ) return it;
    }
    return m_dBuf.end();
}

template<typename Data>
Data* DataBuffer<Data>::curEvt()
{
    return m_dBuf[m_iCur].get();
}

template<typename Data>
int DataBuffer<Data>::size()
{
    return m_dBuf.size();
}

template<typename Data>
void DataBuffer<Data>::clear()
{
    m_dBuf.clear();
}

template<typename Data>
DataBuffer<Data>::DataBuffer()
    : m_iCur(-1)
{
    //std::cout << "Initail DataBuffer ..." << std::endl;
}

template<typename Data>
DataBuffer<Data>::~DataBuffer()
{
    clear();
}

#endif
