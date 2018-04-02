#ifndef MERGEDIALOG_H
#define MERGEDIALOG_H
#include "ui_mergedialog.h"
#include <QDialog>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QMessageBox>


namespace Ui {
class MergeDialog;
}

class MergeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MergeDialog(QWidget *parent = 0);

private slots:
    void onSelectSourceFiles();
    void onMerging();

private:
    void columnMerging(QString savefile);
    void rowMerging(QString savefile);

private:
    Ui::MergeDialog m_ui;
    QStandardItemModel m_model;
    bool m_selected;

private://const static member
    const static int M_MAX_FILENUM;
};

#endif // MERGEDIALOG_H
