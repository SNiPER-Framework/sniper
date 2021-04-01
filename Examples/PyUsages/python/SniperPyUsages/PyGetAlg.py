from Sniper import PyAlgBase

class PyGetAlg(PyAlgBase):

    def __init__(self, name):
        PyAlgBase.__init__(self, name)
        self.count = 0;

    def initialize(self):
        self.data = self.get("DataStore").data()
        self.LogInfo("initialized")
        return True

    def execute(self):
        self.count += 1
        self.LogDebug("PyGetAlg count: ", self.count)

        self.LogInfo("get data: ", self.data)

        return True

    def finalize(self):
        self.LogInfo("finalized")
        return True
