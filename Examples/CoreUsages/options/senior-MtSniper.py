#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    import SniperCoreUsages
    import SniperProfiling

    ###################################################
    mtsniper = Sniper.MtSniper()

    ###################################################
    Sniper.setLogLevel(3)
    nthrd = 4
    mtsniper.setEvtMax(3000)
    mtsniper.setNumThreads(nthrd)
    mtsniper.configGlobalBuffer(nthrd*3, nthrd+2)

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
    svc = task.createSvc("SniperProfiling")
    if nthrd == 1:
        svc.property("SaveDetails").set(True)
    alg = task.createAlg("TimeConsumeAlg")
    alg.createTool("TimeConsumeTool")

    ###################################################
    mtsniper.show()
    mtsniper.run()
