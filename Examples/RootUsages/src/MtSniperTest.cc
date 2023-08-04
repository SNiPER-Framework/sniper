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

#include "SniperKernel/SniperJSON.h"
#include "SniperKernel/ToolBase.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SvcBase.h"
#include "SniperKernel/IIncidentHandler.h"
#include "SniperKernel/DeclareDLE.h"
#include "RootWriter/RootWriter.h"
#include "RootWriter/MtTTree.h"
#include "RootWriter/MtTTreeStore.h"
#include <any>
#include <memory>

typedef SniperJSON JsonEvent;
typedef std::map<std::string, std::any> MappedEvent;

class IFillResultTool
{
public:
    virtual void fill(MappedEvent &emap) = 0;
};

class IWriteResultTool
{
public:
    virtual void save(MappedEvent &emap) = 0;
};

class IGetGlobalBufSvc
{
public:
    virtual MappedEvent &get() = 0;
    virtual MappedEvent &pop() = 0;
};

////////////////////////////////////////////////////////////////////////////////
class FillRootTool : public ToolBase, public IFillResultTool
{
public:
    FillRootTool(const std::string &name);
    virtual ~FillRootTool() = default;

    virtual bool initialize() override;

    virtual void fill(MappedEvent &emap) override;

private:
    TTree *m_tree;
    Long64_t m_id;
    Double_t m_value;
    std::string m_vstr;
};
SNIPER_DECLARE_DLE(FillRootTool);

FillRootTool::FillRootTool(const std::string &name)
    : ToolBase(name)
{
}

bool FillRootTool::initialize()
{
    m_vstr = getParentAlg()->objName();

    auto rw = get<RootWriter>("RootWriter");
    m_tree = rw->bookTree(*m_par, (std::string("MtsTest/") + m_vstr).c_str(), "Result of MtSniper Test");
    m_tree->Branch("EventID", &m_id, "EventID/L");
    m_tree->Branch(m_vstr.c_str(), &m_value, (m_vstr + "/D").c_str());

    return true;
}

void FillRootTool::fill(MappedEvent &emap)
{
    // fill the ROOT TTree
    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);
    m_id = evt["EventID"].get<Long64_t>();
    m_value = evt[m_vstr].get<Double_t>();
    m_tree->Fill();
}

////////////////////////////////////////////////////////////////////////////////
class EndEvtHandler4MtRootWriter : public SvcBase
{
public:
    EndEvtHandler4MtRootWriter(const std::string &name) : SvcBase(name) {}
    virtual ~EndEvtHandler4MtRootWriter() = default;

    virtual bool initialize() override;
    virtual bool finalize() override;

private:
    class EndEvtHandler : public IIncidentHandler
    {
    public:
        EndEvtHandler(ExecUnit *domain);
        virtual ~EndEvtHandler() = default;

        virtual bool handle(Incident &incident) override;

    private:
        IGetGlobalBufSvc *m_getter;
        MtTTreeStore *m_treeStore{nullptr};
    };

    EndEvtHandler *m_handler;
};
SNIPER_DECLARE_DLE(EndEvtHandler4MtRootWriter);

bool EndEvtHandler4MtRootWriter::initialize()
{
    m_handler = new EndEvtHandler(m_par);
    m_handler->regist("EndEvent");
    return true;
}

bool EndEvtHandler4MtRootWriter::finalize()
{
    m_handler->unregist("EndEvent");
    delete m_handler;
    return true;
}

EndEvtHandler4MtRootWriter::EndEvtHandler::EndEvtHandler(ExecUnit *domain)
    : IIncidentHandler(domain)
{
    m_getter = dynamic_cast<IGetGlobalBufSvc *>(domain->find("GetGlobalBufSvc"));
}

bool EndEvtHandler4MtRootWriter::EndEvtHandler::handle(Incident & /*incident*/)
{
    // the MtTTreeStore cann't be retrieved before the first invoking of bookTree
    // in other words, after the initialize of all algorithms
    if (m_treeStore == nullptr)
    {
        DataMemSvc *dsvc = dynamic_cast<DataMemSvc *>(m_domain.find("DataMemSvc"));
        m_treeStore = dynamic_cast<MtTTreeStore *>(dsvc->find("MtTTreeStore"));
    }

    // move the TTree clones into GlobalBuffer in case of MtRootWriter
    auto &emap = m_getter->get();
    emap.insert(std::make_pair("trees", std::make_any<std::vector<MtTTree *>>()));
    auto &trees = m_treeStore->trees();
    trees.swap(std::any_cast<std::vector<MtTTree *> &>(emap["trees"]));

    return true;
}

////////////////////////////////////////////////////////////////////////////////
class WriteRootTool : public ToolBase, public IWriteResultTool
{
public:
    WriteRootTool(const std::string &name) : ToolBase(name) {}
    virtual ~WriteRootTool() = default;

    virtual void save(MappedEvent &emap) override;
};
SNIPER_DECLARE_DLE(WriteRootTool);

void WriteRootTool::save(MappedEvent &emap)
{
    auto &trees = std::any_cast<std::vector<MtTTree *> &>(emap["trees"]);
    for (auto tree : trees)
    {
        tree->DoFillOne();
    }
}