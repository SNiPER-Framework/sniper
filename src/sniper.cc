/* Copyright (C) 2021
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

#include "SniperConfig.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <dlfcn.h>

int exe_python(const std::string &script)
{
#ifdef HAVE_PYTHON
    std::string pyver{SNIPER_USE_PYTHON_VERSION};
    std::string pycmd{"pythonv "};
    pycmd[6] = pyver.empty() ? ' ' : pyver.at(0);
    pycmd += script;
    return system(pycmd.c_str());
#else
    std::cerr << "error: this application is built without python" << std::endl;
    return 10;
#endif
}

int exe_json(const std::string &jfile)
{
    std::ifstream ifs(jfile);
    if (!ifs.good())
    {
        std::cerr << "invalid config file: " << jfile << std::endl;
        return 20;
    }

    void *handle = dlopen("libSniperKernel.so", RTLD_LAZY);
    if (handle)
    {
        typedef bool (*sniper_pfun)(const char *);
        sniper_pfun run_from_json = (sniper_pfun)dlsym(handle, "run_from_json");
        if (!run_from_json(jfile.c_str()))
        {
            return 21;
        }
    }
    dlclose(handle);

    return 0;
}

int main(int argc, char *argv[])
{

#ifdef HAVE_PYTHON
    std::string validfile{" config.[json|py]"};
#else
    std::string validfile{" config.json"};
#endif

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << validfile << std::endl;
        return 1;
    }

    std::string fname(argv[1]);
    if (fname.substr(fname.size() - 3, 10) == ".py")
    {
        return exe_python(fname);
    }
    else if (fname.substr(fname.size() - 5, 10) == ".json")
    {
        return exe_json(fname);
    }
    else
    {
        std::cerr << "error: unsupported config file " << fname << std::endl;
        std::cerr << "Usage: " << argv[0] << validfile << std::endl;
    }

    return 1;
}
