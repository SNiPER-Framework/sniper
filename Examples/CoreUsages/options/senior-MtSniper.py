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
    mtsniper.setEvtMax(3000)
    mtsniper.setNumThreads(nthrd)
    mtsniper.configGlobalBuffer(nthrd*3, nthrd+2)

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
    else:
        tool = alg.createTool("WriteAsciiTool/WriteResultTool")
        tool.property("OutputFile").set("mtresult.txt")

    ###################################################
    task = mtsniper.createMainTask("Task/MainTask")
    task.createSvc("GetGlobalBufSvc")
    svc = task.createSvc("SniperProfiling")
    if nthrd == 1:
        svc.property("SaveDetails").set(True)
    alg = task.createAlg("TimeConsumeAlg")
    alg.createTool("MtTimeConsumeTool/TimeConsumeTool")
    if useRootWriter:
        task.addSvc(wroot)
        alg.createTool("FillRootTool/FillResultTool")

    ###################################################
    mtsniper.show()
    mtsniper.run()
