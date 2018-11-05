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

#ifndef SNIPER_SNIPER_PTR_H
#define SNIPER_SNIPER_PTR_H

#include "SniperKernel/RefBase.h"
#include "SniperKernel/Task.h"
#include <string>

//Get the DLE(Dynamically Loadable Element) reference with its path
template<typename DLE>
class SniperPtr : public RefBase<DLE>
{
    public :

        //Constructor with the DLE's domain and path
        SniperPtr(Task* domain, const std::string& path);
        SniperPtr(Task& domain, const std::string& path);

        virtual ~SniperPtr() = default;

    private :

        //following methods are not supported
        SniperPtr() = delete;
        SniperPtr(const SniperPtr&) = delete;
        SniperPtr& operator=(const SniperPtr&) = delete;
};

//SniperPtr(const std::string& path) is removed (for MT mode)

template<typename DLE>
SniperPtr<DLE>::SniperPtr(Task* domain, const std::string& path)
{
    this->m_obj = dynamic_cast<DLE*>(domain->find(path));
}

template<typename DLE>
SniperPtr<DLE>::SniperPtr(Task& domain, const std::string& path)
{
    this->m_obj = dynamic_cast<DLE*>(domain.find(path));
}

#endif
