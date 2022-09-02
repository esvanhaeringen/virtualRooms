#pragma once
#include <vector>
#include <string>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

class LineChart
{
    std::vector<QLineSeries*> d_series;
    QChart* d_chart;
    QValueAxis* d_xAxis;
    QValueAxis* d_yAxis;
    QChartView* d_view;
    float d_maxY = 0;
    float d_minY = 0;
    int d_maxX = 0;

public:
    LineChart(QString xLabel, QString yLabel);
    void setup(int numberOfAgents);
    void addPoint(int seriesIdx, int x, double y);
    void clear();

    //[ ACCESSORS ]
    QChartView* view();
private:


};

