#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    import SniperCoreUsages

    ###################################################
    mtsniper = Sniper.MtSniper()
    svc = Sniper.create("SharedElem<DummySvc>/SharedSvc")
    svc.setDescription("a service instance that share by different Tasks")

    ###################################################
    Sniper.setLogLevel(3)
    nthrd = 4
    mtsniper.setEvtMax(10)
    mtsniper.setNumThreads(nthrd)
    mtsniper.configGlobalBuffer(nthrd*3, nthrd)

    ###################################################
    task = mtsniper.createInputTask("Task/InputTask")

    ###################################################
    task = mtsniper.createOutputTask("Task/OutputTask")

    ###################################################
    task = mtsniper.createMainTask("Task/MainTask")
    task.addSvc(svc)
    alg = task.createAlg("SvcAndToolAlg")
    toolname = "tool1"
    alg.property("OwnedTool").set(toolname)
    tool = alg.createTool("DummyTool/"+toolname)
    tool.setDescription("a DummyTool instance owned by alg")
    tool.setLogLevel(4)

    ###################################################
    mtsniper.show()
    mtsniper.run()
