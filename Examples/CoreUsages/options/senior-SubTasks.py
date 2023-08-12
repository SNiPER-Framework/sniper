#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    import SniperCoreUsages
    sniper = Sniper.Sniper()

    top = sniper.createTask("TopTask/TopT")
    top.setLogLevel(2)
    top.setDescription("an example to use sub-tasks")
    top.setEvtMax(5)
    alg = top.createAlg("SubTasksAlg")
    alg.setDescription("fire SubT1 for odd events and SubT2 for even events")

    t1 = top.createTask("Task/SubT1")
    t1.createSvc("DataIOSvc")
    t1.createAlg("DummyAlg").property("INFO").set("SubT1 execute ")

    t2 = top.createTask("Task/SubT2")
    md = t2.createSvc("MappedDataSvc")
    md.regist(t1, "output", "input")
    md.regist(t1, "output", "output")
    t2.createAlg("DummyAlg").property("INFO").set("SubT2 execute ")

    sniper.show()
    sniper.run()
