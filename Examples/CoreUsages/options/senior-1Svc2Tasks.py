#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    import SniperCoreUsages

    top = Sniper.TopTask("TopT")
    top.setLogLevel(2)
    top.setDescription("an example to share a service in multiple tasks")
    top.setEvtMax(5)
    alg = top.createAlg("SubTasksAlg")
    alg.setDescription("fire SubT1 for odd events and SubT2 for even events")

    svc = Sniper.create("SharedElem<DummySvc>/SharedSvc")
    svc.setDescription("a service instance that share by different Tasks")

    t1 = top.createTask("Task/SubT1")
    t1.addSvc(svc)
    alg1 = t1.createAlg("SvcAndToolAlg/alg1")
    toolname = "tool1"
    alg1.property("OwnedTool").set(toolname)
    alg1.createTool("DummyTool/"+toolname).setDescription("a DummyTool instance owned by alg1")

    t2 = top.createTask("Task/SubT2")
    t2.addSvc(svc)
    alg2 = t2.createAlg("SvcAndToolAlg/alg2")
    toolname = "tool2"
    alg2.property("OwnedTool").set(toolname)
    alg2.createTool("DummyTool/"+toolname).setDescription("a DummyTool instance owned by alg2")

    top.show()
    top.run()
