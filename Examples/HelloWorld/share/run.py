#!/usr/bin/env python
# -*- coding:utf-8 -*-
# author: lintao

# using the new Mgr.

import Sniper

if __name__ == "__main__":

    Sniper.setLogLevel(2)
    Sniper.setColorful(2)
    #Sniper.setShowTime(True)

    task = Sniper.Task("task")
    task.setEvtMax(1)

    import HelloWorld

    task.property("algs").append("HelloAlg/x")

    x = task.find("x")
    print "Before setting properties"
    x.show()

    print

    x.property("VarString").set("GOD")
    x.property("VectorInt").set(range(6))
    x.property("MapStrInt").set( {"str%d"%v:v for v in range(6)} )
    print "After setting properties"
    x.show()

    task.run()
