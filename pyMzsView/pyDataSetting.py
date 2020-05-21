import pandas
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import (
    QDialog,QFileDialog,QTableWidget,QTableWidgetItem,QRadioButton,
    QListWidgetItem,QMessageBox
    )
from PyQt5.QtGui import QColor
from PyQt5 import QtCore, QtGui, QtWidgets
from Ui_PyDataSetting import Ui_DataSettingDialog

class SelectNamesDialog(QDialog):
    def __init__(self,parent=None):
        super(SelectNamesDialog,self).__init__(parent)
        if(parent is None or parent.mycolumns is None):
            return
        self.setupUi()
        self.retranslateUi()
        self.ListSrcNames.addItems(parent.mycolumns)
        self.ListSrcNames.setSelectionMode(3) #QAbstractItemView::ExtendedSelection==3
        self.ListSelectNames.setSelectionMode(3)
        self.setupConnect() 
        self.selectIndexs=[]  
        self.selectNames=[]  
        self.MAX=parent.TABLE_ROWS 

    def setupUi(self):
        self.setObjectName("SelectNamesDialog")
        self.resize(512, 289)
        self.ListSrcNames = QtWidgets.QListWidget(self)
        self.ListSrcNames.setGeometry(QtCore.QRect(10, 10, 231, 271))
        self.ListSrcNames.setObjectName("ListSrcNames")
        self.BTNSelect = QtWidgets.QPushButton(self)
        self.BTNSelect.setGeometry(QtCore.QRect(250, 80, 41, 17))
        self.BTNSelect.setObjectName("BTNSelect")
        self.BTNRemove = QtWidgets.QPushButton(self)
        self.BTNRemove.setGeometry(QtCore.QRect(250, 110, 41, 17))
        self.BTNRemove.setObjectName("BTNRemove")
        self.BTNClear = QtWidgets.QPushButton(self)
        self.BTNClear.setGeometry(QtCore.QRect(250, 140, 41, 17))
        self.BTNClear.setObjectName("BTNClear")
        self.ListSelectNames = QtWidgets.QListWidget(self)
        self.ListSelectNames.setGeometry(QtCore.QRect(300, 10, 201, 271))
        self.ListSelectNames.setObjectName("ListSelectNames")
        self.buttonBox = QtWidgets.QDialogButtonBox(self)
        self.buttonBox.setGeometry(QtCore.QRect(250, 170, 41, 41))
        self.buttonBox.setOrientation(QtCore.Qt.Vertical)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.retranslateUi()
        QtCore.QMetaObject.connectSlotsByName(self)

    def retranslateUi(self):
        _translate = QtCore.QCoreApplication.translate
        self.setWindowTitle(_translate("SelectNamesDialog", "SelectNames"))
        self.BTNSelect.setText(_translate("SelectNamesDialog", ">>"))
        self.BTNRemove.setText(_translate("SelectNamesDialog", "<<"))
        self.BTNClear.setText(_translate("SelectNamesDialog", "清空"))

    def setupConnect(self):
        self.BTNSelect.clicked.connect(self.onSelect)
        self.BTNRemove.clicked.connect(self.onRemove)
        self.BTNClear.clicked.connect(self.onClear)
        self.buttonBox.accepted.connect(self.onMyaccept)
        self.buttonBox.rejected.connect(self.reject)

    def onSelect(self):
        # n=len(self.selectIndexs)
        # if(n<self.MAX):
        #     items=self.ListSrcNames.selectedItems()[0:self.MAX-n]
        # else:
        #     items=self.ListSrcNames.selectedItems()
        for item in self.ListSrcNames.selectedItems():
            text=item.text()
            index=self.ListSrcNames.row(item)
            if(not index in self.selectIndexs):
                if(len(self.selectIndexs)==self.MAX):
                    self.selectIndexs[self.MAX-1]=index
                    self.selectNames[self.MAX-1]=text
                    self.ListSelectNames.takeItem(self.MAX-1)
                    self.ListSelectNames.addItem(QListWidgetItem(text))
                else:   
                    self.selectIndexs.append(index)
                    self.selectNames.append(text)
                    self.ListSelectNames.addItem(QListWidgetItem(text))

    def onClear(self):
        self.ListSelectNames.clear()
        self.selectIndexs=[]
        self.selectNames=[]

    def onRemove(self):
        for item in self.ListSelectNames.selectedItems():
            index=self.ListSelectNames.row(item)
            self.ListSelectNames.takeItem(index)
            n=len(self.selectIndexs)
            if(index==0):
                self.selectIndexs=self.selectIndexs[1:n]
                self.selectNames=self.selectNames[1:n]
            else:
                self.selectIndexs=self.selectIndexs[0:index]+self.selectIndexs[index+1:n]
                self.selectNames=self.selectNames[0:index]+self.selectNames[index+1:n]
        print(self.selectIndexs)
        print(self.selectNames)

    def onMyaccept(self):
        self.accept()

class DataSettingDialog(QDialog):
    TABLE_COLS=8
    TABLE_ROWS=8
    TABLE_WS=[0.18,0.41,0.09,0.09,0.04,0.04,0.05,0.1]
    COLORS=['blue','green','red','cyan','magenta','black','yellow','brown']
    def __init__(self,parent=None):        
        super(DataSettingDialog,self).__init__(parent)
        self.ui=Ui_DataSettingDialog()
        self.ui.setupUi(self)
        self.ui.retranslateUi(self)
        self.initUi()
        self.setupConnect()        
        self.mycolumns=None
        self.YCols=[]
        self.YColIndexs=[]
        self.XCol='时间'
        self.XColIndex=0 
        self.headstart=0
        self.datastart=1
        self.MAXROW=3000000
        self.delimiter=','
        self.encoding='utf_8'
        self.XType='日期时间'
        self.timeformat='yyyy/mm/dd hh:mm:ss'
        self.YType='实数'
        self.Ysmin=[]
        self.Ysmax=[]
        self.visibles=[]
        self.srcfile=''

    def resizeEvent(self,event):
        w=event.size().width()*0.95
        for i in range(self.TABLE_COLS):
            self.ui.tableWidget.setColumnWidth(i,w*self.TABLE_WS[i])

    def initUi(self):  
        self.ui.tabWidget.setTabText(0,'DataSetting')
        self.ui.tabWidget.setTabText(1,'FigureSetting')    
        self.ui.tableWidget.setSizeAdjustPolicy(QTableWidget.AdjustToContents)
        self.ui.tableWidget.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        headFont=self.ui.tableWidget.horizontalHeader().font()
        headFont.setBold(True)
        self.ui.tableWidget.horizontalHeader().setFont(headFont)
        self.ui.tableWidget.horizontalHeader().setStyleSheet("QHeaderView::section{background:skyblue;}")
        w=self.ui.tableWidget.width()*0.95
        for i in range(self.TABLE_COLS):
            self.ui.tableWidget.setColumnWidth(i,w*self.TABLE_WS[i])
        for i in range(self.TABLE_ROWS):
            self.ui.tableWidget.item(i,5).setData(Qt.BackgroundColorRole,QColor(self.COLORS[i]))    
    
    def setupConnect(self):
        self.ui.RadioUserDefine.clicked.connect(self.onRadioUserDefineClick)
        self.ui.RadioFromFile.clicked.connect(self.onRadioFromFileClick)
        self.ui.buttonBox.accepted.connect(self.onMyaccept)
        self.ui.buttonBox.rejected.connect(self.reject)
        self.ui.BTNFromFile.clicked.connect(self.onFromFile)
        self.ui.BTNSelectYs.clicked.connect(self.onSelectYs)

    def onRadioFromFileClick(self):
        checked=not self.ui.RadioFromFile.isChecked()
        self.processRadioClick(checked)
    
    def onRadioUserDefineClick(self):
        checked=self.ui.RadioUserDefine.isChecked()
        self.processRadioClick(checked)

    def processRadioClick(self,isUserdefined): 
        for i in range(self.TABLE_ROWS):       
            item=self.ui.tableWidget.item(i,0)
            item.setFlags(item.flags() ^ Qt.ItemIsEditable)
            item=self.ui.tableWidget.item(i,1)
            item.setFlags(item.flags() ^ Qt.ItemIsEditable)
        if(isUserdefined):  
            self.ui.BTNFromFile.setEnabled(False)
            self.ui.CBoxXCol.setEnabled(False)
            self.ui.BTNSelectYs.setEnabled(False)
            self.ui.CBoxYCols.setEnabled(False)
            self.ui.CBoxDelimiter.setEnabled(False)
            self.ui.CBoxEncoding.setEnabled(False)
            self.ui.CBoxTimeFormat.setEnabled(False)
            self.ui.SpinMaxRow.setEnabled(False)
            self.ui.SpinDataStart.setEnabled(False)
            self.ui.SpinHeadStart.setEnabled(False)
        else:
            self.ui.BTNFromFile.setEnabled(True)
            self.ui.CBoxXCol.setEnabled(True)
            self.ui.BTNSelectYs.setEnabled(True)
            self.ui.CBoxYCols.setEnabled(True)
            self.ui.CBoxDelimiter.setEnabled(True)
            self.ui.CBoxEncoding.setEnabled(True)
            self.ui.CBoxTimeFormat.setEnabled(True)
            self.ui.SpinMaxRow.setEnabled(True)
            self.ui.SpinDataStart.setEnabled(True)
            self.ui.SpinHeadStart.setEnabled(True)

    def onFromFile(self):
        srcfile,_=QFileDialog.getOpenFileName(self,
            "open data files", 
            "./", 
            "Data Files (*.txt *.csv *.dat);;All Files (*.*)"            
        )
        if(srcfile==''):
            return
        delimiter=self.ui.CBoxDelimiter.currentText()
        if(delimiter=='space'):
            delimiter=' '
        elif(delimiter=='tab'):
            delimiter='\t'        
        src_encoding=self.ui.CBoxEncoding.currentText()
        time_format=self.ui.CBoxTimeFormat.currentText()
        startrow=self.ui.SpinHeadStart.value()-1
        try:
            self.mycolumns=pandas.read_csv(srcfile,
                sep=delimiter,
                encoding=src_encoding,
                skipinitialspace=True,
                skip_blank_lines=True,
                skiprows=startrow,
                header=0,
                nrows=0
            ).columns
            if(not self.mycolumns is None):
                self.ui.EditFileName.setText(srcfile)
                self.ui.CBoxXCol.clear()
                self.ui.CBoxXCol.addItems(self.mycolumns)
        except Exception as e:
            self.mycolumns=None
            QMessageBox.warning(self,"Error!",format(e))

    def onSelectYs(self):
        if(not self.mycolumns is None):
            dlg=SelectNamesDialog(self)
            if(dlg.exec()==QDialog.Accepted): # what's different between exec_() and exec() of QDialog
                if(isinstance(dlg.selectIndexs,list)):
                    self.YColIndexs=dlg.selectIndexs[0:self.TABLE_ROWS]
                    self.YCols=dlg.selectNames[0:self.TABLE_ROWS]
                    self.ui.CBoxYCols.clear()
                    self.ui.CBoxYCols.addItems(self.YCols)
                    for i,element in enumerate(self.YCols):
                        self.ui.tableWidget.item(i,0).setData(Qt.EditRole,element)
                        self.ui.tableWidget.item(i,2).setData(Qt.EditRole,0)
                        self.ui.tableWidget.item(i,3).setData(Qt.EditRole,100)
                    n=len(self.YCols)
                    for i in range(n,self.TABLE_ROWS):
                        self.ui.tableWidget.item(i,0).setData(Qt.EditRole,'')
                        self.ui.tableWidget.item(i,2).setData(Qt.EditRole,'')
                        self.ui.tableWidget.item(i,3).setData(Qt.EditRole,'')

    def onMyaccept(self):
        self.XCol=self.ui.CBoxXCol.currentText()
        self.XColIndex=self.ui.CBoxXCol.currentIndex()
        #self.YCols=self.YCols
        #self.YColIndexs=self.YColIndexs
        self.srcfile=self.ui.EditFileName.text()
        self.headstart=self.ui.SpinHeadStart.value()-1
        self.datastart=self.ui.SpinDataStart.value()-1
        self.MAXROW=self.ui.SpinMaxRow.value()-1
        self.delimiter=self.ui.CBoxDelimiter.currentText()
        self.encoding=self.ui.CBoxEncoding.currentText()
        self.timeformat=self.ui.CBoxTimeFormat.currentText()
        self.XType=self.ui.CBoxXType.currentText()
        self.YType=self.ui.CBoxYType.currentText()
        n=len(self.YColIndexs)
        self.Ysmin=[]
        self.Ysmax=[]
        self.visibles=[]
        for i in range(n):
            self.Ysmin.append(int(self.ui.tableWidget.item(i,2).data(Qt.EditRole)))
            self.Ysmax.append(int(self.ui.tableWidget.item(i,3).data(Qt.EditRole)))
            self.visibles.append(self.ui.tableWidget.item(i,4).data(Qt.EditRole))
        self.accept()

