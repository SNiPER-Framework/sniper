#!/usr/bin/env python
# -*- coding:utf-8 -*-
#This is an example to show the 3 modes of StopRun

import Sniper
import SniperCoreUsages

def run_stop(mode):
    sniper = Sniper.Sniper()
    task = sniper.createTask("Task/task")
    task.setLogLevel(2)
    task.setEvtMax(5)
    task.createSvc("DataIOSvc").setDescription("a dummy service for data I/O")
    task.createAlg("DummyAlg/alg1").property("INFO").set("The beginning of event ")
    task.createAlg("StopRunAlg/alg2").property("StopMode").set(mode)
    task.createAlg("DummyAlg/alg3").property("INFO").set("The ending of event ")

    sniper.show()
    sniper.run()

if __name__ == "__main__":

    for mode in ["Promptly", "Peacefully", "ThisEvent"]:
        print("\nBegin of Sniper::StopRun::%s" % mode, flush=True)
        run_stop(mode)
        print("End of Sniper::StopRun::%s" % mode, flush=True)
