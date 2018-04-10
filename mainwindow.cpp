#include "mainwindow.h"
#include <QDebug>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_model(new QStandardItemModel),
    m_rowCount(mzsview::ROWS),
    m_colCount(mzsview::COLUMNS),
    m_tags_hash(QHash<QString,int>()),
    m_datafile("d:\\data.csv")
{
    m_ui.setupUi(this);
    initModel();
    connect(m_ui.action_Open,&QAction::triggered,this,&MainWindow::openFile);
    connect(m_ui.actionEx_it,&QAction::triggered,this,&MainWindow::close);
    connect(m_ui.actionNext,&QAction::triggered,m_ui.plotview,&PlotView::nextTime);
    connect(m_ui.actionPrev,&QAction::triggered,m_ui.plotview,&PlotView::prevTime);
    connect(m_ui.actionTimespan,&QAction::triggered,m_ui.plotview,&PlotView::setTimespan);
    connect(m_ui.action_Merge_Files ,&QAction::triggered,this,&MainWindow::mergeFiles);
    connect(m_ui.plotview,&PlotView::cursorValueChanged,this,&MainWindow::getCursorValue);
    connect(m_ui.plotview,&PlotView::tagValueChanged,this,&MainWindow::setTagValue);
    connect(m_model,&QStandardItemModel::dataChanged,this,&MainWindow::modelDataChanged);
    connect(this,&MainWindow::dataChangedSignal,m_ui.plotview,&PlotView::modelDataChanged);
    connect(m_ui.plotview,&PlotView::getTagFieldIndex,this,&MainWindow::setTagFieldIndex);
}

void MainWindow::initModel()
{
    m_model->setRowCount(mzsview::ROWS);
    m_model->setColumnCount(mzsview::COLUMNS);
    for(int i=0;i<m_colCount;i++){
        m_model->setHeaderData(i,Qt::Horizontal,mzsview::HEADS[i]);
    }
    QModelIndex index;
    QStandardItem *item;
    for(int i=0;i<m_rowCount;i++){
        index=m_model->index(i,mzsview::CURSOR1_COLUMN,QModelIndex());
        item = m_model->itemFromIndex(index);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        index=m_model->index(i,mzsview::CURSOR2_COLUMN,QModelIndex());
        item = m_model->itemFromIndex(index);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        index=m_model->index(i,mzsview::SELECT_COLUMN,QModelIndex());
        m_model->setData(index,mzsview::DEFAULT_SELECT_STATE, Qt::EditRole);
        index=m_model->index(i,mzsview::PHASE_COLUMN,QModelIndex());
        m_model->setData(index,mzsview::DEFAULT_PHASE_STATE, Qt::EditRole);
        index=m_model->index(i,mzsview::COLOR_COLUMN,QModelIndex());
        m_model->setData(index, QVariant(QColor(mzsview::DEFAULT_COLORS[i])), Qt::BackgroundRole);
        index=m_model->index(i,mzsview::MIN_COLUMN,QModelIndex());
        m_model->setData(index, mzsview::DEFAULT_MINS[i], Qt::EditRole);
        index=m_model->index(i,mzsview::MAX_COLUMN,QModelIndex());
        m_model->setData(index,mzsview::DEFAULT_MAXS[i], Qt::EditRole);
    }
    m_ui.tableView->setModel(m_model);    
    ComboboxDelegate* cbdelegate=new ComboboxDelegate;
    m_ui.tableView->setItemDelegateForColumn(mzsview::TAG_COLUMN,cbdelegate);
    CheckDelegate* checkdelegate=new CheckDelegate;
    m_ui.tableView->setItemDelegateForColumn(mzsview::PHASE_COLUMN,checkdelegate);
    m_ui.tableView->setItemDelegateForColumn(mzsview::SELECT_COLUMN,checkdelegate);
    for(int i=0;i<mzsview::ROWS;i++){
        m_ui.tableView->openPersistentEditor(m_model->index(i,mzsview::TAG_COLUMN,QModelIndex()));
        m_ui.tableView->openPersistentEditor(m_model->index(i,mzsview::SELECT_COLUMN,QModelIndex()));
        m_ui.tableView->openPersistentEditor(m_model->index(i,mzsview::PHASE_COLUMN,QModelIndex()));
    }
    ColorDelegate* colordelegate=new ColorDelegate;
    m_ui.tableView->setItemDelegateForColumn(mzsview::COLOR_COLUMN,colordelegate);
}

int MainWindow::updateView(){
    m_ui.plotview->setDatafile(m_datafile);
    m_ui.plotview->loadChart();
    return 0;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resize(event->size());
    int height=event->size().height();
    int width=event->size().width();
    m_ui.plotview->setGeometry(QRect(mzsview::PADDING, 0, width-2*mzsview::PADDING,
                                      height-mzsview::TABLE_HEIGHT-mzsview::PADDING));
    m_ui.tableView->setGeometry(QRect(mzsview::PADDING, height-mzsview::TABLE_HEIGHT-mzsview::PADDING,
                                      width-2*mzsview::PADDING, mzsview::TABLE_HEIGHT));
    int table_width=m_ui.tableView->size().width();
    for(int i=0;i<mzsview::COLUMNS;i++){
        m_ui.tableView->setColumnWidth(i,int(table_width*mzsview::COLUMN_WIDTH[i]/100.0));
    }
    int row_height=m_ui.tableView->size().height()/(m_rowCount+3);
    for(int i=0;i<m_rowCount;i++){
        m_ui.tableView->setRowHeight(i,row_height);
    }
    m_ui.tableView->horizontalHeader()->setStretchLastSection(true);
    QWidget::resizeEvent(event);
}

void MainWindow::getCursorValue(int cursorColumn,QList<double> y_list)
{
    double y;
    for(int i=0;i<y_list.size();i++){
        y=y_list.at(i);
        m_model->setData(m_model->index(i,cursorColumn,QModelIndex()),y);
    }
}

int MainWindow::getTags()
{
    QFile datafile(m_datafile);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text)) {
         return -1;
     }
    QTextStream stream(&datafile);
    QString line = (stream.readLine()).trimmed();
    QStringList values;
    if(line.isEmpty()){
        datafile.close();
        return -1;
    }
    values = line.split(mzsview::DATA_DELIMITER, QString::SkipEmptyParts);
    if(values.size()<2){
        datafile.close();
        return -1;
    }
    m_tags_hash.clear();
    for(int i=0;i<values.size();i++){
        m_tags_hash.insert(values.at(i),i);
    }
    values.pop_front();
    ComboboxDelegate* cbdelegate=qobject_cast<ComboboxDelegate*>
            (m_ui.tableView->itemDelegateForColumn(mzsview::TAG_COLUMN));

    for(int i=0;i<mzsview::ROWS;i++){
        m_ui.tableView->closePersistentEditor(m_model->index(i,mzsview::TAG_COLUMN,QModelIndex()));
    }
    cbdelegate->setItemlist(values);
    QModelIndex index;
    QStandardItem *item;
    for(int i=0;i<values.size() && i<mzsview::ROWS;i++){
        index=m_model->index(i,mzsview::TAG_COLUMN,QModelIndex());
        item=m_model->itemFromIndex(index);
        item->setFlags(item->flags()|Qt::ItemIsEditable);
        m_ui.tableView->openPersistentEditor(m_model->index(i,mzsview::TAG_COLUMN,QModelIndex()));
    }
    for(int i=values.size();i<mzsview::ROWS;i++){
        index=m_model->index(i,mzsview::TAG_COLUMN,QModelIndex());
        item=m_model->itemFromIndex(index);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_model->setData(index,"",Qt::EditRole);
    }
    return 0;
}

void MainWindow::setTagValue(int rowIndex,const QString& tag)
{
    if(rowIndex<m_rowCount){
        QModelIndex index=m_model->index(rowIndex,mzsview::TAG_COLUMN,QModelIndex());
        m_model->setData(index,tag,Qt::EditRole);
    }
}

void MainWindow::modelDataChanged(const QModelIndex &topLeft)
{
    if(mzsview::CURSOR1_COLUMN!=topLeft.column()
            &&mzsview::CURSOR2_COLUMN!=topLeft.column())
    {
       int row_index=topLeft.row();
       QString tag;
       int tag_field_index=-1;
       int select_state;
       int phase_state;
       QString color;
       double min;
       double max;
       QModelIndex index=m_model->index(row_index,mzsview::TAG_COLUMN,QModelIndex());
       tag=index.data().toString();
       if(m_tags_hash.find(tag)==m_tags_hash.end())
            return;//TAG not exist,not emit datachanged signal to plotview
       tag_field_index=m_tags_hash[tag];
       index=m_model->index(row_index,mzsview::SELECT_COLUMN,QModelIndex());
       select_state=index.data().toInt();
       index=m_model->index(row_index,mzsview::PHASE_COLUMN,QModelIndex());
       phase_state=index.data().toInt();
       index=m_model->index(row_index,mzsview::COLOR_COLUMN,QModelIndex());
       color=index.data(Qt::BackgroundRole).toString();
       index=m_model->index(row_index,mzsview::MIN_COLUMN,QModelIndex());
       min=index.data().toDouble();
       index=m_model->index(row_index,mzsview::MAX_COLUMN,QModelIndex());
       max=index.data().toDouble();
       emit dataChangedSignal(row_index,tag,tag_field_index,select_state,phase_state,
                              color,min,max);
    }
}

void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open data file"), "D:\\", tr("Data Files (*.txt *.csv *.dat);;All Files (*.*)"));//,
                                                    //nullptr,QFileDialog::DontUseNativeDialog);
     if(filename.trimmed()=="")
        return;
    m_datafile=filename;
    getTags();
    this->setWindowTitle(QString("%1@mzsview").arg(m_datafile));
    updateView();
}

void MainWindow::mergeFiles()
{
    MergeDialog mergedialog(this);//=new MergeDialog(this);
    mergedialog.exec();
}

void MainWindow::setTagFieldIndex(int fieldindex_list[], int &n)
{
    n=n<m_tags_hash.size()?n:m_tags_hash.size();
    for(int i=0;i<n;i++){
        QModelIndex index=m_model->index(i,mzsview::TAG_COLUMN,QModelIndex());
        QString tag=index.data().toString();
        fieldindex_list[i]=m_tags_hash[tag];
    }
}
