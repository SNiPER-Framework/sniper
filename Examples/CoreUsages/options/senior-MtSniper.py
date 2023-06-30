#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    import SniperCoreUsages

    ###################################################
    mtsniper = Sniper.MtSniper()
    svc = Sniper.create("SharedElem<DummySvc>/SharedSvc")
    svc.setDescription("a service instance that share by different Tasks")
    svc.setLogLevel(4)

    ###################################################
    Sniper.setLogLevel(3)
    nthrd = 4
    mtsniper.setEvtMax(10000)
    mtsniper.setNumThreads(nthrd)
    mtsniper.configGlobalBuffer(nthrd*3, nthrd*2)

    ###################################################
    task = mtsniper.createInputTask("Task/InputTask")
    alg = task.createAlg("FillGlobalBufAlg")
    #alg.property("MaxEvtNum").set(10)

    ###################################################
    task = mtsniper.createOutputTask("Task/OutputTask")
    alg = task.createAlg("PruneGlobalBufAlg")
    alg.property("OutputFile").set("mtresult.txt")

    ###################################################
    task = mtsniper.createMainTask("Task/MainTask")
    alg = task.createAlg("TimeConsumeAlg")

    ###################################################
    mtsniper.show()
    mtsniper.run()
