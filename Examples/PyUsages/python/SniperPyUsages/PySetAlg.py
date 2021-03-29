from Sniper import PyAlgBase

class PySetAlg(PyAlgBase):

    def __init__(self, name):
        PyAlgBase.__init__(self, name)
        self.count = 0;
        self.pVec = []  #initialize a list property
        self.pMap = {}  #initialize a dict property

    def initialize(self):
        self.data = self.get("DataStore").data()
        self.LogInfo("initialized")
        return True

    def execute(self):
        self.count += 1
        self.LogInfo("Begin event: ", self.count)
        self.LogDebug("PySetAlg count: ", self.count)


        self.data.clear()

        self.data['aStr'] = "Py" + str(self.count)
        self.data['aVec'] = list(i+self.count for i in self.pVec)
        self.data['aMap'] = self.pMap
        self.data['newValue'] = 1.11 * self.count

        self.LogInfo("set data: ", self.data)

        return True

    def finalize(self):
        self.LogInfo("finalized")
        return True
