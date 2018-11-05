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

#ifndef SNIPER_REF_BASE_H
#define SNIPER_REF_BASE_H

template<typename Type>
class RefBase
{
    public :

        //Standard constructor
        RefBase()  : m_obj(0) {}

        //Constructor with initialization
        RefBase(Type* pObj)  : m_obj(pObj) {}

        //Constructor with initialization from const object
        RefBase(const Type* pObj)  : m_obj(const_cast<Type*>(pObj)) {}

        //Copy constructor
        RefBase(const RefBase& copy)  : m_obj(copy.m_obj) {}

        //Assignment operator
        RefBase<Type>& operator=(const RefBase<Type>& right) {
            m_obj = right.m_obj;
            return *this;
        }

        //Setter
        void set(Type* pObj) { m_obj = pObj; }
        //Getter
        Type* data() { return m_obj; }
        //const Getter
        const Type* data() const { return m_obj; }

        //Check its validity
        bool valid() { return m_obj != 0; }
        bool invalid() { return m_obj == 0; }

        //users should check m_obj's validity before dereference opration
        //Dereference operator ->
        Type* operator->() { return m_obj; }
        //const Dereference operator ->
        const Type* operator->() const { return m_obj; }
        //Dereference operator *
        Type& operator*() { return *m_obj; }
        //const Dereference operator *
        const Type& operator*() const { return *m_obj; }


    protected :

        Type* m_obj;
};

#endif
