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
    useInterAlg = True
    nthrd = 4
    mtsniper.setEvtMax(50)

    mtsniper.setNumThreads(nthrd)
    mtsniper.configGlobalBuffer(nthrd*3, nthrd*2)

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

    ####################################
    alg = task.createAlg("FanOutAlg/input")
    alg.property("InitKeys").set(["v1", "v2", "v4", "sum12", "sum24"])
    alg.property("outputs").set(["input"])

    alg = task.createAlg("TimeConsumeAlg/v1")
    alg.property("TimeScale").set(1)
    alg.property("inputs").set(["input"])
    alg.property("outputs").set(["v1"])
    alg.createTool("MtTimeConsumeTool/TimeConsumeTool")

    alg = task.createAlg("TimeConsumeAlg/v2")
    alg.property("TimeScale").set(2)
    alg.property("inputs").set(["input"])
    alg.property("outputs").set(["v2"])
    alg.createTool("MtTimeConsumeTool/TimeConsumeTool")

    alg = task.createAlg("TimeConsumeAlg/v4")
    alg.property("TimeScale").set(4)
    alg.property("inputs").set(["input"])
    alg.property("outputs").set(["v4"])
    alg.createTool("MtTimeConsumeTool/TimeConsumeTool")

    alg = task.createAlg("FanInAlg/sum12")
    alg.property("inputs").set(["v1", "v2"])
    alg.property("outputs").set(["sum12"])

    alg = task.createAlg("FanInAlg/sum24")
    alg.property("inputs").set(["v2", "v4"])
    alg.property("outputs").set(["sum24"])

    if useRootWriter:
        task.addSvc(wroot)
        task.createSvc("EndEvtHandler4MtRootWriter")
        task.findAlg("input").createTool("FillRootTool/FillResultTool")
        task.findAlg("v1").createTool("FillRootTool/FillResultTool")
        task.findAlg("v2").createTool("FillRootTool/FillResultTool")
        task.findAlg("v4").createTool("FillRootTool/FillResultTool")
        task.findAlg("sum12").createTool("FillRootTool/FillResultTool")
        task.findAlg("sum24").createTool("FillRootTool/FillResultTool")

    if useInterAlg:
        task.enableInterAlgConcurrency()

    ###################################################
    mtsniper.show()
    mtsniper.run()
