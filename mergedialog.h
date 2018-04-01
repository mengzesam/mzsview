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
    int m_head_row;
    int m_data_start_row;
    int m_x_column;
    int m_y_start_column;
    int m_y_index_infield;
    int m_head_index_infield;
    int m_field_length;
    bool m_selected;

private://const static member
    const static int m_max_filenum;
};

#endif // MERGEDIALOG_H
