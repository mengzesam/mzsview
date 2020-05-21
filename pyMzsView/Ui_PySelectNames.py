# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pySelectNames.ui'
#
# Created by: PyQt5 UI code generator 5.11.3
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_SelectNamesDialog(object):
    def setupUi(self, SelectNamesDialog):
        SelectNamesDialog.setObjectName("SelectNamesDialog")
        SelectNamesDialog.resize(512, 289)
        self.ListSrcNames = QtWidgets.QListWidget(SelectNamesDialog)
        self.ListSrcNames.setGeometry(QtCore.QRect(10, 10, 231, 271))
        self.ListSrcNames.setObjectName("ListSrcNames")
        self.BTNSelect = QtWidgets.QPushButton(SelectNamesDialog)
        self.BTNSelect.setGeometry(QtCore.QRect(250, 80, 41, 17))
        self.BTNSelect.setObjectName("BTNSelect")
        self.BTNRemove = QtWidgets.QPushButton(SelectNamesDialog)
        self.BTNRemove.setGeometry(QtCore.QRect(250, 110, 41, 17))
        self.BTNRemove.setObjectName("BTNRemove")
        self.BTNClear = QtWidgets.QPushButton(SelectNamesDialog)
        self.BTNClear.setGeometry(QtCore.QRect(250, 140, 41, 17))
        self.BTNClear.setObjectName("BTNClear")
        self.ListSelectNames = QtWidgets.QListWidget(SelectNamesDialog)
        self.ListSelectNames.setGeometry(QtCore.QRect(300, 10, 201, 271))
        self.ListSelectNames.setObjectName("ListSelectNames")
        self.buttonBox = QtWidgets.QDialogButtonBox(SelectNamesDialog)
        self.buttonBox.setGeometry(QtCore.QRect(250, 170, 41, 41))
        self.buttonBox.setOrientation(QtCore.Qt.Vertical)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")

        self.retranslateUi(SelectNamesDialog)
        QtCore.QMetaObject.connectSlotsByName(SelectNamesDialog)

    def retranslateUi(self, SelectNamesDialog):
        _translate = QtCore.QCoreApplication.translate
        SelectNamesDialog.setWindowTitle(_translate("SelectNamesDialog", "SelectNames"))
        self.BTNSelect.setText(_translate("SelectNamesDialog", ">>"))
        self.BTNRemove.setText(_translate("SelectNamesDialog", "<<"))
        self.BTNClear.setText(_translate("SelectNamesDialog", "清空"))

