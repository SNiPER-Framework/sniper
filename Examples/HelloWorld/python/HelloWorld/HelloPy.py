from Sniper import PyAlgBase

class HelloPy(PyAlgBase):

    def __init__(self, name):
        PyAlgBase.__init__(self, name)
        self.count = 0;

    def initialize(self):
        self.data = self.get("PyDataStore").data()
        self.LogInfo("initialized")
        return True

    def execute(self):
        self.count += 1

        # at present the logs in python can't be colored
        self.LogInfo("Hello Python: count: ", self.count)
        if ( self.count == 1 ):
            self.LogDebug("debug message")
            self.LogInfo("info message")
            self.LogWarn("warn message")
            self.LogError("error message")
            self.LogFatal("fatal message")
            return True

        print self.data
        self.data['aStr'] = "PY" + str(self.count)
        self.data['aVec'] = [ i+self.count for i in self.data['aVec'] ]
        self.data['newValue'] = 1.11 * self.count

        self.LogInfo("modify the data dict of PyDataStore to ...")
        print self.data
        print

        return True

    def finalize(self):
        self.LogInfo("finalized")
        return True
