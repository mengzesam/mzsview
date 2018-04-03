#ifndef PLOTVIEW_H
#define PLOTVIEW_H
#include "mzsviewglobal.h"
#include <QChartView>
#include <QtCharts/chartsnamespace.h>
#include <QLineSeries>
#include <QChart>
#include <QDateTime>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QStandardItemModel>
#include <QDebug>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QMouseEvent;
class QResizeEvent;
QT_END_NAMESPACE

QT_CHARTS_BEGIN_NAMESPACE
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

//QT_BEGIN_NAMESPACE

typedef QLineSeries LineType;

class PlotView : public QChartView
{
    Q_OBJECT

public:
    PlotView(QWidget *parent = 0);
    int setDatafile(QString filename);
    int loadChart();

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

public slots:
    void pickLine();
    void modelDataChanged(const int row_index,const QString &tag,int tag_field_index,
                           int select_state,int phase_state,const QString &color,double min,double max);
    void nextTime();
    void prevTime();

signals:
    void cursorValueChanged(int cursorColumn,QList<double> cursorValue);
    void tagValueChanged(int rowIndex,const QString& tag);

private:
    void initChart();
    void emptyChart();
    int setAllLineData();
    int setLineData(int line_No,int field_index);
    int findYbyX(QLineSeries * series,int series_index,double x,double& y);
    void emitCursorChangedSignal(int cursorColumn,double cursorX);
    void emitTagChangedSignal(int rowIndex,const QString& tag);

private:
    QChart *m_chart;
    QString m_datafile;
    LineType *m_cursor_left;
    LineType *m_cursor_right;
    LineType *m_cursor_picked;
    QGraphicsSimpleTextItem *m_status;
    int m_pickedIndex;
    int m_prePickedIndex;
    int m_maxY;
    int m_minY;
    int m_valid_lines;
    bool m_isloadchart;
    bool m_isinitchart;
    bool m_filevalid;
    double m_values_min[mzsview::ROWS];
    double m_values_max[mzsview::ROWS];
    qint64 m_current_lefttime;
    qint64 m_time_span;

private://class static member
};

//QT_END_NAMESPACE
#endif // PLOTVIEW_H
