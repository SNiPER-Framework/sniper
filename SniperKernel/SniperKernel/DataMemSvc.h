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

#ifndef SNIPER_DATA_MEM_SVC_H
#define SNIPER_DATA_MEM_SVC_H

#include "SniperKernel/SvcBase.h"

class IDataBlock;

class DataMemSvc : public SvcBase
{
    public :

        DataMemSvc(const std::string& name);
        virtual ~DataMemSvc();

        //find a DataBlock with its name
        IDataBlock*  find(const std::string& name);

        //regist a DataBlock to this service
        bool regist(const std::string& name, IDataBlock* mem, bool owned = true);

        //initialize and finalize
        bool initialize() override;
        bool finalize() override;


    private :

        std::map<std::string, std::pair<IDataBlock*, bool>> m_mems;
};

#endif
