from email.charset import QP
import psutil
from PySide6.QtCharts import QChart, QLineSeries, QValueAxis
from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QPen
from PySide6.QtWidgets import QLabel

class MemMonitor(QChart):
    def __init__(self, parent=None):
        super().__init__(QChart.ChartTypeCartesian, parent, Qt.WindowFlags())

        # init essential elements
        self.seriesMem = QLineSeries(self)
        self.seriesSwp = QLineSeries(self)
        self.axisX = QValueAxis()
        self.axisYleft = QValueAxis()
        self.axisYright = QValueAxis()

        # important values
        self.step = 1
        self.xRange = 20
        self.x = 0
        # total memory space
        self.memoryData = psutil.virtual_memory()
        self.swapData = psutil.swap_memory()
        self.GBCONV = 1073741824
        self.memoryTotal = psutil.virtual_memory().total / self.GBCONV
        self.swapTotal = psutil.swap_memory().total / self.GBCONV
        # get the memory utilization
        self.yMem = psutil.virtual_memory()
        self.ySwp = psutil.swap_memory()

        # set timer properties
        self.timer = QTimer()
        self.timer.timeout.connect(self.handleTimeout)
        self.timer.setInterval(1000)

        # set texts of legend and title
        self.legend().setVisible(True)
        self.legend().setAlignment(Qt.AlignBottom)
        self.seriesMem.setName("Memory: %.2f/%.2fGB (%.2f%%)"
                %(self.yMem.used / self.GBCONV, self.memoryTotal, self.yMem.percent))
        self.seriesSwp.setName("Swap: %.2f/%.2fGB (%.2f%%)"
                %(self.ySwp.used / self.GBCONV, self.swapTotal, self.ySwp.percent))
        self.axisX.setLabelFormat("%d")
        self.axisX.setTitleText("Time(s)")
        self.axisYleft.setTitleText("Percent(%)")
        self.axisYright.setTitleText("Percent(%)")

        # set series properties
        colorRed = QPen(Qt.red)
        colorBlue = QPen(Qt.blue)
        colorRed.setWidth(3)
        colorBlue.setWidth(3)
        self.seriesMem.setPen(colorRed)
        self.seriesSwp.setPen(colorBlue)
        self.seriesMem.append(self.x, self.yMem.percent)
        self.seriesSwp.append(self.x, self.ySwp.percent)

        # set axes properties
        self.addAxis(self.axisX, Qt.AlignBottom)
        self.addAxis(self.axisYleft, Qt.AlignLeft)
        self.addAxis(self.axisYright, Qt.AlignRight)

        self.axisX.setTickCount(self.xRange + 1)
        self.axisYleft.setTickCount(11)
        self.axisYright.setTickCount(11)

        self.axisX.setRange(-self.xRange, 0)
        self.axisYleft.setRange(0, 100)
        self.axisYright.setRange(0, 100)

        # attach axes to the graph
        self.addSeries(self.seriesMem)
        self.addSeries(self.seriesSwp)
        self.seriesMem.attachAxis(self.axisX)
        self.seriesMem.attachAxis(self.axisYleft)
        self.seriesMem.attachAxis(self.axisYright)
        self.seriesSwp.attachAxis(self.axisX)
        self.seriesSwp.attachAxis(self.axisYleft)
        self.seriesSwp.attachAxis(self.axisYright)

        # start timing
        self.timer.start()

    def handleTimeout(self):
        # one step forward
        self.x += self.step
        self.yMem = psutil.virtual_memory()
        self.ySwp = psutil.swap_memory()
        self.seriesMem.setName("Memory: %.2f/%.2fGB (%.2f%%)"
                %(self.yMem.used / self.GBCONV, self.memoryTotal, self.yMem.percent))
        self.seriesSwp.setName("Swap: %.2f/%.2fGB (%.2f%%)"
                %(self.ySwp.used / self.GBCONV, self.swapTotal, self.ySwp.percent))
        self.seriesMem.append(self.x, self.yMem.percent)
        self.seriesSwp.append(self.x, self.ySwp.percent)
        # move the line to the left for one step
        self.scroll(self.plotArea().width() / (self.axisX.tickCount() - 1), 0)