#include "LineChart.h"

using namespace std;

LineChart::LineChart(QString xLabel, QString yLabel)
{
    d_xAxis = new QValueAxis();
    d_yAxis = new QValueAxis();
    d_yAxis->setTitleText(yLabel);
    d_xAxis->setTitleText(xLabel);
    d_xAxis->setTickCount(5);
    d_xAxis->setRange(0, 50);
    d_yAxis->setRange(-1, 1);
    d_chart = new QChart();
    d_chart->setBackgroundVisible(false);
    d_chart->setMargins(QMargins(0, 0, 0, 0));
    d_chart->legend()->hide();
    d_chart->addAxis(d_xAxis, Qt::AlignBottom);
    d_chart->addAxis(d_yAxis, Qt::AlignLeft);
    d_view = new QChartView(d_chart);
    d_view->setRenderHint(QPainter::Antialiasing);
    d_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    d_view->setMinimumWidth(300);
}

void LineChart::setup(int numberOfAgents)
{
    clear();
    for (int agent = 0; agent < numberOfAgents; ++agent)
    {
        QLineSeries* series = new QLineSeries();
        d_chart->addSeries(series);
        series->attachAxis(d_xAxis);
        series->attachAxis(d_yAxis);
        d_series.push_back(series);
    }
}

void LineChart::clear()
{
    for (int idx = 0; idx < d_series.size(); ++idx)
    {
        d_series[idx]->clear();
    }
    d_series.clear();
}

void LineChart::addPoint(int seriesIdx, int x, double y)
{
    d_series[seriesIdx]->append(x, y);

    if (x > d_maxX)
    {
        d_maxX = (int(x / 50) + 1) * 50;
        d_chart->axisX()->setRange(0, d_maxX);
    }
    if (y > d_maxY)
    {
        d_maxY = y;
        d_chart->axisY()->setRange(d_minY, d_maxY);
    }
    if (y < d_minY)
    {
        d_minY = y;
        d_chart->axisY()->setRange(d_minY, d_maxY);
    }
}

//[ ACCESSORS ]
QChartView* LineChart::view()
{
    return d_view;
}