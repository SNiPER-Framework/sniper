#!/usr/bin/env python
# -*- coding:utf-8 -*-

if __name__ == "__main__":

    import Sniper
    
    mtsniper = Sniper.MtSniper()

    mtsniper.setNumThreads(4)
    mtsniper.setEvtMax(5)

    mtsniper.show()
    mtsniper.run()
