/* Copyright (C) 2018-2021
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

#include "SniperKernel/SvcBase.h"

class PyDataStoreSvc : public SvcBase
{
    public :

        PyDataStoreSvc(const std::string& name);
        ~PyDataStoreSvc();

        bool initialize();
        bool finalize();
};

#include "SniperPython/PyDataStore.h"
#include "SniperKernel/DataMemSvc.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SvcFactory.h"

DECLARE_SERVICE(PyDataStoreSvc);

PyDataStoreSvc::PyDataStoreSvc(const std::string& name)
    : SvcBase(name)
{
}

PyDataStoreSvc::~PyDataStoreSvc()
{
}

bool PyDataStoreSvc::initialize()
{
    SniperPtr<DataMemSvc> pSvc(m_par, "DataMemSvc");
    pSvc->regist(m_name, new PyDataStore());

    return true;
}

bool PyDataStoreSvc::finalize()
{
    return true;
}
