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
        bool regist(const std::string& name, IDataBlock* mem);

        //initialize and finalize
        bool initialize();
        bool finalize();


    private :

        std::map<std::string, IDataBlock*> m_mems;
};

#endif
