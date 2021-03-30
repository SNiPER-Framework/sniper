from __future__ import print_function
import sys
from Sniper import AlgBase, SniperLog

class PyAlgBase(AlgBase):

    def __init__(self, name):
        AlgBase.__init__(self, name)
        self.setTag(self.__class__.__name__)

    def _log(self, level, msgs):
        if ( self.logLevel() <= level):
            log = SniperLog(level, self.scope(), self.objName(), sys._getframe(2).f_code.co_name)
            for msg in msgs:
                log.msg(msg)
            log.msg('\n')

    def get(self, name):
        return self.getParent().find("DataMemSvc").find(name)

    def LogTest(self, *msgs):
        self._log(0, msgs)

    def LogDebug(self, *msgs):
        self._log(2, msgs)

    def LogInfo(self, *msgs):
        self._log(3, msgs)

    def LogWarn(self, *msgs):
        self._log(4, msgs)

    def LogError(self, *msgs):
        self._log(5, msgs)

    def LogFatal(self, *msgs):
        self._log(6, msgs)
