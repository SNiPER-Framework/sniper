#!/usr/bin/env python
# -*- coding:utf-8 -*-
# author: lintao

import Sniper
sniper = Sniper.Sniper()

task = sniper.createTask("Task/task")
task.setLogLevel(2)

import RootWriter
task.property("svcs").append("RootWriter")
task.setDescription("an example to fill ROOT Trees in an algorithm")
rw = task.find("RootWriter")
rw.property("Output").set({"FILE1": "output1.root", "FILE2": "output2.root"})

import SniperRootUsages
alg = task.createAlg("WriteRootAlg")

task.setEvtMax(5)
sniper.show()
sniper.run()
