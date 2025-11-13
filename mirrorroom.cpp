#include "mirrorroom.h"
#include <QPainter>
#include <QMouseEvent>
#include <cmath>
#include <QMessageBox>

MirrorRoom::MirrorRoom(QWidget *parent)
    : QWidget(parent)
    , m_creationMode(DrawByClick)
    , m_currentRay(nullptr)
    , m_regularWallsCount(4)
    , m_roomCompleted(false)
    , m_selectingStartPoint(false)
    , m_selectingAngle(false)
    , m_currentAngle(0.0) // 0° - вправо
{
    setMinimumSize(600, 500);
    setMouseTracking(true);
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
        // Угол передается в радианах, 0 - вправо, увеличение против часовой стрелки
        m_currentRay = new LightRay(startPoint, qDegreesToRadians(angle), m_walls);
        update();
    }
}

void MirrorRoom::startRayExperiment(double angle)
{
    if (m_roomCompleted && !m_walls.isEmpty() && !m_rayStartPoint.isNull()) {
        delete m_currentRay;
        // Угол передается в радианах, 0 - вправо, увеличение против часовой стрелки
        m_currentRay = new LightRay(m_rayStartPoint, qDegreesToRadians(angle), m_walls);
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
    m_rayStartPoint = QPointF();
    m_selectingStartPoint = false;
    m_selectingAngle = false;
    m_currentAngle = 0.0;
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
        painter.setPen(QPen(Qt::blue, 3));
        painter.setBrush(Qt::blue);

        for (const QPointF& point : m_tempPoints) {
            painter.drawEllipse(point, 6, 6);
        }

        // Draw connecting lines
        if (m_tempPoints.size() > 1) {
            painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
            for (int i = 1; i < m_tempPoints.size(); ++i) {
                painter.drawLine(m_tempPoints[i-1], m_tempPoints[i]);
            }

            // Draw line back to first point if we have at least 3 points
            if (m_tempPoints.size() >= 3) {
                painter.drawLine(m_tempPoints.last(), m_tempPoints.first());
            }
        }
    } else {
        drawWalls(painter);

        if (m_currentRay) {
            drawRay(painter);
        }
    }

    // Draw start point and trajectory
    drawStartPointAndTrajectory(painter);

    if (m_selectingAngle && !m_rayStartPoint.isNull()) {
        drawAngleSelection(painter);
    }
}

void MirrorRoom::mousePressEvent(QMouseEvent *event)
{
    QPointF clickPos = event->pos();

    if (m_selectingStartPoint && m_roomCompleted) {
        // Ищем стену, на которую кликнули
        Wall* wall = findWallAtPoint(clickPos);
        if (wall) {
            // Находим ближайшую точку на ВСЕЙ СТЕНЕ
            m_rayStartPoint = wall->getClosestPoint(clickPos);
            m_selectingStartPoint = false;
            m_selectingAngle = true;
            // Начальное направление - вправо (0°)
            m_angleSelectionPoint = m_rayStartPoint + QPointF(50, 0);
            m_currentAngle = 0.0;
            update();
        } else {
            QMessageBox::warning(this, "Selection Error",
                                 "Please click on a wall to select the starting point.");
        }
        return;
    } else if (m_selectingAngle && m_roomCompleted) {
        // Завершаем выбор угла
        m_currentAngle = calculateAngle(m_rayStartPoint, clickPos);
        m_selectingAngle = false;
        update();
        return;
    }

    if (m_creationMode == DrawByClick && !m_roomCompleted) {
        // Проверяем, не закрываем ли многоугольник
        if (m_tempPoints.size() >= 3 && QLineF(clickPos, m_tempPoints.first()).length() < 25) {
            if (m_tempPoints.size() >= 4) {
                completeRoom();
            } else {
                QMessageBox::warning(this, "Room Creation",
                                     "Room must have at least 4 walls. Please add more points.");
                return;
            }
        } else {
            m_tempPoints.append(clickPos);
        }
        update();
    } else if (m_roomCompleted) {
        // Handle wall selection for configuration - РАБОТАЕТ ПО ВСЕЙ СТЕНЕ
        for (int i = 0; i < m_walls.size(); ++i) {
            if (m_walls[i]->containsPoint(clickPos)) {
                emit wallSelected(i);
                update();
                break;
            }
        }
    }
}

void MirrorRoom::mouseMoveEvent(QMouseEvent *event)
{
    if (m_selectingAngle && !m_rayStartPoint.isNull()) {
        m_angleSelectionPoint = event->pos();
        // Обновляем угол в реальном времени при перемещении мыши
        m_currentAngle = calculateAngle(m_rayStartPoint, m_angleSelectionPoint);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

Wall* MirrorRoom::findWallAtPoint(const QPointF& point) const
{
    for (Wall* wall : m_walls) {
        if (wall->containsPoint(point)) {
            return wall;
        }
    }
    return nullptr;
}

double MirrorRoom::calculateAngle(const QPointF& start, const QPointF& end) const
{
    QLineF line(start, end);
    double dx = line.dx();
    double dy = line.dy();

    // Вычисляем угол в радианах
    double angleRad = atan2(dy, dx);

    // Преобразуем в градусы
    double angleDeg = angleRad * 180.0 / M_PI;

    // Нормализуем угол в диапазон [0, 360)
    if (angleDeg < 0) {
        angleDeg += 360.0;
    }

    return angleDeg;
}

QPointF MirrorRoom::calculateDirectionVector(double angle) const
{
    // Преобразуем угол в радианы
    double angleRad = qDegreesToRadians(angle);

    // Вычисляем вектор направления
    return QPointF(cos(angleRad), sin(angleRad));
}

void MirrorRoom::drawAngleSelection(QPainter& painter)
{
    if (m_rayStartPoint.isNull() || m_angleSelectionPoint.isNull()) return;

    // Рисуем линию выбора угла
    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine));
    painter.drawLine(m_rayStartPoint, m_angleSelectionPoint);

    // Рисуем стрелку направления
    QLineF directionLine(m_rayStartPoint, m_angleSelectionPoint);
    if (directionLine.length() > 40) {
        directionLine.setLength(40);
    }

    // Вычисляем угол для стрелки
    double angleRad = qDegreesToRadians(m_currentAngle);

    // Вычисляем точки стрелки
    QPointF arrowTip = directionLine.p2();
    QPointF arrowLeft = arrowTip + QPointF(cos(angleRad + 2.5) * 12,
                                           sin(angleRad + 2.5) * 12);
    QPointF arrowRight = arrowTip + QPointF(cos(angleRad - 2.5) * 12,
                                            sin(angleRad - 2.5) * 12);

    painter.drawLine(arrowTip, arrowLeft);
    painter.drawLine(arrowTip, arrowRight);

    // Показываем угол
    QString angleText = QString("%1°").arg(qRound(m_currentAngle));
    painter.setPen(Qt::darkRed);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(m_angleSelectionPoint + QPointF(10, -10), angleText);
}

void MirrorRoom::drawStartPointAndTrajectory(QPainter& painter)
{
    if (!m_rayStartPoint.isNull()) {
        // Рисуем точку старта
        painter.setBrush(Qt::red);
        painter.setPen(QPen(Qt::darkRed, 2));
        painter.drawEllipse(m_rayStartPoint, 8, 8);

        // Рисуем крестик внутри точки
        painter.setPen(QPen(Qt::white, 2));
        painter.drawLine(m_rayStartPoint - QPointF(5, 0), m_rayStartPoint + QPointF(5, 0));
        painter.drawLine(m_rayStartPoint - QPointF(0, 5), m_rayStartPoint + QPointF(0, 5));

        // Рисуем фиксированную траекторию
        if (!m_selectingAngle) {
            QPointF direction = calculateDirectionVector(m_currentAngle);
            QLineF trajectory(m_rayStartPoint, m_rayStartPoint + direction * 80);

            painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
            painter.drawLine(trajectory);

            // Рисуем стрелку на конце траектории
            QPointF arrowTip = trajectory.p2();
            double angleRad = qDegreesToRadians(m_currentAngle);

            QPointF arrowLeft = arrowTip + QPointF(cos(angleRad + 2.5) * 12,
                                                   sin(angleRad + 2.5) * 12);
            QPointF arrowRight = arrowTip + QPointF(cos(angleRad - 2.5) * 12,
                                                    sin(angleRad - 2.5) * 12);

            painter.drawLine(arrowTip, arrowLeft);
            painter.drawLine(arrowTip, arrowRight);

            // Показываем угол
            QString angleText = QString("%1°").arg(qRound(m_currentAngle));
            painter.setPen(Qt::darkRed);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(arrowTip + QPointF(10, -10), angleText);
        }
    }
}

void MirrorRoom::createRegularPolygon()
{
    m_tempPoints.clear();
    QRect area = rect().adjusted(100, 100, -100, -100);

    for (int i = 0; i < m_regularWallsCount; ++i) {
        m_tempPoints.append(calculateRegularPolygonPoint(i, m_regularWallsCount, area));
    }

    completeRoom();
}

void MirrorRoom::completeRoom()
{
    if (m_tempPoints.size() < 4) {
        QMessageBox::warning(this, "Room Creation",
                             "Room must have at least 4 walls. Please add more points.");
        return;
    }

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
    double radius = qMin(area.width(), area.height()) / 2.5;

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
