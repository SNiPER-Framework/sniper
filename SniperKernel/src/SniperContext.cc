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

#include "SniperKernel/SniperContext.h"
#include <iostream>

Sniper::Context sniper_context;

Sniper::Context::Context()
    : m_mode(static_cast<SysModeInt>(SysMode::BASIC)),
      m_nt(0)
{
}

void Sniper::Context::set(const Sniper::SysMode& mode)
{
    if ( mode == SysMode::MT )  //BASIC and MT conflict
        m_mode &= 0xFE;
    m_mode |= static_cast<SysModeInt>(mode);
}

void Sniper::Context::set_threads(short nt)
{
    m_nt = nt;
    set(SysMode::MT);
}

bool Sniper::Context::check(const Sniper::SysMode& mode)
{
    return (m_mode&static_cast<SysModeInt>(mode)) == static_cast<SysModeInt>(mode);
}

void Sniper::Context::sys_info()
{
    std::string mode = "BASIC";
    if ( check(SysMode::MT) ) mode = "-Threads";  //BASIC and MT conflict
    if ( check(SysMode::GPU) ) mode += " + GPU";
    if ( check(SysMode::MPI) ) mode += " + MPI";

    std::cout << "SNiPER::Context Running Mode = { ";
    if ( check(SysMode::MT) ) std::cout << m_nt;
    std::cout << mode << " }"<< std::endl;
}

void Sniper::Context::summary()
{
    if ( m_msg.empty() ) {
        std::cout << "SNiPER::Context Terminated Successfully" << std::endl;
        return;
    }

    for ( auto& msg : m_msg ) {
        std::cout << msg << std::endl;
    }
}

void Sniper::Context::reg_msg(const std::string& msg)
{
    m_msg.push_back(std::string("SNiPER::Context ") + msg);
}
