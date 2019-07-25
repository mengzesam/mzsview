# -*- coding: utf-8 -*-

import sys
import pandas
from PyQt5.QtCore import Qt,QCoreApplication,QModelIndex
from PyQt5.QtWidgets import QApplication,QDialog,QMessageBox,QFileDialog
from PyQt5.QtGui import QStandardItemModel
from Ui_MergeDialog import Ui_MergeDialog

class MergeDialog(QDialog):
    MAX_FILENUM=50
    MAX_READLINE=10000
    
    def __init__(self,parent=None):
        super(QDialog,self).__init__(parent)
        self.datetime_formats={
            'yyyy/mm/dd hh:mm:ss':'%Y/%m/%d %H:%M:%S',
            'yyyy/mm/dd h:m:s':'%Y/%m/%d %H:%M:%S',
            'yyyy/mm/dd hh:mm':'%Y/%m/%d %H:%M',
            'yyyy/mm/dd h:m':'%Y/%m/%d %H:%M',
            'yy/mm/dd hh:mm:ss':'%y/%m/%d %H:%M:%S',
            'yy/mm/dd h:m:s':'%y/%m/%d %H:%M:%S',
            'yy/mm/dd hh:mm':'%y/%m/%d %H:%M',
            'yy/mm/dd h:m':'%y/%m/%d %H:%M',
            'yyyy-mm-dd hh:mm:ss':'%Y-%m-%d %H:%M:%S',
            'yyyy-mm-dd h:m:s':'%Y-%m-%d %H:%M:%S',
            'yyyy-mm-dd hh:mm':'%Y-%m-%d %H:%M',
            'yyyy-mm-dd h:m':'%Y-%m-%d %H:%M',
            'yy-mm-dd hh:mm:ss':'%y-%m-%d %H:%M:%S',
            'yy-mm-dd h:m:s':'%y-%m-%d %H:%M:%S',
            'yy-mm-dd hh:mm':'%y-%m-%d %H:%M',
            'yy-mm-dd h:m':'%y-%m-%d %H:%M'
        }
        self.m_selected=False
        self.ui=Ui_MergeDialog()
        self.ui.setupUi(self)
        self.ui.retranslateUi(self)
        self.initUi()
        self.setWindowTitle("Merging Source Files")
        self.m_model=QStandardItemModel(1,1)
        self.ui.m_fileListView.setModel(self.m_model)
        self.setupConnect()
    
    def initUi(self):
        coding_list=["utf_8","utf_16","latin_1","cp65001","ascii",
            "gb2312","gb18030","gbk","big5","big5hkscs"
        ]
        self.ui.m_srcEncodingCBBox.addItems(coding_list)
        self.ui.m_srcEncodingCBBox.setCurrentIndex(0)
        self.ui.m_destEncodingCBBox.addItems(coding_list)
        self.ui.m_destEncodingCBBox.setCurrentIndex(0)
        self.ui.m_datetimeFormatCBBox.addItems([key for key in self.datetime_formats])
        self.ui.m_datetimeFormatCBBox.setCurrentIndex(0)


    def setupConnect(self):
        self.ui.m_selectButton.clicked.connect(self.onSelectSrcFiles)
        self.ui.m_saveButton.clicked.connect(self.onMerging)
        self.ui.m_quitButton.clicked.connect(self.close)
        self.ui.m_XisDateTimeCheck.stateChanged.connect(
            lambda state:self.ui.m_datetimeFormatCBBox.setEnabled(state)
        )
        self.ui.m_rowSpanCheck.stateChanged.connect(
            lambda state:(self.ui.m_headrowEdit.setEnabled(not state) 
                ,self.ui.m_dataStartRowEdit.setEnabled(not state)
                ,self.ui.m_XColumnEdit.setEnabled(not state)
                ,self.ui.m_YStartColumnEdit.setEnabled(not state)
                ,self.ui.m_stepEdit.setEnabled(not state)              
                ,self.ui.m_offsetDataHeadEdit.setEnabled(not state)
            )
        )


    def onSelectSrcFiles(self):
        (file_list,_)=QFileDialog.getOpenFileNames(self,
                "open data files", 
                "./", 
                "Data Files (*.txt *.csv *.dat);;All Files (*.*)"            
        )
        if(len(file_list)>0):
            self.m_model.setRowCount(len(file_list))
            i=0
            for filename in file_list:
                self.m_model.setData(self.m_model.index(i,0,QModelIndex()),filename)
                i=i+1
            self.m_selected=True

    def onMerging(self):
        if(self.m_selected):
            save_file,_=QFileDialog.getSaveFileName(
                self,
                "Open data file", 
                "./",
                "Data Files (*.csv);;All Files (*.*)"
            )
            if(save_file!=""):
                if(self.ui.m_colmunSpanCheck.isChecked()):#column span
                    self.columnMerging(save_file)
                elif(self.ui.m_rowSpanCheck.isChecked()):
                    self.rowMerging(save_file)
            else:
                QMessageBox.warning(self,"Warning!","please select merging file!")
        else:
            QMessageBox.warning(self,"Warning","source files have not been selected")

    def columnMerging(self,save_file):
        head_row=self.ui.m_headrowEdit.value()-1
        data_startrow=self.ui.m_dataStartRowEdit.value()-1
        x_column=self.ui.m_XColumnEdit.value()-1
        ycolumn_step=self.ui.m_stepEdit.value()
        ystart_column=self.ui.m_YStartColumnEdit.value()-1
        offset_data_head=self.ui.m_offsetDataHeadEdit.value() #数据相对字头的列偏移量，1为右偏1，-1为左偏1
        src_coding=self.ui.m_srcEncodingCBBox.currentText()
        dest_coding=self.ui.m_destEncodingCBBox.currentText()
        delimiter=","
        if(self.ui.m_semiCheck.isChecked()):
            delimiter=";"
        elif(self.ui.m_spaceCheck.isChecked()):
            delimiter=" "
        elif(self.ui.m_tabCheck.isChecked()):
            delimiter="\t"
        elif(self.ui.m_otherCheck.isChecked()):
            ss=self.ui.m_otherEdit.text()
            if(len(ss)>=1):
                delimiter=ss
        files_nume=self.m_model.rowCount()
        for i in range(files_nume):
            filename=self.m_model.data(self.m_model.index(i,0,QModelIndex()))
            mycolumns=pandas.read_csv(filename,
                sep=delimiter,
                encoding=src_coding,
                skipinitialspace=True,
                skip_blank_lines=True,
                skiprows=head_row,
                header=0,
                nrows=0
            ).columns
            use_cols=[i for i in range(ystart_column,len(mycolumns),ycolumn_step)]
            use_cols.insert(0,x_column)
            myindex_col=None
            if(self.ui.m_XasIndexCheck.isChecked()):
                myindex_col=x_column 
            data=pandas.read_csv(filename,
                sep=delimiter,
                encoding=src_coding,
                skipinitialspace=True,
                skip_blank_lines=True,
                skiprows=data_startrow,
                header=None,
                index_col=myindex_col,
                usecols=use_cols,
                nrows=self.MAX_READLINE
            )
            data.index.name="time"
            if(self.ui.m_XasIndexCheck.isChecked()):
                use_cols=use_cols[1:]
            data.columns=mycolumns[use_cols]
            if(self.ui.m_XisDateTimeCheck.isChecked()):
                mydatetime_format=self.datetime_formats[
                    self.ui.m_datetimeFormatCBBox.currentText()
                ]
                data.index=pandas.to_datetime(data.index,format=mydatetime_format)
            print(data)

    def rowMerging(self,save_file):
        QMessageBox.warning(self,"Warning!",save_file)        


if __name__=='__main__':
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #>=Qt5.6:Enables high-DPI scaling in Qt on supported platforms (see also High DPI Displays
    app=QApplication(sys.argv)
    w=MergeDialog()
    w.show()
    sys.exit(app.exec_())


