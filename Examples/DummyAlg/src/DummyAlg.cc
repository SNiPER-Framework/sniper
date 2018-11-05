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

#include "DummyAlg.h"
#include "DummyTool.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "RootWriter/RootWriter.h"
#include "TTree.h"

DECLARE_ALGORITHM(DummyAlg);

DummyAlg::DummyAlg(const std::string& name)
    : AlgBase(name)
{
    m_iEvt = 0;
}

DummyAlg::~DummyAlg()
{
}

bool DummyAlg::initialize()
{
    SniperPtr<RootWriter> m_rw(this->getParent(), "RootWriter");
    if ( ! m_rw.valid() ) {
	LogError << "Failed to get RootWriter instance!" << std::endl;
	return false;
    }

    m_tree1 = new TTree("tree1", "Dummy Test");
    m_tree1->Branch("iEvt",  &m_iEvt,  "iEvt/I");
    m_tree1->Branch("iLeaf", &m_iLeaf, "iLeaf/I");

    m_tree2 = new TTree("tree2", "Dummy Test");
    m_tree2->Branch("iEvt",  &m_iEvt,  "iEvt/I");
    m_tree2->Branch("fLeaf", &m_fLeaf, "fLeaf/F");

    m_tree3 = new TTree("tree3", "Dummy Test");
    m_tree3->Branch("iEvt",  &m_iEvt,  "iEvt/I");
    m_tree3->Branch("dLeaf", &m_dLeaf, "dLeaf/D");

    /* Make RootWriter be in charge of the Trees, so that we can set the
     * output file names in config_file as follows:
     *      RootWriter.Output = { "FILE1" : "output1.root",
     *                            "FILE2" : "output2.root" };
     *
     */
    m_rw->attach("FILE1", m_tree1);
    m_rw->attach("FILE1", m_tree3);

    m_rw->attach("FILE2", m_tree2);

    LogInfo << " initialized successfully" << std::endl;

    return true;
}

bool DummyAlg::execute()
{
    ++m_iEvt;

    //Valid log level: LogDebug, LogInfo, LogWarn, LogError, LogFatal
    LogDebug << "Processing event " << m_iEvt << std::endl;

    //calculations
    m_iLeaf = m_iEvt % 100;
    m_fLeaf = (m_iEvt / 3.377) * 0.019;
    m_dLeaf = (m_iEvt / 3.377111) * 0.019222;

    //fill the trees
    m_tree1->Fill();
    m_tree2->Fill();
    m_tree3->Fill();

    //call a tool
    DummyTool* ptool = tool<DummyTool>("dtool");
    ptool->doSomeThing();

    return true;
}

bool DummyAlg::finalize()
{
    LogInfo << " finalized successfully" << std::endl;

    return true;
}
