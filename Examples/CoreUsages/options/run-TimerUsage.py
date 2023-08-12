#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    Sniper.setLogLevel(3)
    sniper = Sniper.Sniper()

    task = sniper.createTask("Task/task")
    task.setDescription("an example to use profiling")
    task.setEvtMax(5)
    
    import SniperProfiling
    svc1 = task.createSvc("SniperProfiling")
    svc1.setDescription("a service for providing some profiles")
    svc1.setLogLevel(2)

    import SniperCoreUsages
    svc2 = task.createSvc("DummySvc/SharedSvc")
    svc2.setDescription("a service instance that can be share by all algorithms")

    alg1 = task.createAlg("SvcAndToolAlg/alg1")
    alg1.setDescription("the 1st SvcAndToolAlg instance named as alg1")
    toolname = "tool1"
    alg1.property("OwnedTool").set(toolname)
    #alg1.property("SharedSvc").set("SharedSvc") #use its default value
    tool1 = alg1.createTool("DummyTool/"+toolname)
    tool1.setDescription("a DummyTool instance owned by alg1")

    alg2 = task.createAlg("SvcAndToolAlg/alg2")
    alg2.setDescription("the 2nd SvcAndToolAlg instance named as alg2")
    toolname = "tool2"
    alg2.property("OwnedTool").set(toolname)
    #alg2.property("SharedSvc").set("SharedSvc") #use its default value
    tool2 = alg2.createTool("DummyTool/"+toolname)
    tool2.setDescription("a DummyTool instance owned by alg2")

    sniper.show()
    sniper.run()
