#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    Sniper.setLogLevel(2)
    Sniper.setColorful(3)
    #Sniper.setShowTime(True)

    sniper = Sniper.Sniper()
    task = sniper.createTask("Task/task")
    task.setEvtMax(5)
    task.setDescription("an example to use SniperLog and Properties")

    import SniperCoreUsages
    alg = task.createAlg("HelloWorld")
    alg.property("VarBool").set(True)
    alg.property("VecFloat").set([0.01])
    alg.property("PairDVD").set([0.09, [1.1, 2.2, 3.3]])
    alg.property("MapIntStr").set({1: 'str1'})

    sniper.show()
    sniper.run()
