#include "mergedialog.h"
#include <QDebug>

const int MergeDialog::M_MAX_FILENUM=50;

MergeDialog::MergeDialog(QWidget *parent):
    QDialog(parent),
    m_model(1,1),
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
            int button_id=m_ui.m_spanCheckGroup->checkedId();//defultly,1st ID is -2,2nd is -3...;
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
        QFile fs_save(savefile);
        if (!fs_save.open(QIODevice::WriteOnly | QIODevice::Text)) {
             QMessageBox::warning(this,"warning",QString("cannot create file:%1").arg(savefile));
             return;
         }
        QTextStream out(&fs_save);
        QFile fs_selects[M_MAX_FILENUM];
        QTextStream* stream_selects[M_MAX_FILENUM];
        int filenum=0;
        for(int i=0;i<m_model.rowCount() && filenum<M_MAX_FILENUM;i++){
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
        int head_row=m_ui.m_headrowEdit->text().toInt()-1;
        int data_startrow=m_ui.m_dataStartRowEdit->text().toInt()-1;
        int x_column=m_ui.m_XColumnEdit->text().toInt()-1;
        int head_startcolumn=m_ui.m_headStartColumnEdit->text().toInt()-1;
        int ycolumn_step=m_ui.m_stepEdit->text().toInt();
        int ystart_column=m_ui.m_YStartColumnEdit->text().toInt()-1;
        int delimiter_check=m_ui.m_delimiterCheckGroup->checkedId();
        QString delimiter=",";
        if(delimiter_check==-3)
            delimiter=";";
        else if(delimiter_check==-4)
            delimiter=" ";
        else if(delimiter_check==-5)
            delimiter="\t";
        else if(delimiter_check==-6){
            if(m_ui.m_otherEdit->text().size()>=1)
                delimiter=m_ui.m_otherEdit->text();
        }
        int row_pos=0;
        int ycount_list[M_MAX_FILENUM];
        for(int i=0;i<filenum;i++){
            for(;row_pos<head_row;row_pos++){
                if(!stream_selects[i]->atEnd()){
                    stream_selects[i]->readLine();
                }
            }
            if(stream_selects[i]->atEnd())
                continue;
            in_line=stream_selects[i]->readLine();
            values = QString(in_line).split(delimiter,QString::KeepEmptyParts);
            if(x_column<values.size()){
                out_line.append(values.at(x_column)+delimiter);
            }
            ycount_list[i]=1;
            for(int j=head_startcolumn;j<values.size();j+=ycolumn_step){
                out_line.append(values.at(j)+delimiter);
                ++ycount_list[i];
            }
        }
        row_pos++;
        if(out_line.size()>=delimiter.size()){
            out_line.chop(delimiter.size());
        }
        out<<out_line<<"\n";
        for(;row_pos<data_startrow;row_pos++){
            for(int i=0;i<filenum;i++){
                if(!stream_selects[i]->atEnd())
                   in_line=stream_selects[i]->readLine();
            }
        }
        bool flag=true;
        while(flag){
            out_line="";
            for(int i=0;i<filenum;i++){
                int num=0;
                if(stream_selects[i]->atEnd()){
                    flag=false;
                }else{
                    flag=true;
                    in_line=stream_selects[i]->readLine();
                    values = QString(in_line).split(delimiter,QString::KeepEmptyParts);
                    if(x_column<values.size()){
                        out_line.append(values.at(x_column)+delimiter);
                        ++num;
                    }
                    for(int j=ystart_column;j<values.size();j+=ycolumn_step){
                        out_line.append(values.at(j)+delimiter);
                        ++num;
                    }
                }
                if(num<ycount_list[i]){
                    out_line.append(delimiter.repeated(ycount_list[i]-num));
                }
            }
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
