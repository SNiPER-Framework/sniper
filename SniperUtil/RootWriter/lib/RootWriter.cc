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

#include "RootWriter/RootWriter.h"
#include "SniperKernel/SvcFactory.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include <iostream>
#include <list>
#include <limits.h>
#include <stdlib.h>

DECLARE_SERVICE(RootWriter);

RootWriter::RootWriter(const std::string &name)
    : SvcBase(name)
{
    declProp("Output", fmap);
}

RootWriter::~RootWriter()
{
}

bool RootWriter::initialize()
{
    for (auto &it : fmap)
    {
        auto iabs = m_absfile.find(it.second);
        if (iabs == m_absfile.end())
        {
            addFile(it.first, it.second);
        }
        else
        {
            m_key2file[it.first] = iabs->second;
        }
    }

    return true;
}

bool RootWriter::finalize()
{
    for (auto &iabs : m_absfile)
    {
        iabs.second->Write();
        iabs.second->Close();
        delete iabs.second;
    }
    m_absfile.clear();
    m_key2file.clear();
    return true;
}

TTree *RootWriter::bookTree(const std::string &fullPath, const std::string &title)
{
    std::string::size_type lSeg = fullPath.rfind('/');
    std::string name = fullPath.substr(lSeg + 1, std::string::npos);
    std::string fullDirs = fullPath.substr(0, lSeg);

    TTree *result = new TTree(name.c_str(), title.c_str());
    attach(fullDirs, result);
    return result;
}

TNtuple *RootWriter::bookNtuple(const std::string &fullPath, const std::string &title, const std::string &leafs)
{
    std::string::size_type lSeg = fullPath.rfind('/');
    std::string name = fullPath.substr(lSeg + 1, std::string::npos);
    std::string fullDirs = fullPath.substr(0, lSeg);

    TNtuple *result = new TNtuple(name.c_str(), title.c_str(), leafs.c_str());
    attach(fullDirs, result);
    return result;
}

TDirectory *RootWriter::getDir(const std::string &fullPath)
{
    TDirectory *result = 0;

    std::list<std::string> vPath;
    std::string::size_type iSeg = 0;
    while (iSeg != std::string::npos)
    {
        std::string::size_type nSeg = fullPath.find('/', iSeg);
        std::string::size_type count = nSeg - iSeg;
        std::string path = fullPath.substr(iSeg, count);
        if (!path.empty())
        {
            vPath.push_back(fullPath.substr(iSeg, count));
        }
        iSeg = (nSeg == std::string::npos) ? nSeg : nSeg + 1;
    }

    if (vPath.empty())
        vPath.push_back(fullPath);

    const std::string &fKey = vPath.front();
    std::map<std::string, TFile *>::iterator it = m_key2file.find(fKey);
    if (it == m_key2file.end())
    {
        return result;
    }
    result = it->second;

    vPath.pop_front();
    while (!vPath.empty())
    {
        const std::string &path = vPath.front();
        TDirectory *pDir = result->GetDirectory(path.c_str());
        if (pDir == 0)
        {
            pDir = result->mkdir(path.c_str());
        }
        result = pDir;
        vPath.pop_front();
    }

    return result;
}

bool RootWriter::addFile(const std::string &fKey, const std::string &fname)
{
    std::map<std::string, TFile *>::iterator it = m_key2file.find(fKey);
    if (it == m_key2file.end())
    {
        const char *preDir = gDirectory->GetPath();
        auto fptr = TFile::Open(fname.c_str(), "RECREATE");
        m_key2file[fKey] = fptr;
        m_absfile[fname] = fptr;
        gDirectory->cd(preDir);
        return true;
    }
    return false;
}
