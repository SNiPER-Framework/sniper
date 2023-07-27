/* Copyright (C) 2018-2021
   Haolai Tian <tianhl@ihep.ac.cn>
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

#include "SniperKernel/Workflow.h"
#include "SniperKernel/DeclareDLE.h"
#include "SniperKernel/SniperContext.h"
#include "SniperPrivate/TaskProperty.h"
#include "SniperPrivate/WhiteBoard.h"

SNIPER_DECLARE_DLE(Workflow);

Workflow::Workflow(const std::string& name)
    : ExecUnit(name)
{
    if ( m_tag.empty() ) m_tag = "Workflow";  //protection for derived classes

    m_pmgr.addProperty(new TaskProperty("svcs", this));
    m_pmgr.addProperty(new TaskProperty("algs", this));
}

Workflow::~Workflow()
{
}

void Workflow::setLogLevel(int level)
{
    DLElement::setLogLevel(level);

    for (auto target : m_targets)
    {
        target->setLogLevel(level);
    }
}

bool Workflow::run()
{
    if ( sniper_context->check(Sniper::SysMode::MT) ) {
        LogWarn << "please use Muster::run() instead" << std::endl;
        return true;
    }

    if ( m_snoopy->config() ) {
        if ( m_snoopy->initialize() ) {
            if ( ! m_snoopy->run() ) {
                //LogError << "Failed to execute algorithms" << std::endl;
            }
            m_snoopy->finalize();
        }
    }

    return ! m_snoopy->isErr();
}

bool Workflow::stop(Sniper::StopRun mode)
{
    return m_snoopy->stop(mode);
}

void Workflow::reset()
{
    //for ( auto it = m_targets.rbegin(); it != m_targets.rend(); ++it ) {
    //    (*it)->clear();
    //}
    //m_targets.clear();
}

bool Workflow::config()
{
    WhiteBoard::mark(this);
    return true;
}

bool Workflow::initialize()
{
    bool stat = true;
    if ( ! m_svcs.initialize() ) stat = false;
    if ( ! m_algs.initialize() ) stat = false;

    if ( stat ) {
        LogInfo << "initialized" << std::endl;
    }
    else {
        m_snoopy->setErr();
    }

    return stat;
}

bool Workflow::finalize()
{
    bool stat = true;
    if ( ! m_algs.finalize() ) stat = false;
    if ( ! m_svcs.finalize() ) stat = false;

    LogInfo << "End Workflow " << std::endl;

    if ( ! stat ) {
        m_snoopy->setErr();
    }

    return stat;
}

bool Workflow::execute()
{
    return true;
}
