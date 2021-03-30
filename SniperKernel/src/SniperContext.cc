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

#include "SniperKernel/SniperContext.h"
#include "SniperKernel/JSONParser.h"
#include "SniperKernel/SniperLog.h"
#include "SniperKernel/Sniper.h"
#include "SniperPrivate/IncidentMgr.h"
#include "SniperPrivate/DLEFactory.h"
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>

Sniper::Context::Context()
    : m_greeting(true),
      m_mode(static_cast<SysModeInt>(SysMode::BASIC)),
      m_nt(0)
{
    const char *finit = getenv("SNIPER_INIT_FILE");
    std::string welcome{"Welcome to SNiPER"};
    if (finit != nullptr)
    {
        std::vector<std::string> keys{
            "\"ShowGreeting\"",
            "\"WelcomeMsg\""};

        std::ifstream ifs(finit);
        auto json = SniperJSON::load(ifs);
        Sniper::JSONParser jp(json);
        if (!jp.check(keys))
        {
            std::cerr << "Error in init file: " << finit << '\n'
                      << jp.err() << std::endl;
            exit(-1);
        }

        jp.assign_if_exist(keys[0], m_greeting);
        jp.assign_if_exist(keys[1], welcome);
    }

    if (m_greeting)
    {
        std::cout << "**************************************************\n"
                  << welcome << '\n'
                  << "**************************************************\n"
                  << "Running @ " << hostName()
                  << " on " << Sniper::System::sysDate();
    }
}

Sniper::Context::~Context()
{
    IncidentMgr::release();
    DLEFactory::release();

    if (m_greeting)
    {
        std::cout << std::endl
                  << "**************************************************\n"
                  << "Terminating @ " << hostName()
                  << " on " << Sniper::System::sysDate()
                  << sys_info() << '\n';
    }
    std::cout << summary() << std::endl;

    if (dynamic_cast<std::ofstream *>(SniperLog::LogStream))
    {
        delete SniperLog::LogStream;
        SniperLog::LogStream = &std::cout;
    }
}

const std::string &Sniper::Context::hostName()
{
    if (m_hostName.empty())
    {
        char name[128];
        int status = gethostname(name, 128);
        m_hostName = status ? "?" : name;
    }
    return m_hostName;
}

void Sniper::Context::set(const Sniper::SysMode &mode)
{
    if (mode == SysMode::MT) //BASIC and MT conflict
        m_mode &= 0xFE;
    m_mode |= static_cast<SysModeInt>(mode);
}

void Sniper::Context::set_threads(short nt)
{
    m_nt = nt;
    set(SysMode::MT);
}

bool Sniper::Context::check(const Sniper::SysMode &mode)
{
    return (m_mode & static_cast<SysModeInt>(mode)) == static_cast<SysModeInt>(mode);
}

std::string Sniper::Context::sys_info()
{
    std::string mode = "BASIC";
    if (check(SysMode::MT))
        mode = "-Threads"; //BASIC and MT conflict
    if (check(SysMode::GPU))
        mode += " + GPU";
    if (check(SysMode::MPI))
        mode += " + MPI";

    std::ostringstream oss;
    oss << "SNiPER::Context Running Mode = { ";
    if (check(SysMode::MT))
        oss << m_nt;
    oss << mode << " }";

    return oss.str();
}

std::string Sniper::Context::summary()
{
    if (m_msg.empty())
    {
        return "SNiPER::Context Terminated Successfully";
    }

    std::ostringstream oss;
    for (auto &msg : m_msg)
    {
        oss << msg << std::endl;
    }
    return oss.str();
}

void Sniper::Context::reg_msg(const std::string &msg)
{
    m_msg.push_back(std::string("SNiPER::Context ") + msg);
}
