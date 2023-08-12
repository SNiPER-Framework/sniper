#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    sniper = Sniper.Sniper()

    task = sniper.createTask("Task/task")
    task.setEvtMax(5)

    import SniperPython
    task.createSvc("PyDataStoreSvc/DataStore")

    import SniperPyUsages

    # set PyDataStore in PySetAlg
    pyalg1 = SniperPyUsages.PySetAlg("PyAlg1")
    task.addAlg(pyalg1)
    #set algorithm properties
    pyalg1.pVec = list(range(3))
    pyalg1.pMap = {"str%d"%v:v for v in range(1, 3)}

    # update PyDataStore in CppUpdateAlg
    cppalg = task.createAlg("CppUpdateAlg/CppAlg")

    # get PyDataStore in PySetAlg
    pyalg2 = SniperPyUsages.PyGetAlg("PyAlg2")
    task.addAlg(pyalg2)

    sniper.show()
    sniper.run()
