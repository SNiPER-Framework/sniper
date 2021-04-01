#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    Sniper.setLogLevel(2)

    task = Sniper.Task("task")
    task.setDescription("an example to use data I/O service")
    task.setEvtMax(5)

    import SniperCoreUsages
    svc = task.createSvc("DataIOSvc")
    svc.setDescription("a dummy service for data I/O")

    task.createAlg("DummyAlg").property("INFO").set("Get input data ")

    task.show()
    task.run()
