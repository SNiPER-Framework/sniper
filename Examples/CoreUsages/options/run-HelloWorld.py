#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    Sniper.setLogLevel(2)
    Sniper.setColorful(3)
    #Sniper.setShowTime(True)

    task = Sniper.Task("task")
    task.setEvtMax(5)
    task.setDescription("an example to use SniperLog and Properties")

    import SniperCoreUsages
    alg = task.createAlg("HelloWorld")
    alg.property("VarBool").set(True)
    alg.property("VecFloat").set([0.01])
    alg.property("MapIntStr").set({1: 'str1'})

    task.show()
    task.run()
