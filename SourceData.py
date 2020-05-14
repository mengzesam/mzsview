# -*- coding: utf-8 -*-
#如使用PySide2,import时将PyQt5替换成PySide2
import sys
import numpy
import pandas
import re
from PyQt5.QtCore import Qt,QCoreApplication,QModelIndex
from PyQt5.QtWidgets import QApplication,QWidget,QMenu,QFileDialog,QMessageBox
from PyQt5.QtGui import QStandardItemModel
from Ui_SourceData import Ui_SourceData

class SourceData(QWidget):
    MAXROWS=20000
    MAXCOLS=600
    DEFAULTROWS=100
    DEFAULTCOLS=50
    def __init__(self,parent=None):
        super(SourceData,self).__init__(parent)
        self.ui=Ui_SourceData()
        self.ui.setupUi(self)
        self.ui.retranslateUi(self)
        self.initMembers()
        self.setContextMenuPolicy(Qt.DefaultContextMenu)
        self.setupConnect()
        self.setWindowTitle("test source data")
        self.showMaximized()    

    def initMembers(self):
        self.m_data_encoding="utf-8"
        self.m_data_delimiter=","
        self.m_data=None
        self.m_model=QStandardItemModel(self.DEFAULTROWS,self.DEFAULTCOLS,self)
        self.ui.tableView.setModel(self.m_model)

    def contextMenuEvent(self,event):
        cmenu = QMenu(self)    
        cmenu.addAction(self.ui.actionImport)
        cmenu.addAction(self.ui.actionClearAll)
        cmenu.addSeparator()
        action = cmenu.exec_(self.mapToGlobal(event.pos()))

    def setupConnect(self):
        self.ui.actionImport.triggered.connect(
            self.onImportData
        )
        self.ui.actionClearAll.triggered.connect(
            self.onClearAll
        )
    
    def onClearAll(self):
        if(self.m_model):
            self.m_data=None
            self.m_model.clear()
            self.m_model.setRowCount(self.DEFAULTROWS)
            self.m_model.setColumnCount(self.DEFAULTCOLS)

    def onImportData(self):
        filename,_=QFileDialog.getOpenFileName(self,
            "select data file", 
            "./", 
            "Data Files (*.txt *.csv *.dat);;All Files (*.*)"   
        )
        if(len(filename)>0):
            try:
                self.importData(filename)
            except Exception as e:
                QMessageBox.warning(self,"Error!",format(e))
        
    def importData(self,datafile):
        self.m_data=pandas.read_csv(datafile,
            sep=self.m_data_delimiter,
            encoding=self.m_data_encoding,
            index_col=0
        )
        row_count,col_count=self.m_data.shape
        rr=row_count+1 if(row_count<self.MAXROWS) else self.MAXROWS 
        cc=col_count+1 if(col_count<self.MAXCOLS) else self.MAXCOLS 
        self.m_model.setRowCount(rr)
        self.m_model.setColumnCount(cc)
        if(not self.m_data.empty and self.m_model):
            self.m_model.setData(self.m_model.index(0,0,QModelIndex()),self.m_data.index.name)
            r=1
            for idx in self.m_data.index:
                self.m_model.setData(self.m_model.index(r,0,QModelIndex()),idx)
                r=r+1
            c=1
            for col in self.m_data.columns:
                self.m_model.setData(self.m_model.index(0,c,QModelIndex()),col)
                c=c+1
            for c in range(col_count):
                for r in range(row_count):
                    value=self.m_data.iloc[r,c]
                    if(not isinstance(value,str)):                        
                        value=numpy.float(value)
                    else:
                        value=re.split("[,\t ]",value)[0]                       
                        value=numpy.float(value)
                    self.m_model.setData(self.m_model.index(r+1,c+1,QModelIndex()),value,Qt.EditRole)

if __name__=='__main__':
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #>=Qt5.6:Enables high-DPI scaling in Qt on supported platforms (see also High DPI Displays
    app=QApplication(sys.argv)
    w=SourceData()
    w.show()
    sys.exit(app.exec_())
