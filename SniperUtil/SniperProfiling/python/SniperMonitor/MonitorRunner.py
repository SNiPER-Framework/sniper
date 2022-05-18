import sys
sys.path.append('../python/')

from PySide6.QtWidgets import QApplication
from SniperMonitor.MainMonitor import MainMonitor
#from SniperMonitor.MainMonitor import MainMonitor

if __name__ == "__main__":
    app = QApplication(sys.argv)
    sniper_monitor = MainMonitor()
    sniper_monitor.resize(1000, 500)
    sniper_monitor.show()
    sniper_monitor.AddInfo()

    sys.exit(app.exec())
