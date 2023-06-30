/* Copyright (C) 2023
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

#include "SniperKernel/MtSniperContext.h"
#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/DeclareDLE.h"
#include "RootWriter/RootWriter.h"
#include "TTree.h"

typedef SniperJSON JsonEvent;
typedef std::map<std::string, std::any> MappedEvent;

class IGetGlobalBufSvc
{
public:
    virtual MappedEvent &get() = 0;
    virtual MappedEvent &pop() = 0;
    virtual void done() = 0;
};

////////////////////////////////////////////////////////////////////////////////
class MtsSaveRootHandler : public IIncidentHandler
{
public:
    MtsSaveRootHandler(ExecUnit *domain)
        : IIncidentHandler(domain) {}
    virtual ~MtsSaveRootHandler() = default;

    virtual bool handle(Incident &incident) override;
};

bool MtsSaveRootHandler::handle(Incident &Incident)
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
class MtsStoreRootSvc : public SvcBase
{
public:
    MtsStoreRootSvc(const std::string &name);
    virtual ~MtsStoreRootSvc() = default;

    virtual bool initialize() override;
    virtual bool finalize() override { return true; }
};
SNIPER_DECLARE_DLE(MtsStoreRootSvc);

MtsStoreRootSvc::MtsStoreRootSvc(const std::string &name)
    : SvcBase(name)
{
}

bool MtsStoreRootSvc::initialize()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////
class MtsWriteRootAlg : public AlgBase
{
public:
    MtsWriteRootAlg(const std::string &name);
    virtual ~MtsWriteRootAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override { return true; }

private:
    IGetGlobalBufSvc *m_svc;
};
SNIPER_DECLARE_DLE(MtsWriteRootAlg);

MtsWriteRootAlg::MtsWriteRootAlg(const std::string &name)
    : AlgBase(name)
{
}

bool MtsWriteRootAlg::initialize()
{
    m_svc = SniperPtr<IGetGlobalBufSvc>(getParent(), "GetGlobalBufSvc").data();
    return true;
}

bool MtsWriteRootAlg::execute()
{
    auto &emap = m_svc->get();

    return true;
}

////////////////////////////////////////////////////////////////////////////////
class PruneGlobalBuf2RootAlg : public AlgBase
{
public:
    PruneGlobalBuf2RootAlg(const std::string &name);
    virtual ~PruneGlobalBuf2RootAlg() = default;

    virtual bool initialize() override;
    virtual bool execute() override;
    virtual bool finalize() override { return true; }

private:
    IGetGlobalBufSvc *m_svc;
};
SNIPER_DECLARE_DLE(PruneGlobalBuf2RootAlg);

PruneGlobalBuf2RootAlg::PruneGlobalBuf2RootAlg(const std::string &name)
    : AlgBase(name)
{
}

bool PruneGlobalBuf2RootAlg::initialize()
{
    m_svc = SniperPtr<IGetGlobalBufSvc>(getParent(), "GetGlobalBufSvc").data();
    return true;
}

bool PruneGlobalBuf2RootAlg::execute()
{
    auto &emap = m_svc->pop();
    //auto &pevt = std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);

    return true;
}