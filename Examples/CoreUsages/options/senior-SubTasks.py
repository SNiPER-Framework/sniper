#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    import SniperCoreUsages

    top = Sniper.TopTask("TopT")
    top.setLogLevel(2)
    top.setDescription("an example to use sub-tasks")
    top.setEvtMax(5)
    alg = top.createAlg("SubTasksAlg")
    alg.setDescription("fire SubT1 for odd events and SubT2 for even events")

    t1 = top.createTask("Task/SubT1")
    t1.createSvc("DataIOSvc")
    t1.createAlg("DummyAlg").property("INFO").set("SubT1 execute ")

    t2 = top.createTask("Task/SubT2")
    t2.createSvc("DataIOSvc")
    t2.createAlg("DummyAlg").property("INFO").set("SubT2 execute ")

    top.show()
    top.run()
