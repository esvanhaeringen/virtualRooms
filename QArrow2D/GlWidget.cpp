#define _USE_MATH_DEFINES

#include "GlWidget.h"
#include <QWheelEvent> 
#include <QPaintEvent>
#include <vector>
#include <cmath>
#include <QDebug>

using namespace std;

GlWidget::GlWidget(QArrow2D* parent)
    : d_parent(parent)
{
    setFixedSize(1000, 1000);
    setSize();
    setAutoFillBackground(true);
    d_painter.setRenderHint(QPainter::Antialiasing);
    d_painter.setRenderHint(QPainter::HighQualityAntialiasing);
}

void GlWidget::resetZoom()
{
    d_zoomFac = 1.f;
    d_zoomOffX = 0.f;
    d_zoomOffY = 0.f;
}

void GlWidget::setSize()
{
    d_zoomFac = 1;
    d_zoomOffX = 0;
    d_zoomOffY = 0;

    d_mapX = 1000;
    d_mapY = 1000;

    d_cameraScale = 1000 / std::max(d_mapX, d_mapY);

    if (d_mapX < d_mapY)
    {
        d_sizeOffX = 500 - (d_mapX / d_mapY) * 500;
        d_sizeOffY = 0;
    }
    else if (d_mapY < d_mapX)
    {
        d_sizeOffX = 0;
        d_sizeOffY = 500 - (d_mapY / d_mapX) * 500;
    }
    else
    {
        d_sizeOffX = 0;
        d_sizeOffY = 0;
    }
}

void GlWidget::paintEvent(QPaintEvent* event)
{
    if (d_parent->model() != nullptr && d_parent->model()->ready() && !d_drawing)
    {
        d_drawing = true;
        d_painter.begin(this);
        drawMap();
        d_painter.setPen(Qt::NoPen);

        //calculate location agents
        d_pointX.clear();
        d_pointY.clear();
        d_angle.clear();
        int breaksX = ceil(sqrt(d_parent->model()->nRooms()));
        int breaksY = ceil(float(d_parent->model()->nRooms()) / breaksX);
        float margin = min(d_mapX, d_mapY) * 0.05;
        float spaceX = (min(d_mapX, d_mapY) - 2 * margin) / breaksX;
        float spaceY = (min(d_mapX, d_mapY) - 2 * margin) / breaksY;
        int roomCounter = 0;
        for (int rIdx = 0; rIdx < d_parent->model()->nRooms(); ++rIdx)
        {
            vector<int> const& room = d_parent->model()->room(rIdx);
            if (room.size() > 0)
            {
                float centerX = margin + spaceX / 2 + spaceX * (roomCounter % breaksX);
                float centerY = margin + spaceY / 2 + spaceY * int(roomCounter / breaksX);
                drawRoom(centerX - 0.8 * spaceX / 2, centerY - 0.8 * spaceX / 2, centerX + 0.8 * spaceX / 2, centerY + 0.8 * spaceX / 2, d_zoomFac, Qt::black, QString::fromStdString(d_parent->model()->roomId(rIdx)));
                float anglePart = 2 * M_PI / room.size();
                int agentCounter = 0;
                for (int aIdx = 0; aIdx < room.size(); ++aIdx)
                {
                    Agent const& agent = d_parent->model()->agent(room.at(aIdx));
                    d_pointX[agent.idx()] = sin(anglePart * agentCounter) * (0.5 * spaceX / 2) + centerX;
                    d_pointY[agent.idx()] = cos(anglePart * agentCounter) * (0.5 * spaceX / 2) + centerY;
                    d_angle[agent.idx()] = -agentCounter * (1.f / room.size()) * 360;
                    ++agentCounter;
                }
            }
            ++roomCounter;
        }

        //draw agents
        int selected = d_parent->monitor()->agent();
        for (int idx = 0; idx < d_parent->model()->agents().size(); ++idx)
        {
            Agent const& agent = d_parent->model()->agent(idx);
            drawAgent(d_pointX[idx], d_pointY[idx], d_mapX / 50, d_angle[idx], outerColor(agent), innerColor(agent), selected == agent.idx());
        }
        d_painter.end();
        d_drawing = false;
    }
}

QColor GlWidget::outerColor(Agent const& agent)
{
    QColor color(d_colors[agent.emotionMdl().emotionLabel()]);
    return color;
}

QColor GlWidget::innerColor(Agent const& agent)
{
    QColor color;
    color.setRgb(30, 30, 30);
    return color;
}

void GlWidget::drawAgent(float xPos, float yPos, float radius, float angle,
    QColor cirCol, QColor triCol, bool selected)
{
    d_painter.translate(d_cameraScale * (xPos * d_zoomFac) - d_zoomOffX + d_sizeOffX,
        d_cameraScale * (yPos * d_zoomFac) - d_zoomOffY + d_sizeOffY);
    d_painter.setPen(QPen(Qt::black, 0, Qt::SolidLine, Qt::RoundCap));
    if (selected)
    {
        d_painter.setBrush(QColor("yellow"));
        d_painter.drawEllipse(QRectF(-radius * 1.2 * d_zoomFac * d_cameraScale,
            -radius * 1.2 * d_zoomFac * d_cameraScale, 2 * radius * 1.2 * d_zoomFac * d_cameraScale,
            2 * radius * 1.2 * d_zoomFac * d_cameraScale));
    }

    d_painter.setBrush(cirCol);
    d_painter.drawEllipse(QRectF(-radius * d_zoomFac * d_cameraScale,
        -radius * d_zoomFac * d_cameraScale, 2 * radius * d_zoomFac * d_cameraScale,
        2 * radius * d_zoomFac * d_cameraScale));

    d_painter.setBrush(cirCol.lighter(100 + (255 - cirCol.lightness()) / 1.5));

    d_painter.rotate(angle);
    QPolygonF triangle;
    triangle << QPointF((-(2 * radius) / 3.f) * d_zoomFac * d_cameraScale,
        ((2 * radius) / 3.f) * d_zoomFac * d_cameraScale) <<
        QPointF(((2 * radius) / 3.f) * d_zoomFac * d_cameraScale, ((2 * radius) / 3.f) *
            d_zoomFac * d_cameraScale) <<
        QPointF(0, (-(2 * radius) / 2.f) * d_zoomFac * d_cameraScale);
    d_painter.drawPolygon(triangle);  // Draw a triangle on a polygonal model
    d_painter.rotate(-angle);
    d_painter.translate(-d_cameraScale * (xPos * d_zoomFac) + d_zoomOffX - d_sizeOffX,
        -d_cameraScale * (yPos * d_zoomFac) + d_zoomOffY - d_sizeOffY);
}

void GlWidget::drawLine(float xBegin, float yBegin, float xEnd, float yEnd,
    float size, QColor lineCol)
{
    d_painter.setPen(QPen(lineCol, size, Qt::DotLine, Qt::RoundCap));
    d_painter.drawLine(
        xBegin * d_zoomFac * d_cameraScale - d_zoomOffX + d_sizeOffX,
        yBegin * d_zoomFac * d_cameraScale - d_zoomOffY + d_sizeOffY,
        xEnd * d_zoomFac * d_cameraScale - d_zoomOffX + d_sizeOffX,
        yEnd * d_zoomFac * d_cameraScale - d_zoomOffY + d_sizeOffY);
}

void GlWidget::drawRoom(float x1, float y1, float x2, float y2,
    float size, QColor lineCol, QString label)
{
    d_painter.setFont(QFont("times", 22 * d_zoomFac));
    d_painter.setPen(QPen(lineCol, size, Qt::SolidLine, Qt::RoundCap));
    x1 = x1 * d_zoomFac * d_cameraScale - d_zoomOffX + d_sizeOffX;
    y1 = y1 * d_zoomFac * d_cameraScale - d_zoomOffY + d_sizeOffY;
    x2 = x2 * d_zoomFac * d_cameraScale - d_zoomOffX + d_sizeOffX;
    y2 = y2 * d_zoomFac * d_cameraScale - d_zoomOffY + d_sizeOffY;
    d_painter.drawRect(QRectF(x1, y1, x2 - x1, y2 - y1));
    d_painter.drawText(QRectF(x1, y1 - (y2 - y1) * 0.12, x2 - x1, (y2 - y1) * 0.12), label, QTextOption(Qt::AlignTop | Qt::AlignHCenter));
}

void GlWidget::drawMap()
{
    d_painter.setPen(Qt::NoPen);
    d_painter.setBrush(Qt::white);
    d_painter.drawRect(QRect(
        d_sizeOffX - d_zoomOffX,
        d_sizeOffY - d_zoomOffY,
        d_mapX * d_zoomFac * d_cameraScale,
        d_mapY * d_zoomFac * d_cameraScale));
}

void GlWidget::wheelEvent(QWheelEvent* event)
{
    if (d_parent->model() != nullptr)
    {
        float oldZoomFac = d_zoomFac;
        float numStep = (event->angleDelta().y() / 8 / 15) * WHEEL_DELTA;

        //if outside zoom limits or mouse is not inside the map
        if ((d_zoomFac == 0 && numStep < 0) ||
            (d_zoomFac == 10 && numStep > 0) ||
            event->x() < d_sizeOffX - d_zoomOffX ||
            event->x() > d_sizeOffX - d_zoomOffX + d_mapX * d_zoomFac * d_cameraScale ||
            event->y() < d_sizeOffY - d_zoomOffY ||
            event->y() > d_sizeOffY - d_zoomOffY + d_mapY * d_zoomFac * d_cameraScale)
            return;

        d_zoomFac += (numStep * 0.01f);

        if (d_zoomFac < 1.f)
            d_zoomFac = 1.f;
        else if (d_zoomFac > 10.f)
            d_zoomFac = 10.f;

        float pxWidth = d_mapX * d_cameraScale;
        float pxHeight = d_mapY * d_cameraScale;
        float relPosX = d_zoomOffX / (pxWidth * oldZoomFac) + (event->x() - d_sizeOffX) / (pxWidth * oldZoomFac);
        float relPosY = d_zoomOffY / (pxHeight * oldZoomFac) + (event->y() - d_sizeOffY) / (pxHeight * oldZoomFac);
        d_zoomOffX = relPosX * (d_zoomFac * pxWidth - pxWidth);
        d_zoomOffY = relPosY * (d_zoomFac * pxHeight - pxHeight);

    }
}

void GlWidget::mousePressEvent(QMouseEvent* event)
{
    if (d_parent->model() != nullptr)
    {
        float x = (d_zoomOffX - d_sizeOffX + event->x()) / (d_zoomFac * d_cameraScale);
        float y = (d_zoomOffY - d_sizeOffY + event->y()) / (d_zoomFac * d_cameraScale);

        int selectedAgent = -1;
        float selectedDist = min(d_mapX, d_mapY) / 50;

        for (int idx = 0; idx < d_parent->model()->agents().size(); ++idx)
        {
            Agent const& agent = d_parent->model()->agent(idx);
            float dist = sqrt(pow(d_pointX[agent.idx()] - x, 2) + pow(d_pointY[agent.idx()] - y, 2));
            if (dist < selectedDist)
            {
                selectedAgent = agent.idx();
                selectedDist = dist;

            }
        }

        if (selectedAgent != -1)
            d_parent->monitor()->setAgent(selectedAgent);
        else
            d_parent->monitor()->clean();
    }
}

//[ ACCESSORS ]
float const GlWidget::zoomFac() const
{
    return d_zoomFac;
}
float const GlWidget::zoomOffX() const
{
    return d_zoomOffX;
}
float const GlWidget::zoomOffY() const
{
    return d_zoomOffY;
}
float const GlWidget::sizeOffX() const
{
    return d_sizeOffX;
}
float const GlWidget::sizeOffY() const
{
    return d_sizeOffY;
}
float const GlWidget::cameraScale() const
{
    return d_cameraScale;
}
float const GlWidget::mapX() const
{
    return d_mapX;
}
float const GlWidget::mapY() const
{
    return d_mapY;
}
float const GlWidget::capture() const
{
    return d_capture;
}

//[ MODIFIERS ]
void GlWidget::setZoomFac(float value)
{
    d_zoomFac = value;
}
void GlWidget::setZoomOffX(float value)
{
    d_zoomOffX = value;
}
void GlWidget::setZoomOffY(float value)
{
    d_zoomOffY = value;
}
void GlWidget::setSizeOffX(float value)
{
    d_sizeOffX = value;
}
void GlWidget::setSizeOffY(float value)
{
    d_sizeOffY = value;
}
void GlWidget::setCameraScale(float value)
{
    d_cameraScale = value;
}
void GlWidget::setMapX(float value)
{
    d_mapX = value;
}
void GlWidget::setMapY(float value)
{
    d_mapY = value;
}