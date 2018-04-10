#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mzsviewglobal.h"
#include "ui_mainwindow.h"
#include "comboboxdelegate.h"
#include "checkdelegate.h"
#include "colordelegate.h"
#include "mergedialog.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QLineSeries>
#include <QChart>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QHash>
#include <QFileDialog>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void initModel();
    int updateView();
    int getTags();

public slots:
    void setTagFieldIndex(int fieldindex_list[],int &n);

private slots:
    void getCursorValue(int cursorColumn,QList<double> y_list);
    void setTagValue(int rowIndex,const QString& tag);
    void modelDataChanged(const QModelIndex &topLeft);
    void openFile();
    void mergeFiles();

signals:
    void dataChangedSignal(const int row_index,const QString &tag,int tag_field_index,
                          int select_state,int phase_state,const QString &color,double min,double max);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow m_ui;
    QStandardItemModel* m_model;
    QChart m_chart;
    int m_rowCount;
    int m_colCount;
    QHash<QString,int> m_tags_hash;
    QString m_datafile;

private://const static member

};

#endif // MAINWINDOW_H
