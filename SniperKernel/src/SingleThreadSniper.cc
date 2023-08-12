/* Copyright (C) 2023
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

#include "SniperKernel/SingleThreadSniper.h"
#include "SniperKernel/Sniper.h"
#include "SniperKernel/SniperException.h"
#include "SniperKernel/DeclareDLE.h"

// a trick: use the short string "Sniper" instead of the class name as its tag
static SniperBookDLE SniperBook_SingleThreadSniper_("Sniper", &SniperCreateDLE_T<SingleThreadSniper>);

SingleThreadSniper::SingleThreadSniper(const std::string &name)
    : DLElement(name)
{
    m_tag = "Sniper";
}

SingleThreadSniper::~SingleThreadSniper()
{
    delete m_task;
}

Task *SingleThreadSniper::createTask(const std::string &identifier)
{
    if (m_task != nullptr)
    {
        throw ContextMsgException("already exist Task");
    }

    auto p = Sniper::create(identifier);
    if (p != nullptr)
    {
        m_task = dynamic_cast<Task *>(p);
        if (m_task == nullptr)
        {
            delete p;
        }
    }
    return m_task;
}

bool SingleThreadSniper::run()
{
    m_task->setLogLevel(m_logLevel);

    auto &snoopy = m_task->Snoopy();
    if (snoopy.config())
    {
        if (snoopy.initialize())
        {
            if (!snoopy.run())
            {
                //LogError << "Failed to execute algorithms" << std::endl;
            }
            snoopy.finalize();
        }
    }

    return !snoopy.isErr();
}

SniperJSON SingleThreadSniper::json()
{
    static SniperJSON keys = SniperJSON().from(std::vector<std::string>{
        "identifier",
        "properties",
        "Task"});

    SniperJSON j;
    j.insert("Config", SniperJSON(Sniper::Config::json_str()));
    if (j["Config"].find("LoadDlls") != j["Config"].map_end())
    {
        j["Config"]["LoadDlls"].format(false);
    }

    j.insert("Engine", DLElement::json());
    if (m_task != nullptr)
    {
        j["Engine"]["Task"] = m_task->json();
        j["Engine"]["ordered_keys"] = keys;
    }

    return j;
}

void SingleThreadSniper::eval(const SniperJSON &json)
{
    // eval for base class
    DLElement::eval(json);

    // eval the Task
    if (json.find("Task") != json.map_end())
    {
        auto &jtask = json["Task"];
        createTask(jtask["identifier"].get<std::string>());
        m_task->eval(jtask);
    }
}

bool SingleThreadSniper::initialize()
{
    return true;
}

bool SingleThreadSniper::finalize()
{
    return true;
}