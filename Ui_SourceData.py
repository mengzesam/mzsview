# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'e:\project\TurbineTest\SourceData.ui'
#
# Created by: PyQt5 UI code generator 5.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_SourceData(object):
    def setupUi(self, SourceData):
        SourceData.setObjectName("SourceData")
        SourceData.resize(503, 388)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(SourceData)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.tableView = QtWidgets.QTableView(SourceData)
        self.tableView.setObjectName("tableView")
        self.verticalLayout.addWidget(self.tableView)
        self.verticalLayout_2.addLayout(self.verticalLayout)
        self.actionImport = QtWidgets.QAction(SourceData)
        self.actionImport.setObjectName("actionImport")
        self.actionClearAll = QtWidgets.QAction(SourceData)
        self.actionClearAll.setObjectName("actionClearAll")

        self.retranslateUi(SourceData)
        QtCore.QMetaObject.connectSlotsByName(SourceData)

    def retranslateUi(self, SourceData):
        _translate = QtCore.QCoreApplication.translate
        SourceData.setWindowTitle(_translate("SourceData", "Source Data"))
        self.actionImport.setText(_translate("SourceData", "Import Data"))
        self.actionImport.setToolTip(_translate("SourceData", "import source data"))
        self.actionClearAll.setText(_translate("SourceData", "Clear All"))
        self.actionClearAll.setToolTip(_translate("SourceData", "clear all data"))

