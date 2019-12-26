# -*- coding: utf-8 -*-
#如使用PySide2,import时将PyQt5替换成PySide2
import sys
import pandas
from PyQt5.QtCore import Qt,QCoreApplication,QModelIndex
from PyQt5.QtWidgets import QApplication,QDialog,QMessageBox,QFileDialog
from PyQt5.QtGui import QStandardItemModel
from Ui_MergeDialog import Ui_MergeDialog

class MergeDialog(QDialog):
    MAX_FILENUM=50
    MAX_READLINE=10000
    REPEAT_SUFFIX="_suffix_#$:@%|&*(!@#$:@%|" #设置为现实中很难出现的字符串
    
    def __init__(self,parent=None):
        super(MergeDialog,self).__init__(parent)
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
        self.ui.m_offsetDataHeadEdit.setToolTip(u"数据相对字头的列偏移量，1为右偏1，-1为左偏1")

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
                    try:
                        self.mergingByColumn(save_file)
                    except Exception as e:
                        QMessageBox.warning(self,"Error!",format(e))
                elif(self.ui.m_rowSpanCheck.isChecked()):
                    self.mergingByRow(save_file)
            else:
                QMessageBox.warning(self,"Warning!","please select merging file!")
        else:
            QMessageBox.warning(self,"Warning","source files have not been selected")

    def mergingByColumn(self,save_file):
        head_row=self.ui.m_headrowEdit.value()-1
        data_startrow=self.ui.m_dataStartRowEdit.value()-1
        x_column=self.ui.m_XColumnEdit.value()-1
        ycolumn_step=self.ui.m_stepEdit.value()
        ystart_column=self.ui.m_YStartColumnEdit.value()-1
        offset_data_head=self.ui.m_offsetDataHeadEdit.value() #数据相对字头的列偏移量，1为右偏1，-1为左偏1
        src_encoding=self.ui.m_srcEncodingCBBox.currentText()
        dest_encoding=self.ui.m_destEncodingCBBox.currentText()
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
        total_data=pandas.DataFrame()
        for i in range(files_nume):
            filename=self.m_model.data(self.m_model.index(i,0,QModelIndex()))
            mycolumns=pandas.read_csv(filename,
                sep=delimiter,
                encoding=src_encoding,
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
                myindex_col=0 
            data=pandas.read_csv(filename,
                sep=delimiter,
                encoding=src_encoding,
                skipinitialspace=True,
                skip_blank_lines=True,
                skiprows=data_startrow,
                header=None,
                index_col=myindex_col,
                usecols=use_cols,
                nrows=self.MAX_READLINE
            )
            if(len(use_cols)>1):#不大于1列不执行
                j=use_cols[0]
                use_cols=use_cols[1:] 
                if(offset_data_head!=0    #以下：只有加上偏移后索引不超出范围才有限
                    and use_cols[0]-offset_data_head>=0 
                    and use_cols[-1]-offset_data_head<len(mycolumns)
                ):
                    use_cols=[i-offset_data_head for i in use_cols]                
                if(not self.ui.m_XasIndexCheck.isChecked()):
                    use_cols.insert(0,j) 
                data.columns=mycolumns[use_cols]
            data.index.name="index"
            if(self.ui.m_XisDateTimeCheck.isChecked()):
                if(self.ui.m_XasIndexCheck.isChecked()):
                    data.index.name="time"
                mydatetime_format=self.datetime_formats[
                    self.ui.m_datetimeFormatCBBox.currentText()
                ]
                data.index=pandas.to_datetime(data.index,format=mydatetime_format)
            if(i==0):
                total_data=data
            else:
                total_data=total_data.join(data,lsuffix=self.REPEAT_SUFFIX,rsuffix=self.REPEAT_SUFFIX)
        mycolumns=[s.replace(self.REPEAT_SUFFIX,"") for s in total_data.columns]
        total_data.columns=mycolumns
        total_data.to_csv(path_or_buf=save_file,sep=delimiter,encoding=dest_encoding)
        #print(total_data)

    def mergingByRow(self,save_file):
        QMessageBox.warning(self,"Warning!",save_file)        


if __name__=='__main__':
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling) #>=Qt5.6:Enables high-DPI scaling in Qt on supported platforms (see also High DPI Displays
    #QApplication.setStyle("fusion")
    app=QApplication(sys.argv)
    w=MergeDialog()
    w.show()
    sys.exit(app.exec_())


