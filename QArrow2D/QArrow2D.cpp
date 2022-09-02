#include "GLWidget.h"
#include "QArrow2D.h"
#include "ui_QArrow2D.h"

#include <vector>
#include <string>
#include <QtWidgets>
#include <QTimer>
#include <QString>
#include <QFileDialog>
#include <windows.h>
#include <ShellApi.h>

using namespace std;

QArrow2D::QArrow2D(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::QArrow2D)
{
    ui->setupUi(this);
    setWindowTitle(tr("QArrow2D: virtual rooms"));
    setWindowIcon(QIcon(":/qArrow2D/icons/QArrow2D.ico"));

    d_monitor = new MonitorWidget(this);
    d_glWidget = new GlWidget(this);
    d_glWidget->resetZoom();
    setupCharts();

    ui->centralPane->addWidget(d_glWidget);
    ui->leftPane->addWidget(d_monitor);

    d_ready = false;
    setStates();
    d_timer = new QTimer(this);
    connect(d_timer, &QTimer::timeout, this, &QArrow2D::update);
    d_timer->start(32); //increase to run slower, lower to run faster perhaps with 16 as a minimum?

    connect(ui->playPause, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(ui->record, SIGNAL(clicked()), this, SLOT(record()));
    connect(ui->speed, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));
    connect(ui->loadScene, SIGNAL(clicked()), this, SLOT(loadScene()));
    connect(ui->defaultScene, SIGNAL(clicked()), this, SLOT(defaultScene()));
    connect(ui->openConfig, SIGNAL(clicked()), this, SLOT(openConfig()));
}

void QArrow2D::setupModel(string file)
{
    if (d_model != nullptr)
        delete(d_model);
    d_model = new Model(file);
    d_monitor->clean();
    d_valAgentPlot.setup(d_model->nAgents());
    d_aroAgentPlot.setup(d_model->nAgents());
    d_valTeamPlot.setup(d_model->nGroups());
    d_aroTeamPlot.setup(d_model->nGroups());
}

void QArrow2D::setupCharts()
{
    QLabel* titleIndv = new QLabel(this);
    titleIndv->setText("Individual agents");
    QFont titleFont;
    titleFont.setPixelSize(18);
    titleFont.bold();
    titleIndv->setFont(titleFont);
    titleIndv->setAlignment(Qt::AlignCenter);
    ui->rightPane->addWidget(titleIndv);
    ui->rightPane->addWidget(d_valAgentPlot.view());
    ui->rightPane->addWidget(d_aroAgentPlot.view());
    QLabel* titleTeam = new QLabel(this);
    titleTeam->setText("Team average");
    titleTeam->setFont(titleFont);
    titleTeam->setAlignment(Qt::AlignCenter);
    ui->rightPane->addWidget(titleTeam);
    ui->rightPane->addWidget(d_valTeamPlot.view());
    ui->rightPane->addWidget(d_aroTeamPlot.view());
}

void QArrow2D::update()
{
    if (d_model != nullptr)
    {
        if (d_running)
        {
            if (d_model->endStep() == d_model->timeStep())
            {
                playPause();
                d_glWidget->update();
                return;
            }
            else
            {
                d_model->step();
                float valTmp1 = 0.f, valTmp2 = 0.f, aroTmp1 = 0.f, aroTmp2 = 0.f;
                int count1 = 0, count2 = 0;
                for (int idx = 0; idx < d_model->nAgents(); ++idx)
                {
                    d_valAgentPlot.addPoint(idx, d_model->timeStep(), d_model->agent(idx).emotionMdl().expression().valence());
                    d_aroAgentPlot.addPoint(idx, d_model->timeStep(), d_model->agent(idx).emotionMdl().expression().arousal());
                    if (d_model->agent(idx).group() == 0)
                    {
                        valTmp1 += d_model->agent(idx).emotionMdl().expression().valence();
                        aroTmp1 += d_model->agent(idx).emotionMdl().expression().arousal();
                        ++count1;
                    }
                    else
                    {
                        valTmp2 += d_model->agent(idx).emotionMdl().expression().valence();
                        aroTmp2 += d_model->agent(idx).emotionMdl().expression().arousal();
                        ++count2;
                    }
                }    
                d_valTeamPlot.addPoint(0, d_model->timeStep(), valTmp1 / count1);
                d_aroTeamPlot.addPoint(0, d_model->timeStep(), aroTmp1 / count1);
                d_valTeamPlot.addPoint(1, d_model->timeStep(), valTmp2 / count2);
                d_aroTeamPlot.addPoint(1, d_model->timeStep(), aroTmp2 / count2);
                ui->stepsLbl->setText(QString::number(d_model->timeStep()));
            }
        }
        d_glWidget->update();
        d_monitor->update();        
        if (d_recording)
            saveImage();
    }
}

void QArrow2D::setStates()
{
    d_monitor->setEnabled(d_ready);
    d_glWidget->setEnabled(d_ready);
    ui->playPause->setEnabled(d_ready);
    ui->playPause->setEnabled(d_ready);
    ui->record->setEnabled(d_ready);
    ui->speed->setEnabled(d_ready);
    ui->stepsLbl->setEnabled(d_ready);
    ui->openConfig->setEnabled(d_ready);
    if (d_ready)
        ui->stepsLbl->setText("Ready");
    else
        ui->stepsLbl->setText("");
}

void QArrow2D::saveImage()
{
    QString file;
    if (d_recording)
        file = QString::fromStdString(d_recPath + to_string(d_model->timeStep()) + ".png");
    else
    {
        string name = d_model->outputPath();
        if (name == "N/A")
            name = "output";
        else
            name += "_[STEP" + to_string(d_model->timeStep()) + ']';
        file = QFileDialog::getSaveFileName(this, "Save as...", name.c_str(), "PNG (*.png);; BMP (*.bmp);;TIFF (*.tiff *.tif);; JPEG (*.jpg *.jpeg)");
    }
    QImage img = d_glWidget->grabFrameBuffer();
    QImage cropped = img.copy(QRect(
        d_glWidget->sizeOffX(),
        d_glWidget->sizeOffY(),
        1000 - d_glWidget->sizeOffX() * 2,
        1000 - d_glWidget->sizeOffY() * 2));
    cropped.save(file);
}

void QArrow2D::keyPressEvent(QKeyEvent* pe)
{
    switch (pe->key())
    {
    case Qt::Key_Space:
        if (d_ready)
            d_glWidget->update();
        break;

    case Qt::Key_Escape:
        QCoreApplication::quit();
        break;

    case Qt::Key_M:
        if (d_ready)
        {
            if (d_model->endStep() != d_model->timeStep())
            {
                d_model->step();
                d_glWidget->update();
                ui->stepsLbl->setText(QString::number(d_model->timeStep()));
            }
            d_monitor->update();
        }
        break;

    case Qt::Key_A:
        d_glWidget->setZoomOffX(d_glWidget->zoomOffX() - 5 * d_glWidget->zoomFac());
        if (d_glWidget->zoomOffX() < 0)
            d_glWidget->setZoomOffX(0);
        break;

    case Qt::Key_D:
        d_glWidget->setZoomOffX(d_glWidget->zoomOffX() + 5 * d_glWidget->zoomFac());
        if (d_glWidget->mapX() * d_glWidget->zoomFac() - d_glWidget->zoomOffX() < d_glWidget->mapX())
            d_glWidget->setZoomOffX(d_glWidget->mapX() * d_glWidget->zoomFac() - d_glWidget->mapX());
        break;

    case Qt::Key_W:
        d_glWidget->setZoomOffY(d_glWidget->zoomOffY() - 5 * d_glWidget->zoomFac());
        if (d_glWidget->zoomOffY() < 0)
            d_glWidget->setZoomOffY(0);
        break;

    case Qt::Key_S:
        d_glWidget->setZoomOffY(d_glWidget->zoomOffY() + 5 * d_glWidget->zoomFac());
        if (d_glWidget->mapY() * d_glWidget->zoomFac() - d_glWidget->zoomOffY() < d_glWidget->mapY())
            d_glWidget->setZoomOffY(d_glWidget->mapY() * d_glWidget->zoomFac() - d_glWidget->mapY());
        break;

    case Qt::Key_G:
        saveImage();
        break;
    }
}

//[ SLOTS ]
void QArrow2D::playPause()
{
    if (d_running)
    {
        ui->playPause->setIcon(QIcon(":/qArrow2D/icons/play.ico"));
        ui->record->setIcon(QIcon(":/qArrow2D/icons/recStart.ico"));
        d_running = false;
        d_recording = false;
    }
    else
    {
        ui->playPause->setIcon(QIcon(":/qArrow2D/icons/pause.ico"));
        ui->record->setIcon(QIcon(":/qArrow2D/icons/recStart.ico"));
        d_running = true;
        d_recording = false;
    }
}

void QArrow2D::record()
{
    if (d_recording)
    {
        ui->record->setIcon(QIcon(":/qArrow2D/icons/recStart.ico"));
        ui->playPause->setIcon(QIcon(":/qArrow2D/icons/play.ico"));
        d_running = false;
        d_recording = false;
    }
    else
    {
        if (d_recPath.empty())
        {
            d_recPath = "./" + d_model->outputPath();
            if (d_recPath == "N/A")
                d_recPath = "output";
            d_recPath += "_rec";
        }
        ui->record->setIcon(QIcon(":/qArrow2D/icons/recStop.ico"));
        ui->playPause->setIcon(QIcon(":/qArrow2D/icons/play.ico"));
        d_running = true;
        d_recording = true;
    }
}

void QArrow2D::setSpeed(int value)
{
    ui->speed->blockSignals(true);
    d_timer->start((pow(100-value, 2)) / 10 + 10);
    ui->speed->blockSignals(false);
}

void QArrow2D::loadScene()
{
    d_ready = false;
    setStates();
    d_config = QFileDialog::getOpenFileName(this, ".", tr("Open Scenario"),
        tr("Text Files (*.txt *.csv);;All Files (*)")).toStdString();
    if (d_config != "")
    {
        setupModel(d_config);
        d_ready = true;
    }
    setStates();
}

void QArrow2D::defaultScene()
{
    setupModel("./defaultConfig.txt");
    d_ready = true;
    setStates();
}

void QArrow2D::openConfig()
{
    ShellExecuteA(GetDesktopWindow(), "open", d_config.c_str(), NULL, NULL, SW_SHOW);
}

//[ ACCESSORS ]
Model* QArrow2D::model()
{
    return d_model;
}

MonitorWidget* QArrow2D::monitor()
{
    return d_monitor;
}

GlWidget* QArrow2D::glWidget()
{
    return d_glWidget;
}

bool QArrow2D::ready()
{
    return d_ready;
}
