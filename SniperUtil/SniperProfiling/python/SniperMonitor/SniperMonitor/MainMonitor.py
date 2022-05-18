from PySide6.QtGui import QPainter
from PySide6.QtCharts import QChart, QChartView
from PySide6.QtWidgets import QMainWindow, QTabWidget
from SniperMonitor.CPUMonitor import CPUMonitor
from SniperMonitor.MemMonitor import MemMonitor

class MainMonitor(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.tab_widget = QTabWidget()

        self.setCentralWidget(self.tab_widget)
        self.setWindowTitle("SniperMonitor")
        self.AddInfo

    def AddInfo(self):
        cpu_monitor = CPUMonitor()
        self.AddNewTab(cpu_monitor, "CPU")

        mem_monitor = MemMonitor()
        self.AddNewTab(mem_monitor, "Memory")

    def AddNewTab(self, obj, name):
        obj.setAnimationOptions(QChart.SeriesAnimations)
        char_view = QChartView(obj)
        char_view.setRenderHint(QPainter.Antialiasing)
        self.tab_widget.addTab(char_view, name)
