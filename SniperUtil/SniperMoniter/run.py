import sys
from PySide6.QtWidgets import QApplication
from SniperMonitor import SniperMonitor

if __name__ == "__main__":
    app = QApplication(sys.argv)
    sniper_monitor = SniperMonitor()
    sniper_monitor.resize(1000, 500)
    sniper_monitor.show()
    sniper_monitor.AddInfo()

    sys.exit(app.exec())
