#include "plotview.h"
#include <QDebug>
#include <QTime>

PlotView::PlotView(QWidget *parent) :
      QChartView(parent),
      m_chart(new QChart),
      m_datafile(QString("")),
      m_cursor_left(new LineType()),
      m_cursor_right(new LineType()),
      m_cursor_picked(NULL),
      m_status(new QGraphicsSimpleTextItem(m_chart)),
      m_minY(0),
      m_maxY(100),
      m_valid_lines(-1),
      m_isinitchart(false),
      m_isloadchart(false),
      m_filevalid(false),
      m_pickedIndex(0),
      m_prePickedIndex(0),
      m_current_lefttime(0),
      m_time_span(7200000),
      m_leftpoint_index(0),
      m_rightpoint_index(60)
{    
    for(int i=0;i<mzsview::ROWS;i++){
        m_values_min[i]=mzsview::DEFAULT_MINS[i];
        m_values_max[i]=mzsview::DEFAULT_MAXS[i];
    }
    setDragMode(QChartView::NoDrag);
    setMouseTracking(true);
    initChart();
}

int PlotView::setDatafile(QString filename)
{

    QFile fs(filename);
    if (!fs.open(QIODevice::ReadOnly | QIODevice::Text)) {
         return -1;
     }
    m_datafile=filename;
    m_filevalid=true;
    fs.close();
    return 0;
}

void PlotView::initChart()
{
    for(int i=0;i<mzsview::ROWS;i++){
       LineType* line=new LineType;
       QPen pen=line->pen();
       pen.setWidth(mzsview::NORMAL_LINEWIDTH);
       line->setPen(pen);
       line->setColor(QColor(mzsview::DEFAULT_COLORS[i]));
       m_chart->addSeries(line);
    }
    m_chart->addSeries(m_cursor_left);
    m_chart->addSeries(m_cursor_right);
    m_chart->legend()->hide();
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(mzsview::TICKCOUNT);
    axisX->setFormat(mzsview::AXISX_FORMAT);
    axisX->setTitleText(mzsview::AXISX_TITLE);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    for(int i=0;i<mzsview::ROWS;i++){
        m_chart->series().at(i)->attachAxis(axisX);
    }
    m_cursor_left->attachAxis(axisX);
    m_cursor_right->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat(mzsview::AXISY_FORMAT);
    axisY->setTitleText(mzsview::AXISY_TITLE);
    axisY->setMin(m_minY);
    axisY->setMax(m_maxY);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    for(int i=0;i<mzsview::ROWS;i++){
        m_chart->series().at(i)->attachAxis(axisY);
    }
    m_cursor_left->attachAxis(axisY);
    m_cursor_right->attachAxis(axisY);
    setChart(m_chart);
    setRenderHint(QPainter::Antialiasing);
    for(int i=0;i<mzsview::ROWS;i++){
        connect((LineType*)m_chart->series().at(i), &LineType::clicked, this
                ,[=](){m_prePickedIndex=m_pickedIndex;m_pickedIndex=i;pickLine();});
    }
    m_isinitchart=true;
}

void PlotView::emptyChart()
{
    if(m_isinitchart){
        for(int i=0;i<m_chart->series().size();i++){
            ((LineType*)m_chart->series().at(i))->clear();
        }
    }
    m_isloadchart=false;
}

int PlotView::setAllLineData()
{
    //QTime t;
    //t.start();
    if(!m_filevalid)
        return -1;
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
    emptyChart();//empty data for every line series
    m_valid_lines=mzsview::ROWS<values.size()-1?mzsview::ROWS : values.size()-1;
    for(int i=0;i<m_valid_lines;i++){
        m_chart->series().at(i)->setName(values.at(i+1));
        emitTagChangedSignal(i,values.at(i+1));
    }
    QDateTime momentInTime;
    LineType* p_line;
    QList<QPointF> points[mzsview::ROWS];
    while (!stream.atEnd()) {
        line = (stream.readLine()).trimmed();
        if(line.isEmpty())
            continue;
        values = line.split(mzsview::DATA_DELIMITER, QString::SkipEmptyParts);
        if(values.size()<2)
            continue;
        momentInTime=QDateTime::fromString(values[0],mzsview::DATETIME_FORMAT);
        if(!momentInTime.isValid())
            continue;
        for(int i=0;i<values.size()-1 && i<mzsview::ROWS;i++){
            double y=values[i+1].toDouble();
            if(abs(m_values_max[i]-m_values_min[i])>mzsview::ERR){
                y=(y-m_values_min[i])/(m_values_max[i]-m_values_min[i])*100.0;
            }
            double xx=momentInTime.toMSecsSinceEpoch();
            points[i].append(QPointF(xx,y));
        }
    }
    datafile.close();
    //qDebug()<<QString("LoadPointData time(ms):%1").arg(t.elapsed());
    for(int i=0;i<values.size()-1 && i<mzsview::ROWS;i++){
        p_line=(LineType*)m_chart->series().at(i);
        p_line->replace(points[i]);
    }
    //qDebug()<<QString("setLineData time(ms):%1").arg(t.elapsed());
    return 0;
}

int PlotView::setLineData(int line_No, int field_index)
{
    if(field_index<0 || line_No<0 || line_No>=m_chart->series().size()-2)
        return -1;
    if(!m_filevalid)
        return -1;

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
    if(values.size()<2 ||values.size()<=field_index){
        datafile.close();
        return -1;
    }
    m_chart->series().at(line_No)->setName(values.at(field_index));
    QDateTime momentInTime;
    LineType* p_line=(LineType*)m_chart->series().at(line_No);
    while (!stream.atEnd()) {
        line = (stream.readLine()).trimmed();
        if(line.isEmpty()) continue;
        values = line.split(mzsview::DATA_DELIMITER, QString::SkipEmptyParts);
        if(values.size()<2 ||values.size()<=field_index) continue;
        momentInTime=QDateTime::fromString(values[0],mzsview::DATETIME_FORMAT);
        if(!momentInTime.isValid()) continue;
        double y=values[field_index].toDouble();
        if(abs(m_values_max[line_No]-m_values_min[line_No])>mzsview::ERR){
            y=(y-m_values_min[line_No])/(m_values_max[line_No]-m_values_min[line_No])*100.0;
        }
        p_line->append(momentInTime.toMSecsSinceEpoch(),y);
    }
    datafile.close();
    return 0;
}

void PlotView::setCursor(int flag)//flag 0:called by loadChart,1:called by nextTime,
                                   //2:called by prevTime,3:called by by setTimespan
{
    LineType* p_line=NULL;
    for(int i=0;i<m_chart->series().size()-2;i++){
        p_line=(LineType*)m_chart->series().at(i);
        if(p_line->count()>1)
            break;
    }
    double x_left=0.0;
    double x_right=0.0;
    qint64 point_count;
    if(p_line!=NULL && p_line->count()>1){
        x_left=p_line->at(0).x();
        point_count=p_line->count();
        x_right=p_line->at(point_count-1).x();
    }else{
        return;
    }
    if(abs(x_right-x_left)<mzsview::ERR){
        return;
    }
    qint64 span_count;
    if(x_left+m_time_span<x_right)
        span_count=1.0*m_time_span*point_count/abs(x_right-x_left);
    else
        span_count=point_count;
    span_count=m_time_span/3600000*120;
    if(flag==0){
        m_current_lefttime=x_left;
        m_leftpoint_index=0;
        if(m_current_lefttime+m_time_span<x_right){
            x_right=m_current_lefttime+m_time_span;
        }
    }else if(flag==1){
        if(m_current_lefttime+m_time_span>x_right){
            return;
        }
        m_current_lefttime=m_current_lefttime+m_time_span;
        m_leftpoint_index+=span_count;
        x_left=m_current_lefttime;
        x_right=m_current_lefttime+m_time_span;
    }else if(flag==2){
        if(m_current_lefttime-m_time_span<x_left){
            return;
        }
        m_current_lefttime=m_current_lefttime-m_time_span;
        m_leftpoint_index-=span_count;
        x_left=m_current_lefttime;
        x_right=m_current_lefttime+m_time_span;
    }else if(flag==3){
        x_left=m_current_lefttime;
        if(m_current_lefttime+m_time_span<x_right){
            x_right=m_current_lefttime+m_time_span;
        }
    }else{
        return;
    }
    m_rightpoint_index=m_leftpoint_index+span_count-1;
    m_cursor_left->clear();
    m_cursor_right->clear();
    m_cursor_left->append(x_left,m_minY);
    m_cursor_left->append(x_left,m_maxY);
    m_cursor_right->append(x_right,m_minY);
    m_cursor_right->append(x_right,m_maxY);
    m_chart->axisX()->setMin(QDateTime::fromMSecsSinceEpoch(x_left));
    m_chart->axisX()->setMax(QDateTime::fromMSecsSinceEpoch(x_right));
    QString stime0=QDateTime::fromMSecsSinceEpoch(x_left).toString(mzsview::CURSOR_FORMAT);
    QString stime1=QDateTime::fromMSecsSinceEpoch(x_right).toString(mzsview::CURSOR_FORMAT);
    m_status->setPos(m_chart->size().width()*0.1, m_chart->size().height()*0.92);
    m_status->setText(QString("cursor1 %1   cursor2 %2").arg(stime0).arg(stime1));
    m_cursor_picked=NULL;
    if(flag==0){
        m_isloadchart=true;
    }
    emitCursorChangedSignal(mzsview::CURSOR1_COLUMN,x_left);
    emitCursorChangedSignal(mzsview::CURSOR2_COLUMN,x_right);
}

int PlotView::loadChart()
{
    if(setAllLineData()!=0)
        return -1;
    setCursor(0);
    m_isloadchart=true;
    return 0;
}

void PlotView::setTimespan()
{
    bool ok;
    double time_span=QInputDialog::getDouble(this,tr("Input Time Span"),tr("Hours:"),2,1,24,1, &ok);
    if(ok){
        m_time_span=time_span*3600000;
        setCursor(3);

    }
}

void PlotView::nextTime()
{
    if(!m_isloadchart)
        return;
    setCursor(1);
}

void PlotView::prevTime()
{
    if(!m_isloadchart)
        return;
    setCursor(2);
}

int PlotView::findYbyX(LineType *series,int series_index,double x,double& y)//series point.x must be sorted by ascend
{
    if(series==NULL)
        return -1;
    qint64 n=series->count();
    if(n<2)
        return -1;
    qint64 from=0;//m_leftpoint_index;
    qint64 to=n-1;//m_rightpoint_index;
    from=m_leftpoint_index;
    to=m_rightpoint_index;
    qint64 i;
    double xx;
    int j=0;
    qDebug()<<"start:"<<from<<":"<<to<<":"<<j;
    while(from<to-1){
        i=(from+to)/2;
        xx=series->at(i).x();
        if(abs(xx-x)<mzsview::ERR){
            y=series->at(i).y();
            break;
        }
        if(x<xx)
            to=i;
        else
            from=i;
        j++;
        qDebug()<<"mid:"<<from<<":"<<to<<":"<<j;
    }
    qDebug()<<"end:"<<from<<":"<<to<<":"<<j;
    double y0,y1,x0,x1;
    y0=series->at(from).y();
    y1=series->at(to).y();
    x0=series->at(from).x();
    x1=series->at(to).x();
    if(abs(x1-x0)<mzsview::ERR) //x1==x0
        y=(y0+y1)/2.0;
    else
        y=y0+(y1-y0)/(x1-x0)*(x-x0);
    i=series_index;
    if(i<mzsview::ROWS &&
            abs(m_values_max[i]-m_values_min[i])>mzsview::ERR){
        y=m_values_min[i]+(m_values_max[i]-m_values_min[i])*y/100.0;
    }
    return 0;
}

void PlotView::resizeEvent(QResizeEvent *event)
{
    resize(event->size());
    if(m_isloadchart){
         m_chart->resize(event->size());
         QString ss=m_status->text();
         m_status->setPos(m_chart->size().width()*0.1, m_chart->size().height()*0.92);
         m_status->setText(ss);
    }
    QChartView::resizeEvent(event);
}

void PlotView::mouseMoveEvent(QMouseEvent *event)
{
    //QTime t;
    //t.start();
    if(m_isloadchart && m_cursor_picked!=NULL){
        double x=m_chart->mapToValue(event->pos()).x();
//        m_cursor_picked->clear();
//        m_cursor_picked->append(x,m_minY);
//        m_cursor_picked->append(x,m_maxY);
        m_cursor_picked->replace(QList<QPointF>{{x,1.0*m_minY},{x,1.0*m_maxY}});
        QString stime0=QDateTime::fromMSecsSinceEpoch(m_cursor_left->at(0).x()).toString("hh:mm:ss");
        QString stime1=QDateTime::fromMSecsSinceEpoch(m_cursor_right->at(0).x()).toString("hh:mm:ss");
        m_status->setPos(m_chart->size().width()*0.1, m_chart->size().height()*0.92);
        m_status->setText(QString("cursor1 %1   cursor2 %2").arg(stime0).arg(stime1));
        //qDebug()<<"move1 time(ms):"<<t.elapsed();
        if(m_cursor_left==m_cursor_picked)
            emitCursorChangedSignal(mzsview::CURSOR1_COLUMN,x);
        else if(m_cursor_right==m_cursor_picked)
            emitCursorChangedSignal(mzsview::CURSOR2_COLUMN,x);
    }
    //qDebug()<<"move2 time(ms):"<<t.elapsed();
    //QChartView::mouseMoveEvent(event);
    //qDebug()<<"move3 time(ms):"<<t.elapsed();
}

void PlotView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(m_isloadchart){
        double x=m_chart->mapToValue(event->pos()).x();
        double xl=m_cursor_left->at(0).x();
        double xr=m_cursor_right->at(0).x();
        int cursor_column;
        LineType *mycursor;
        if(abs(x-xl)<=abs(x-xr)){
            mycursor=m_cursor_left;
            cursor_column=mzsview::CURSOR1_COLUMN;
        }else{
            mycursor=m_cursor_right;
            cursor_column=mzsview::CURSOR2_COLUMN;
        }
        mycursor->clear();
        mycursor->append(x,m_minY);
        mycursor->append(x,m_maxY);
        if(m_cursor_picked!=NULL)
            m_cursor_picked=NULL;
        else
            m_cursor_picked=mycursor;
        QString stime0=QDateTime::fromMSecsSinceEpoch(m_cursor_left->at(0).x()).toString("hh:mm:ss");
        QString stime1=QDateTime::fromMSecsSinceEpoch(m_cursor_right->at(0).x()).toString("hh:mm:ss");
        m_status->setPos(m_chart->size().width()*0.1, m_chart->size().height()*0.92);
        m_status->setText(QString("cursor1 %1   cursor2 %2").arg(stime0).arg(stime1));
        emitCursorChangedSignal(cursor_column,x);
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

void PlotView::pickLine()
{
    if(m_isloadchart){
        QString ss=m_chart->series().at(m_pickedIndex)->name();
        m_status->setPos(m_chart->size().width()*0.1, m_chart->size().height()*0.92);
        m_status->setText(QString("PickedLine:%1").arg(ss));
        LineType * picked=(LineType *)m_chart->series().at(m_pickedIndex);
        QPen pen=picked->pen();
        pen.setWidth(mzsview::BOLD_LINEWIDTH);
        picked->setPen(pen);
        if(m_pickedIndex!=m_prePickedIndex){
            picked=(LineType *)m_chart->series().at(m_prePickedIndex);
            pen=picked->pen();
            pen.setWidth(mzsview::NORMAL_LINEWIDTH);
            picked->setPen(pen);
        }
    }
}

void PlotView::emitCursorChangedSignal(int cursor_column,double cursorX)
{

    //QTime t;
    //t.start();
    if(m_isloadchart){
        QList<double> y_list;
        double y;
        for(int i=0;i<m_valid_lines;i++){
            findYbyX((LineType *)m_chart->series().at(i),i,cursorX,y);
            y_list.append(y);
        }
        //qDebug()<<"findYbyX time(ms):"<<t.elapsed();
        emit cursorValueChanged(cursor_column,y_list);
    }
    //qDebug()<<"cursorchanged time(ms):"<<t.elapsed();
}

void PlotView::emitTagChangedSignal(int rowIndex, const QString &tag)
{
    //if(m_isloadchart){
        emit tagValueChanged(rowIndex,tag);
    //}
}

void PlotView::modelDataChanged(const int row_index, const QString &tag, int tag_field_index, int select_state,
                                int phase_state,const QString &color, double min, double max)
{
    if(m_isloadchart && select_state==2){//2(Qt::Checked) is be checked
        int n=m_chart->series().size()-2;
        bool is_line_changed=true;
        LineType* line;
        if(row_index<n){//line is existed
            line=(LineType *)m_chart->series().at(row_index);
            if(line->name()==tag){//tag is not changed
                is_line_changed=false;
                if((abs(max-m_values_max[row_index])>mzsview::ERR
                        || abs(min-m_values_min[row_index])>mzsview::ERR)
                    && abs(max-min)>mzsview::ERR){//min or max is changed
                        for(int i=0;i<line->count();i++){
                            double y=line->at(i).y();
                            y=(y/100.0)*(m_values_max[row_index]-m_values_min[row_index])
                               +m_values_min[row_index];
                            y=(y-min)/(max-min)*100;
                            double x=line->at(i).x();
                            line->replace(i,QPointF(x,y));
                        }
                }//if min or max changed
            }else{
                line->clear();
            }//if tag is not changed
        }else{
            line=new LineType;
            m_chart->series().insert(row_index,line);
        }//if line is existed
        m_values_max[row_index]=max;
        m_values_min[row_index]=min;
        if(is_line_changed){//update line
            if(setLineData(row_index,tag_field_index)==0){
                double x_left=m_cursor_left->at(0).x();
                double x_right=m_cursor_right->at(0).x();
                emitCursorChangedSignal(mzsview::CURSOR1_COLUMN,x_left);
                emitCursorChangedSignal(mzsview::CURSOR2_COLUMN,x_right);
            }
        }
        line->setColor(QColor(color));
        line->setVisible();
    }else if(m_isloadchart && select_state==0){//0(Qt::Unchecked) is be unchecked
        int n=m_chart->series().size()-2;
        if(row_index<n){//line is existed,hide it
            m_chart->series().at(row_index)->hide();
        }
    }
}
