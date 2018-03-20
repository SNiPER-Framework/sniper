#!/usr/bin/env python
# -*- coding:utf-8 -*-
# author: lintao

import Sniper

task = Sniper.Task("task")
task.setLogLevel(2)

import RootWriter
task.property("svcs").append("RootWriter")
rw = task.find("RootWriter")
rw.property("Output").set({"FILE1": "output1.root", "FILE2": "output2.root"})

import DummyAlg  #infact DummyTool is imported at the same time
alg = task.createAlg("DummyAlg/dalg")
alg.createTool("DummyTool/dtool")

task.setEvtMax(5)
task.show()
task.run()
