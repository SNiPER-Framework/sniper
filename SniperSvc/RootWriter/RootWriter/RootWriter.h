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

#ifndef SNIPER_ROOT_WRITER_SVC_H
#define SNIPER_ROOT_WRITER_SVC_H

#include <boost/python.hpp>
#include "SniperKernel/SvcBase.h"
#include <map>

/**************************************************************************
 * Examples
 * It's similar to TTree and TNtuple :
 *     RootWriter* svc = SvcMgr::get<RootWriter>("RootWriter");
 *     TTree* tree = svc->bookTree("Fkey/dir/tree", "title");
 *
 * or to Histograms:
 *     RootWriter* svc = SvcMgr::get<RootWriter>("RootWriter");
 *     TH1D* hist = new TH1D(...)
 *     svc->attach("Fkey/dir", hist);
 *
 * the dir(TDirectory in TFile) is optional (0 - several levels)
 *************************************************************************/

class TFile;
class TTree;
class TNtuple;
class TDirectory;

class RootWriter : public SvcBase
{
    public :

	RootWriter(const std::string& name);
	virtual ~RootWriter();

	bool initialize();
	bool finalize();

	TTree* bookTree(const std::string& fullPath, const std::string& title);
	TNtuple* bookNtuple(const std::string& fullPath, const std::string& title, const std::string& leafs);

	template<class Type>
	bool attach(const std::string& fullDirs, Type* obj);  //fullDirs: fKey/Dirs
        bool attach_py(const std::string& fullDirs, boost::python::object obj);
    private :

	//fKey: the tag of a file
	bool addFile(const std::string& fKey, const std::string& fname);

	//get the TDirectory that associated to fullDirs
	TDirectory* getDir(const std::string& fullDirs);

	//fKey -> TFile
	std::map<std::string, TFile*> m_key2file;
        std::map<std::string, std::string> fmap;
};

template<class Type>
bool RootWriter::attach(const std::string& fullDirs, Type* obj)
{
    TDirectory* pDir = getDir(fullDirs);
    obj->SetDirectory(pDir);

    return (pDir!=0);
}


#endif
