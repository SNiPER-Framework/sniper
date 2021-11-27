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

#ifndef SNIPER_SHARED_ELEM_FACTORY_H
#define SNIPER_SHARED_ELEM_FACTORY_H

#include "SniperKernel/DLElement.h"
#include "SniperKernel/DeclareDLE.h"
#include <mutex>

// Enable a DLElement inherited class be shared in different Tasks
// IMPORTANT:
// The concrete class must be thread safe if the object is shared between threads

//forward declaration
namespace SharedElemMgr
{
   void take_ownership(DLElement *obj);
}

struct SharedElemBase
{
   virtual SniperJSON json_r() = 0;
   int index; // to distinguish elements that have same names
};

//the wrapper class for the shared DLElement
template<typename DLEClass>
class SharedElem : public DLEClass, public SharedElemBase
{
public:
   SharedElem(const std::string &name);
   virtual ~SharedElem();

   virtual bool initialize() override;
   virtual bool finalize() override;

   virtual void setParent(ExecUnit *parent) override;
   virtual SniperJSON json() override;
   virtual SniperJSON json_r() override;

private:
   bool m_iStat;
   bool m_fStat;
   bool m_initialized;
   bool m_finalized;
   std::mutex m_mutex;
};

template <typename DLEClass>
SharedElem<DLEClass>::SharedElem(const std::string &name)
    : DLEClass(name),
      m_iStat(false),
      m_fStat(false),
      m_initialized(false),
      m_finalized(false)
{
   this->m_scope = "SharedElem-";
   SharedElemMgr::take_ownership(this);
}

template <typename DLEClass>
SharedElem<DLEClass>::~SharedElem()
{
}

template <typename DLEClass>
bool SharedElem<DLEClass>::initialize()
{
   const std::lock_guard<std::mutex> lock(m_mutex);
   if (!m_initialized)
   {
      m_iStat = DLEClass::initialize();
      m_initialized = true;
   }
   return m_iStat;
}

template <typename DLEClass>
bool SharedElem<DLEClass>::finalize()
{
   const std::lock_guard<std::mutex> lock(m_mutex);
   if (!m_finalized)
   {
      m_fStat = DLEClass::finalize();
      m_finalized = true;
   }
   return m_fStat;
}

template <typename DLEClass>
void SharedElem<DLEClass>::setParent(ExecUnit *)
{
   //nothing to do
}

template <typename DLEClass>
SniperJSON SharedElem<DLEClass>::json()
{
   std::stringstream ss;
   ss << '[' << this->index << "]:" << this->m_tag << '/' << this->m_name;
   SniperJSON json;
   json["identifier"].from(ss.str());
   return json;
}

template <typename DLEClass>
SniperJSON SharedElem<DLEClass>::json_r()
{
   SniperJSON json = DLEClass::json();
   json["index"].from(this->index);
   return json;
}

#define DECLARE_CAN_BE_SHARED(DLEClass) \
SniperBookDLE SniperBook_Shared_##DLEClass##_("SharedElem<"#DLEClass">", &SniperCreateDLE_T<SharedElem<DLEClass>>)

#endif