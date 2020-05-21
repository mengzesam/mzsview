import sys
import pandas
import numpy
from PyQt5.QtCore import Qt,QCoreApplication
from PyQt5.QtWidgets import (
    QApplication,QMainWindow,QMessageBox,QFileDialog,QTableWidget,QTableWidgetItem,QDialog
    )
from PyQt5.QtGui import QColor 
from matplotlib.backends.qt_compat import is_pyqt5
if is_pyqt5():
    from matplotlib.backends.backend_qt5agg import (
        FigureCanvas, NavigationToolbar2QT as NavigationToolbar)
else:
    from matplotlib.backends.backend_qt4agg import (
        FigureCanvas, NavigationToolbar2QT as NavigationToolbar)
from matplotlib.figure import Figure
from Ui_PyMzsView import Ui_MainWindow
from pyDataSetting import DataSettingDialog

class PyMzsView(QMainWindow):    
    TABLE_COLS=12
    TABLE_ROWS=4
    TABLE_HEADS=['名称','色','下限','上限','游标1','游标2','名称','色','下限','上限','游标1','游标2']
    TABLE_WS=[0.20,0.02,0.07,0.07,0.07,0.07,0.20,0.02,0.07,0.07,0.07,0.07]
    def __init__(self,parent=None):
        super(PyMzsView,self).__init__(parent)
        self.ui=Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.retranslateUi(self)
        self.fig=None
        self.ax=None
        self.linecolors=[]
        self.lines=[]
        self.cursor1=None
        self.cursor2=None
        self.annotate1=None        
        self.SETTINGDLG=None
        self.dataframe=None
        self.initUi()
        self.initFigure()
        self.initTable()
        self.plotLines()
        self.setupConnect()
    
    def initUi(self):
        #self.setWindowFlags(self.windowFlags() & ~Qt.WindowMaximizeButtonHint)
        #self.setFixedSize(-10+QApplication.desktop().availableGeometry().width()
        #            ,-30+QApplication.desktop().availableGeometry().height())
        self.showMaximized()
        self.statusBar().setVisible(False)
        canvas = FigureCanvas(Figure(tight_layout=True))
        self.ui.verticalLayout.addWidget(canvas)
        self.ui.verticalLayout_3.setContentsMargins(5, 2, 5, 2)
        self.ui.verticalLayout_2.setSpacing(2)
        self.ui.verticalLayout.setSpacing(2)
        self.ui.verticalLayout_3.setStretchFactor(self.ui.verticalLayout,3)
        self.ui.verticalLayout_3.setStretchFactor(self.ui.verticalLayout_2,1)
        self.fig=canvas.figure 
        self.linecolors=['blue','green','red','cyan','magenta','yellow','black','brown']      
        self.SETTINGDLG=DataSettingDialog()
    
    def initFigure(self):
        if(self.fig!=None):
            self.ax=self.fig.subplots()
            ymin,ymax=self.ax.get_ylim()
            xmin,xmax=self.ax.get_xlim()
            self.cursor1,=self.ax.plot([xmin,xmin],[ymin,ymax],lw=0.5,color='red',dashes=[1,2,5,2])
            self.annotate1=self.ax.annotate('x{:.3f}'.format(xmin),(xmin,ymax-0.1))
            self.cursor2,=self.ax.plot([xmin,xmin],[ymin,ymax],lw=0.5,color='blue',dashes=[1,2,5,2])
            self.ax.set_xlim((0,1))
            for i in range(2*self.TABLE_ROWS):
                L,=self.ax.plot([],[],lw=1)
                L.set_color(self.linecolors[i])
                self.lines.append(L)

    def initTable(self):
        self.ui.infoTable.setColumnCount(self.TABLE_COLS)
        self.ui.infoTable.setRowCount(self.TABLE_ROWS)
        self.ui.infoTable.verticalHeader().hide()
        self.ui.infoTable.setSizeAdjustPolicy(QTableWidget.AdjustToContents)
        self.ui.infoTable.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        headFont=self.ui.infoTable.horizontalHeader().font()
        headFont.setBold(True)
        self.ui.infoTable.horizontalHeader().setFont(headFont)
        self.ui.infoTable.horizontalHeader().setStyleSheet("QHeaderView::section{background:skyblue;}")
        self.ui.infoTable.setHorizontalHeaderLabels(self.TABLE_HEADS)
        w=self.ui.infoTable.width()*0.97
        for i in range(self.TABLE_COLS):
            self.ui.infoTable.setColumnWidth(i,w*self.TABLE_WS[i])
        for i in range(self.TABLE_ROWS):
            for j in range(self.TABLE_COLS):
                item=QTableWidgetItem('')
                item.setFlags(item.flags() ^ Qt.ItemIsEditable)
                self.ui.infoTable.setItem(i,j,item)
        for i in range(self.TABLE_ROWS):
            self.ui.infoTable.item(i,1).setData(Qt.BackgroundColorRole,QColor(self.linecolors[i]))
            self.ui.infoTable.item(i,7).setData(Qt.BackgroundColorRole,QColor(self.linecolors[i+self.TABLE_ROWS]))

    def resizeEvent(self,event):
        w=event.size().width()*0.98
        for i in range(self.TABLE_COLS):
            self.ui.infoTable.setColumnWidth(i,w*self.TABLE_WS[i])

    def setupConnect(self):
        self.fig.canvas.mpl_connect('motion_notify_event',self.onFigMove)
        self.ui.action_Open.triggered.connect(self.onSetting)

    def plotLines(self):
        x=numpy.linspace(0,2*numpy.pi,501)
        y=numpy.sin(x)
        y2=numpy.cos(x)
        self.lines[0].set_data(x,y)
        self.lines[1].set_data(x,y2)
        self.ax.set_xlim([0,2*numpy.pi])
        self.ax.set_ylim([-1.2,1.2])

    def loadData(self):
        self.dataframe=pandas.read_csv(self.SETTINGDLG.srcfile,
            sep=self.SETTINGDLG.delimiter,
            skipinitialspace=True,
            skip_blank_lines=True,
            skiprows=self.SETTINGDLG.datastart,
            header=None,
            index_col=False,
            usecols=[self.SETTINGDLG.XColIndex]+self.SETTINGDLG.YColIndexs,
            nrows=self.SETTINGDLG.MAXROW
        )
        nrows,ncols=self.dataframe.shape
        n=min(ncols-1,self.TABLE_ROWS)
        x=self.dataframe.iloc[:,0]
        print(x)
        for i in range(n):
            self.lines[i].set_data(x,self.dataframe.iloc[:,1+i])        
        self.ax.set_ylim([-1.2,5.2])
        self.fig.canvas.draw()
        
    def onFigMove(self,event):
        if event.inaxes != self.ax: return
        x=event.xdata
        ymin,ymax=self.ax.get_ylim()
        if(self.cursor1!=None):
            self.cursor1.set_data([x,x],[ymin,ymax])
            self.annotate1.set_text('x{:.3f}'.format(x))
            self.annotate1.set_x(x)
            self.annotate1.set_y(ymax-0.1)
            self.fig.canvas.draw()
            
    def onSetting(self):
        if(self.SETTINGDLG):
            if(self.SETTINGDLG.exec()==QDialog.Accepted): # what's different between exec_() and exec() of QDialog
                self.loadData()
                self.dataframe

if __name__ == "__main__":    
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #>=Qt5.6:Enables high-DPI scaling in Qt on supported platforms (see also High DPI Displays
    #QApplication.setStyle("fusion")
    app=QApplication(sys.argv)
    w=PyMzsView()
    w.show()
    sys.exit(app.exec_())
    
