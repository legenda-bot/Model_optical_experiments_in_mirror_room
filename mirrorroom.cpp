#include "mirrorroom.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

MirrorRoom::MirrorRoom(QWidget *parent)
    : QWidget(parent)
    , m_creationMode(DrawByClick)
    , m_currentRay(nullptr)
    , m_regularWallsCount(4)
    , m_roomCompleted(false)
{
    setMinimumSize(600, 500);
}

MirrorRoom::~MirrorRoom()
{
    qDeleteAll(m_walls);
    delete m_currentRay;
}

void MirrorRoom::setRoomCreationMode(MirrorRoom::RoomCreationMode mode)
{
    m_creationMode = mode;
    clearRoom();
}

void MirrorRoom::setNumberOfWalls(int count)
{
    if (count >= 4 && count <= 9) {
        m_regularWallsCount = count;
        if (m_creationMode == RegularPolygon) {
            clearRoom();
            createRegularPolygon();
        }
    }
}

void MirrorRoom::startRayExperiment(const QPointF& startPoint, double angle)
{
    if (m_roomCompleted && !m_walls.isEmpty()) {
        delete m_currentRay;
        m_currentRay = new LightRay(startPoint, qDegreesToRadians(angle), m_walls);
        update();
    }
}

void MirrorRoom::clearRoom()
{
    qDeleteAll(m_walls);
    m_walls.clear();
    m_tempPoints.clear();
    delete m_currentRay;
    m_currentRay = nullptr;
    m_roomCompleted = false;
    update();
}

void MirrorRoom::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background
    painter.fillRect(rect(), Qt::white);

    if (!m_roomCompleted) {
        // Draw temporary points during room creation
        painter.setPen(QPen(Qt::blue, 2));
        painter.setBrush(Qt::NoBrush);

        for (const QPointF& point : m_tempPoints) {
            painter.drawEllipse(point, 5, 5);
        }

        // Draw connecting lines
        if (m_tempPoints.size() > 1) {
            for (int i = 1; i < m_tempPoints.size(); ++i) {
                painter.drawLine(m_tempPoints[i-1], m_tempPoints[i]);
            }
        }
    } else {
        drawWalls(painter);

        if (m_currentRay) {
            drawRay(painter);
        }
    }
}

void MirrorRoom::mousePressEvent(QMouseEvent *event)
{
    QPointF clickPos = event->pos();

    if (m_creationMode == DrawByClick && !m_roomCompleted) {
        m_tempPoints.append(clickPos);

        if (m_tempPoints.size() >= 3) {
            // Check if clicked near first point to complete polygon
            if (QLineF(clickPos, m_tempPoints.first()).length() < 20) {
                completeRoom();
            }
        }
        update();
    } else if (m_roomCompleted) {
        // Handle wall selection for configuration
        for (int i = 0; i < m_walls.size(); ++i) {
            if (m_walls[i]->containsPoint(clickPos)) {
                emit wallSelected(i);  // Теперь этот сигнал объявлен
                break;
            }
        }
    }
}

void MirrorRoom::createRegularPolygon()
{
    m_tempPoints.clear();
    QRect area = rect().adjusted(50, 50, -50, -50);

    for (int i = 0; i < m_regularWallsCount; ++i) {
        m_tempPoints.append(calculateRegularPolygonPoint(i, m_regularWallsCount, area));
    }

    completeRoom();
}

void MirrorRoom::completeRoom()
{
    if (m_tempPoints.size() < 3) return;

    qDeleteAll(m_walls);
    m_walls.clear();

    // Create walls from points
    for (int i = 0; i < m_tempPoints.size(); ++i) {
        QPointF start = m_tempPoints[i];
        QPointF end = m_tempPoints[(i + 1) % m_tempPoints.size()];
        m_walls.append(new Wall(start, end));
    }

    m_roomCompleted = true;
    update();
}

void MirrorRoom::drawWalls(QPainter& painter)
{
    for (Wall* wall : m_walls) {
        wall->draw(painter);
    }
}

void MirrorRoom::drawRay(QPainter& painter)
{
    if (m_currentRay) {
        m_currentRay->draw(painter);
    }
}

QPointF MirrorRoom::calculateRegularPolygonPoint(int index, int totalPoints, const QRect& area)
{
    double angle = 2 * M_PI * index / totalPoints - M_PI / 2;
    double radius = qMin(area.width(), area.height()) / 2.0;

    double x = area.center().x() + radius * cos(angle);
    double y = area.center().y() + radius * sin(angle);

    return QPointF(x, y);
}

void MirrorRoom::saveExperiment(const QString& filename)
{
    // TODO: Implement save functionality
    Q_UNUSED(filename);
}

void MirrorRoom::loadExperiment(const QString& filename)
{
    // TODO: Implement load functionality
    Q_UNUSED(filename);
}
