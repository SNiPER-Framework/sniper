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

#ifndef SNIPER_SNIPER_RUN_STATE_H
#define SNIPER_SNIPER_RUN_STATE_H

namespace Sniper
{
    typedef unsigned short RunStateInt;

    // the Running State of a Statemachine
    enum class RunState : RunStateInt {
        Invalid      =  0,
        StartUp      =  0x010,
        Ready        =  0x020,
        Running      =  0x040,
        Stopped      =  0x080,
        Finalized    =  0x100,
        EndUp        =  0x200,
        Error        =  0x800
    };
}

#endif
