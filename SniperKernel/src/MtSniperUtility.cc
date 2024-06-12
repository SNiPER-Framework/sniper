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

#include "SniperKernel/MtSniperUtility.h"
#include "SniperKernel/MtsEvtBufferRing.h"
#include "SniperPrivate/MtsWorkerPool.h"
#include <condition_variable>

namespace MtSniperUtil
{
    // global variables
    bool MtSniperStatus{false};

    // file scoped variables
    static MtsEvtBufferRing *_pGlobalBuffer{nullptr};
    static MtsWorkerPool *_pWorkerPool{nullptr};
    static std::condition_variable _workerSync;

    // thread local variable
    static thread_local ucontext_t *_pIncubatorCtx{nullptr};
}

void MtSniperUtil::contextInit()
{
    _pGlobalBuffer = new MtsEvtBufferRing();
    _pWorkerPool = MtsWorkerPool::instance();

    MtSniperStatus = true;
}

void MtSniperUtil::contextRelease()
{
    MtSniperStatus = false;

    _pWorkerPool->destroy();
    delete _pGlobalBuffer;
}

void MtSniperUtil::Thread::pause()
{
    static std::mutex _mtx;
    std::unique_lock<std::mutex> _lock(_mtx);
    _workerSync.wait(_lock);
}

void MtSniperUtil::Thread::resumeOne()
{
    _workerSync.notify_one();
}

void MtSniperUtil::Thread::resumeAll()
{
    _workerSync.notify_all();
}

void MtSniperUtil::Worker::spawn(int n)
{
    _pWorkerPool->spawn(n);
}

void MtSniperUtil::Worker::waitAll()
{
    _pWorkerPool->waitAll();
}

void MtSniperUtil::Worker::raiseAnother()
{
    if (auto w = _pWorkerPool->allocate())
    {
        w->resume();
    }
    else
    {
        w = _pWorkerPool->create();
        w->initContext();
        w->resume();
    }
}

void MtSniperUtil::Worker::setIncubatorContext(ucontext_t *ctx)
{
    _pIncubatorCtx = ctx;
}

ucontext_t *MtSniperUtil::Worker::getIncubatorContext()
{
    return _pIncubatorCtx;
}

MtsEvtBufferRing *MtSniperUtil::GlobalBuffer::instance()
{
    return _pGlobalBuffer;
}