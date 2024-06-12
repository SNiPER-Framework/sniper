import Sniper

class SharedElemMgrClass():
    '''the class to manage all of the shared elements'''

    def __init__(self):
        self.elems = {}

    def create(self, identifier):
        elem = Sniper.create(identifier)
        self.elems[elem.objName()] = elem
        return elem

    #TODO: use index in case of duplicated names
    def get(self, name, index=None):
        return self.elems[name]

SharedElemMgr = SharedElemMgrClass()