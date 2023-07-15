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
    useRootWriter = False
    nthrd = 4
    timeScale = 10
    mtsniper.setEvtMax(3000)
    mtsniper.setNumThreads(nthrd)
    if nthrd != 1:
        mtsniper.configGlobalBuffer(nthrd*3, nthrd+2)
    else:
        mtsniper.configGlobalBuffer(3, 1)

    ###################################################
    if useRootWriter:
        import RootWriter
        import SniperRootUsages
        wroot = Sniper.create("MtRootWriter/RootWriter")
        wroot.property("Output").set(
                {"MtsTest": "mtresult.root"}
                )

    ###################################################
    task = mtsniper.createInputTask("Task/InputTask")
    task.createSvc("FillGlobalBufSvc")
    alg = task.createAlg("FillGlobalBufAlg")
    #alg.property("MaxEvtNum").set(10)

    ###################################################
    task = mtsniper.createOutputTask("Task/OutputTask")
    task.createSvc("GetGlobalBufSvc")
    alg = task.createAlg("PruneGlobalBufAlg")
    if useRootWriter:
        tool = alg.createTool("WriteRootTool/WriteResultTool")
        task.addSvc(wroot)
    else:
        tool = alg.createTool("WriteAsciiTool/WriteResultTool")
        tool.property("OutputFile").set("mtresult.txt")

    ###################################################
    task = mtsniper.createMainTask("Task/MainTask")
    task.createSvc("GetGlobalBufSvc")
    svc = task.createSvc("SniperProfiling")
    alg = task.createAlg("TimeConsumeAlg")
    alg.property("TimeScale").set(timeScale)
    if nthrd == 1:
        svc.property("SaveDetails").set(True)
        alg.createTool("TimeConsumeTool")
    else:
        tool = alg.createTool("MtTimeConsumeTool/TimeConsumeTool")
        tool.property("MicroStep").set(10*timeScale)
    if useRootWriter:
        task.addSvc(wroot)
        alg.createTool("FillRootTool/FillResultTool")

    ###################################################
    mtsniper.show()
    mtsniper.run()
