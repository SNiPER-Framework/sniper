from __future__ import print_function
import sys
from Sniper import AlgBase

class PyAlgBase(AlgBase):

    def __init__(self, name):
        AlgBase.__init__(self, name)
        self.setTag(self.__class__.__name__)

    def _log(self, level, flag, msgs):
        #TODO: Colorful and ShowTime
        if ( self.logLevel() <= level):
            prefix = self.scope() + self.objName() + '.' + sys._getframe(2).f_code.co_name
            print("%-30s" % prefix, flag, end='')
            for msg in msgs:
                print(msg, end='')
            print()

    def get(self, name):
        return self.getParent().find("DataMemSvc").find(name)

    def LogTest(self, *msgs):
        self._log(0,  " TEST: ", msgs)

    def LogDebug(self, *msgs):
        self._log(2,  "DEBUG: ", msgs)

    def LogInfo(self, *msgs):
        self._log(3,  " INFO: ", msgs)

    def LogWarn(self, *msgs):
        self._log(4,  " WARN: " , msgs)

    def LogError(self, *msgs):
        self._log(5,  "ERROR: ", msgs)

    def LogFatal(self, *msgs):
        self._log(6,  "FATAL: ", msgs)
