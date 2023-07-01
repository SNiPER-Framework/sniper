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
#include "SniperKernel/DeclareDLE.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
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
    Double_t m_input;
    Double_t m_result;

    MtTTreeStore *m_treeStore;
};
SNIPER_DECLARE_DLE(FillRootTool);

FillRootTool::FillRootTool(const std::string &name)
    : ToolBase(name)
{
}

bool FillRootTool::initialize()
{
    SniperPtr<RootWriter> rw(m_par, "RootWriter");
    m_tree = rw->bookTree(*m_par, "MtsTest/event", "Result of MtSniper Test");
    m_tree->Branch("EventID", &m_id, "EventID/L");
    m_tree->Branch("input", &m_input, "input/D");
    m_tree->Branch("result", &m_result, "result/D");

    m_treeStore = SniperDataPtr<MtTTreeStore>(m_par, "MtTTreeStore").data();

    return true;
}

void FillRootTool::fill(MappedEvent &emap)
{
    // fill the ROOT TTree
    auto &evt = *std::any_cast<std::shared_ptr<JsonEvent> &>(emap["event"]);
    m_id = evt["EventID"].get<Long64_t>();
    m_input = evt["input"].get<Double_t>();
    m_result = evt["result"].get<Double_t>();
    m_tree->Fill();

    // move the TTree clones into GlobalBuffer in case of MtRootWriter
    if (m_treeStore != nullptr)
    {
        auto &trees = m_treeStore->trees();
        emap.insert(std::make_pair("trees", std::make_any<std::vector<MtTTree *>>()));
        trees.swap(std::any_cast<std::vector<MtTTree *> &>(emap["trees"]));
    }
}

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