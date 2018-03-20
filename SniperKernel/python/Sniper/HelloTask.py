from libSniperPython import Task
from libSniperPython import Incident

class HelloTask(Task) :

    def __init__(self, name) :
        Task.__init__(self, name)
        Task.regist(self, "hi")

    def run(self) :
        print "Hello World!"

    def handle(self, incident) :
        if ( incident.name() == 'hi' ) :
            print "How are you doing ?"
        return Task.handle(self, incident)
