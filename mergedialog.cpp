#include "mergedialog.h"
#include <QDebug>

const int MergeDialog::m_max_filenum=50;

MergeDialog::MergeDialog(QWidget *parent):
    QDialog(parent),
    m_model(1,1),
    m_head_row(0),
    m_data_start_row(2),
    m_x_column(0),
    m_y_start_column(1),
    m_y_index_infield(0),
    m_head_index_infield(0),
    m_field_length(1),
    m_selected(false)
{
    m_ui.setupUi(this);
    this->setWindowTitle("Merging Files Dialog");
    m_ui.m_fileListView->setModel(&m_model);
    connect(m_ui.m_selectButton,&QPushButton::clicked,this,&MergeDialog::onSelectSourceFiles);
    connect(m_ui.m_saveButton,&QPushButton::clicked,this,&MergeDialog::onMerging);
    connect(m_ui.m_quitButton,&QPushButton::clicked,this,&MergeDialog::close);
}

void MergeDialog::onSelectSourceFiles()
{
    QStringList file_list=QFileDialog::getOpenFileNames(this,tr("Open data file"), "D:\\",
                                  tr("Data Files (*.txt *.csv *.dat);;All Files (*.*)"));
    int rows=file_list.size();
    if(rows>0){
        m_model.setRowCount(rows);
        for(int i=0;i<rows;i++){
            m_model.setData(m_model.index(i,0,QModelIndex()),file_list.at(i));
        }
        m_selected=true;
    }
}

void MergeDialog::onMerging()
{
    if(m_selected){
        QString savefile=QFileDialog::getSaveFileName(this,tr("Open data file"), "D:\\",
                                                      tr("Data Files (*.csv);;All Files (*.*)"));
        if(savefile.trimmed()!=""){
            int button_id=m_ui.m_checkButtonGroup->checkedId();//defultly,1st ID is -2,2nd is -3...
            if(-2==button_id){//column span
                columnMerging(savefile);
            }else if(-3==button_id){//row span
                ;
            }
        }
    }else{
        QMessageBox::warning(this,"Warning","soure files have not been selected");
    }
}

void MergeDialog::columnMerging(QString savefile)
{
    if(m_selected && savefile!=""){
        QString delimiter=",";
        QFile fs_save(savefile);
        if (!fs_save.open(QIODevice::WriteOnly | QIODevice::Text)) {
             QMessageBox::warning(this,"warning",QString("cannot create file:%1").arg(savefile));
             return;
         }
        QTextStream out(&fs_save);
        QFile fs_selects[m_max_filenum];
        QTextStream* stream_selects[m_max_filenum];
        int filenum=0;
        for(int i=0;i<m_model.rowCount() && filenum<m_max_filenum;i++){
            QString selectfile=m_model.data(m_model.index(i,0,QModelIndex())).toString();
            fs_selects[filenum].setFileName(selectfile);
            if(!fs_selects[filenum].open(QIODevice::ReadOnly | QIODevice::Text)){
                QMessageBox::warning(this,"warning",QString("cannot open file:%1").arg(selectfile));
                continue;
            }
            stream_selects[filenum]=new QTextStream(&fs_selects[i]);
            filenum++;
        }
        if(filenum<1){
            QMessageBox::warning(this,"warning","no file be opened");
            return;
        }
        QString out_line="";
        QString in_line;
        QStringList values;
        int row_pos=0;
        for(int i=0;i<filenum;i++){
            for(;row_pos<m_head_row;row_pos++){
                if(!stream_selects[i]->atEnd()){
                    stream_selects[i]->readLine();
                }
            }
            if(stream_selects[i]->atEnd())
                continue;
            in_line=stream_selects[i]->readLine();
            values = QString(in_line).split(delimiter,QString::KeepEmptyParts);
            if(m_x_column<values.size()){
                out_line.append(values.at(m_x_column)+",");
            }
            for(int j=m_y_start_column+m_head_index_infield;j<values.size();j+=m_field_length){
                out_line.append(values.at(j)+",");
            }
        }
        row_pos++;
        if(out_line.size()>=delimiter.size()){
            out_line.chop(delimiter.size());
        }
        out<<out_line<<"\n";
        for(;row_pos<m_data_start_row;row_pos++){
            for(int i=0;i<filenum;i++){
                if(!stream_selects[i]->atEnd())
                   in_line=stream_selects[i]->readLine();
            }
        }
        int end_nums=0;
        while(true){
            out_line="";
            for(int i=0;i<filenum;i++){
                if(stream_selects[i]->atEnd()){
                    end_nums++;
                    continue;
                 }
                in_line=stream_selects[i]->readLine();
                values = QString(in_line).split(delimiter,QString::KeepEmptyParts);
                if(m_x_column<values.size()){
                    out_line.append(values.at(m_x_column)+",");
                }
                for(int j=m_y_start_column+m_y_index_infield;j<values.size();j+=m_field_length){
                    out_line.append(values.at(j)+",");
                }
            }
            if(end_nums==filenum)
                break;
            if(out_line.size()>=delimiter.size()){
                out_line.chop(delimiter.size());
            }
            out<<out_line<<"\n";
        }
        for(int i=0;i<filenum;i++){
           fs_selects[i].close();
           delete stream_selects[i];
        }
        fs_save.close();
    }
}
