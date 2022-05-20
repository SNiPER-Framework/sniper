import psutil
from PySide6.QtCharts import QChart, QLineSeries, QValueAxis
from PySide6.QtCore import Qt, QTimer
from PySide6.QtGui import QPen

class CPUMonitor(QChart):
    def __init__(self, parent=None):
        super().__init__(QChart.ChartTypeCartesian, parent, Qt.WindowFlags())

        # init essential elements
        self.series = QLineSeries(self)
        self.axisX = QValueAxis()
        self.axisYleft = QValueAxis()
        self.axisYright = QValueAxis()

        # important values
        self.step = 1
        self.xRange = 20
        self.x = 0
        # get the cpu utilization
        self.y = psutil.cpu_percent()

        # set timer properties
        self.timer = QTimer()
        self.timer.timeout.connect(self.handleTimeout)
        self.timer.setInterval(1000)

        # set texts of legend and title
        self.legend().setVisible(True)
        self.legend().setAlignment(Qt.AlignBottom)
        self.series.setName("CPU: %.2f%%" %self.y)
        self.axisX.setLabelFormat("%d")
        self.axisX.setTitleText("Time(s)")
        self.axisYleft.setTitleText("Percent(%)")
        self.axisYright.setTitleText("Percent(%)")

        # set series properties
        redColor = QPen(Qt.red)
        redColor.setWidth(3)
        self.series.setPen(redColor)
        self.series.append(self.x, self.y)

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
        self.addSeries(self.series)
        self.series.attachAxis(self.axisX)
        self.series.attachAxis(self.axisYleft)
        self.series.attachAxis(self.axisYright)

        # start timing
        self.timer.start()

    def handleTimeout(self):
        # one step forward
        self.x += self.step
        self.y = psutil.cpu_percent()
        self.series.setName("CPU: %.2f%%" %self.y)
        self.series.append(self.x, self.y)
        # move the line to the left for one step
        self.scroll(self.plotArea().width() / (self.axisX.tickCount() - 1), 0)
