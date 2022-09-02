#pragma once
#include "QArrow2D.h"
#include <QGLWidget>
#include <QPainter>
#include <QDebug>

class QArrow2D;
class Agent;

class GlWidget : public QGLWidget
{
    Q_OBJECT

    QArrow2D* d_parent;
    QPainter d_painter;
    bool d_drawing = false;
    QImage d_map;
    float d_mapX;
    float d_mapY;

    std::map<int, double> d_pointX;
    std::map<int, double> d_pointY;
    std::map<int, double> d_angle;

    //View state
    float d_zoomFac;
    float d_zoomOffX;
    float d_zoomOffY;
    float d_sizeOffX;
    float d_sizeOffY;
    float d_cameraScale;
    bool d_capture;

    std::map<std::string, const char*> d_colors{
        {"Neutral", "#666666"},
        {"Pleased/relaxed", "#8ee8ac"},
        {"Sleepy/peaceful", "#0081a3"},
        {"Tired/bored", "#3d004d"},
        {"Miserable/sad", "#d86f91"},
        {"Distressed/frustrated", "#bf0005"},
        {"Angry/afraid", "#ff8e24"},
        {"Excited/aroused", "#a5b400"},
        {"Happy/delighted", "#176014"}
    };

public:
    GlWidget(QArrow2D* parent);
    void resetZoom();

    //[ ACCESSORS ]
    float const zoomFac() const;
    float const zoomOffX() const;
    float const zoomOffY() const;
    float const sizeOffX() const;
    float const sizeOffY() const;
    float const cameraScale() const;
    float const mapX() const;
    float const mapY() const;
    float const capture() const;

    //[ MODIFIERS ]
    void setZoomFac(float value);
    void setZoomOffX(float value);
    void setZoomOffY(float value);
    void setSizeOffX(float value);
    void setSizeOffY(float value);
    void setCameraScale(float value);
    void setMapX(float value);
    void setMapY(float value);

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setSize();
    void drawAgent(float xPos, float yPos, float radius, float angle,
        QColor cirCol, QColor triCol, bool selected);
    void drawLine(float xBegin, float yBegin, float xEnd, float yEnd,
        float size, QColor lineCol);
    void drawRoom(float x1, float y1, float x2, float y2,
        float size, QColor lineCol, QString label);
    void drawMap();
    QColor outerColor(Agent const& agent);
    QColor innerColor(Agent const& agent);
};

