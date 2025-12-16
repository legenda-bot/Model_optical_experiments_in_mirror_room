#include "mirrorroom.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>
#include <QMessageBox>
#include <QTransform>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QKeyEvent>
#include <QCursor>

namespace {
constexpr double kZoomStep = 1.1;
constexpr double kMinZoom = 0.25;
constexpr double kMaxZoom = 6.0;
}

MirrorRoom::MirrorRoom(QWidget *parent)
    : QWidget(parent)
    , m_creationMode(DrawByClick)
    , m_currentRay(nullptr)
    , m_rayStepIndex(0)
    , m_minFrameIntervalMs(16)
    , m_currentSegmentStartMs(0)
    , m_animationIntervalMs(400)
    , m_rayLifetimeMs(3000)
    , m_regularWallsCount(4)
    , m_regularPolygonScale(50) // стартовый масштаб выпуклого полигона в процентах от доступной области
    , m_roomCompleted(false)
    , m_selectingStartPoint(false)
    , m_selectingAngle(false)
    , m_currentAngle(0.0) // angle in degrees
    , m_zoomFactor(1.0)
    , m_cameraCenter(QPointF(0, 0))
    , m_cameraInitialized(false)
    , m_isPanning(false)
{
    setMinimumSize(600, 500);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Анимация луча по сегментам с фиксированным таймером
    m_rayAnimationTimer.setInterval(m_minFrameIntervalMs);
    connect(&m_rayAnimationTimer, &QTimer::timeout, this, &MirrorRoom::advanceRayAnimation);
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
        // Старт луча от заданной точки под углом (градусы)
        m_currentRay = new LightRay(startPoint, qDegreesToRadians(angle), m_walls);
        m_rayStepIndex = 0;
        m_segmentTimes.clear();
        m_segmentTimes.resize(qMax(0, m_currentRay->path().size() - 1), -1);
        m_rayTimer.restart();
        m_currentSegmentStartMs = m_rayTimer.elapsed();
        m_rayAnimationTimer.setInterval(m_minFrameIntervalMs);
        rebuildSegmentDurations();
        m_rayAnimationTimer.start();
        emit simulationStateChanged(true); // running
        update();
    }
}

void MirrorRoom::startRayExperiment(double angle)
{
    if (m_roomCompleted && !m_walls.isEmpty() && !m_rayStartPoint.isNull()) {
        delete m_currentRay;
        // Старт луча из выбранной точки на стене
        m_currentRay = new LightRay(m_rayStartPoint, qDegreesToRadians(angle), m_walls);
        m_rayStepIndex = 0;
        m_segmentTimes.clear();
        m_segmentTimes.resize(qMax(0, m_currentRay->path().size() - 1), -1);
        m_rayTimer.restart();
        m_currentSegmentStartMs = m_rayTimer.elapsed();
        m_rayAnimationTimer.setInterval(m_minFrameIntervalMs);
        rebuildSegmentDurations();
        m_rayAnimationTimer.start();
        emit simulationStateChanged(true); // running
        update();
    }
}

void MirrorRoom::clearRay()
{
    delete m_currentRay;
    m_currentRay = nullptr;
    m_rayStepIndex = 0;
    m_currentSegmentStartMs = 0;
    m_segmentTimes.clear();
    m_segmentDurations.clear();
    m_rayTimer.invalidate();
    m_rayAnimationTimer.stop();
    emit simulationStateChanged(false); // not running
    update();
}

void MirrorRoom::clearRoom()
{
    // Полный сброс: удаляем стены, луч, очищаем состояние камеры
    qDeleteAll(m_walls);
    m_walls.clear();
    m_tempPoints.clear();
    delete m_currentRay;
    m_currentRay = nullptr;
    m_rayAnimationTimer.stop();
    m_rayStepIndex = 0;
    m_currentSegmentStartMs = 0;
    m_segmentTimes.clear();
    m_segmentDurations.clear();
    m_rayTimer.invalidate();
    m_roomCompleted = false;
    m_rayStartPoint = QPointF();
    m_selectingStartPoint = false;
    m_selectingAngle = false;
    m_currentAngle = 0.0;
    emit simulationStateChanged(false); // not running
    update();
}

void MirrorRoom::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    updateViewTransform();
    painter.setTransform(m_viewTransform);

    // Фон сцены в мировых координатах
    painter.fillRect(m_invViewTransform.mapRect(rect()), Qt::white);

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
    setFocus();
    updateViewTransform();

    // Панорамирование средней/правой кнопкой
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        m_isPanning = true;
        m_lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        return;
    }

    QPointF clickPos = event->pos();
    QPointF worldPos = m_invViewTransform.map(clickPos);

    if (m_selectingStartPoint && m_roomCompleted) {
        //    ,      
        Wall* wall = findWallAtPoint(worldPos);
        if (wall) {
            //            
            m_rayStartPoint = wall->getClosestPoint(worldPos);
            m_selectingStartPoint = false;
            m_selectingAngle = true;
            //     -   (0 )
            m_angleSelectionPoint = m_rayStartPoint + QPointF(50, 0);
            m_currentAngle = 0.0;
            update();
        } else {
            QMessageBox::warning(this, "Selection Error",
                                 "Please click on a wall to select the starting point.");
        }
        return;
    } else if (m_selectingAngle && m_roomCompleted) {
        // Set angle by click
        m_currentAngle = calculateAngle(m_rayStartPoint, worldPos);
        m_selectingAngle = false;
        emit angleUpdated(m_currentAngle);
        update();
        return;
    }

    if (m_creationMode == DrawByClick && !m_roomCompleted) {
        //  ,        
        if (m_tempPoints.size() >= 3 && QLineF(worldPos, m_tempPoints.first()).length() < 25) {
            if (m_tempPoints.size() >= 4) {
                completeRoom();
            } else {
                QMessageBox::warning(this, "Room Creation",
                                     "Room must have at least 4 walls. Please add more points.");
                return;
            }
        } else {
            m_tempPoints.append(worldPos);
        }
        update();
    } else if (m_roomCompleted) {
        // Handle wall selection for configuration -        
        for (int i = 0; i < m_walls.size(); ++i) {
            if (m_walls[i]->containsPoint(worldPos)) {
                emit wallSelected(i);
                update();
                break;
            }
        }
    }
}

void MirrorRoom::mouseMoveEvent(QMouseEvent *event)
{
    updateViewTransform();

    // Если панорамируем — двигаем центр камеры и не обрабатываем выделение угла
    if (m_isPanning) {
        QPointF delta = event->pos() - m_lastPanPos;
        m_lastPanPos = event->pos();
        m_cameraCenter -= QPointF(delta.x() / m_zoomFactor, delta.y() / m_zoomFactor);
        updateViewTransform();
        update();
        return;
    }

    QPointF worldPos = m_invViewTransform.map(event->pos());
    if (m_selectingAngle && !m_rayStartPoint.isNull()) {
        m_angleSelectionPoint = worldPos;
        //                
        m_currentAngle = calculateAngle(m_rayStartPoint, m_angleSelectionPoint);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void MirrorRoom::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) && m_isPanning) {
        m_isPanning = false;
        unsetCursor();
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

void MirrorRoom::keyPressEvent(QKeyEvent *event)
{
    bool handled = false;
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
        adjustZoom(kZoomStep, rect().center());
        handled = true;
    } else if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
        adjustZoom(1.0 / kZoomStep, rect().center());
        handled = true;
    }

    if (handled) {
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
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

    //        
    double angleRad = atan2(dy, dx);

    //      
    double angleDeg = angleRad * 180.0 / M_PI;

    //         [0, 360)
    if (angleDeg < 0) {
        angleDeg += 360.0;
    }

    return angleDeg;
}

QPointF MirrorRoom::calculateDirectionVector(double angle) const
{
    //        
    double angleRad = qDegreesToRadians(angle);

    //      
    return QPointF(cos(angleRad), sin(angleRad));
}

void MirrorRoom::drawAngleSelection(QPainter& painter)
{
    if (m_rayStartPoint.isNull() || m_angleSelectionPoint.isNull()) return;

    //        
    painter.setPen(QPen(Qt::red, 3, Qt::SolidLine));
    painter.drawLine(m_rayStartPoint, m_angleSelectionPoint);

    //      
    QLineF directionLine(m_rayStartPoint, m_angleSelectionPoint);
    if (directionLine.length() > 40) {
        directionLine.setLength(40);
    }

    //        
    double angleRad = qDegreesToRadians(m_currentAngle);

    //      
    QPointF arrowTip = directionLine.p2();
    QPointF arrowLeft = arrowTip + QPointF(cos(angleRad + 2.5) * 12,
                                           sin(angleRad + 2.5) * 12);
    QPointF arrowRight = arrowTip + QPointF(cos(angleRad - 2.5) * 12,
                                            sin(angleRad - 2.5) * 12);

    painter.drawLine(arrowTip, arrowLeft);
    painter.drawLine(arrowTip, arrowRight);

    //    
        QString angleText = QString("%1 deg").arg(qRound(m_currentAngle));
    painter.setPen(Qt::darkRed);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(m_angleSelectionPoint + QPointF(10, -10), angleText);
}

void MirrorRoom::drawStartPointAndTrajectory(QPainter& painter)
{
    if (!m_rayStartPoint.isNull()) {
        //      
        painter.setBrush(Qt::red);
        painter.setPen(QPen(Qt::darkRed, 2));
        painter.drawEllipse(m_rayStartPoint, 8, 8);

        //        
        painter.setPen(QPen(Qt::white, 2));
        painter.drawLine(m_rayStartPoint - QPointF(5, 0), m_rayStartPoint + QPointF(5, 0));
        painter.drawLine(m_rayStartPoint - QPointF(0, 5), m_rayStartPoint + QPointF(0, 5));

        //      
        if (!m_selectingAngle) {
    QPointF direction = calculateDirectionVector(m_currentAngle);
    QLineF trajectory(m_rayStartPoint, m_rayStartPoint + direction * 80);

            painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
            painter.drawLine(trajectory);

            //          
            QPointF arrowTip = trajectory.p2();
            double angleRad = qDegreesToRadians(m_currentAngle);

            QPointF arrowLeft = arrowTip + QPointF(cos(angleRad + 2.5) * 12,
                                                   sin(angleRad + 2.5) * 12);
            QPointF arrowRight = arrowTip + QPointF(cos(angleRad - 2.5) * 12,
                                                    sin(angleRad - 2.5) * 12);

            painter.drawLine(arrowTip, arrowLeft);
            painter.drawLine(arrowTip, arrowRight);

            //    
                QString angleText = QString("%1 deg").arg(qRound(m_currentAngle));
            painter.setPen(Qt::darkRed);
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            painter.drawText(arrowTip + QPointF(10, -10), angleText);
        }
    }
}

void MirrorRoom::createRegularPolygon()
{
    m_tempPoints.clear();
    // Уменьшаем отступы, чтобы можно было строить почти полноэкранные правильные многоугольники
    QRect area = rect().adjusted(20, 20, -20, -20); // небольшая рамка, чтобы точки не прилипали к границе

    // Удаляем любые старые точки, добавляем новые строго внутри ограниченной области
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

    if (!isConvexPolygon(m_tempPoints)) {
        QMessageBox::warning(this, "Room Creation",
                             "Room must be convex. Please adjust the points to form a convex polygon.");
        return;
    }

    // Строим стены по вершинам и запоминаем центр комнаты для ориентации нормалей
    qDeleteAll(m_walls);
    m_walls.clear();

    // Create walls from points
    for (int i = 0; i < m_tempPoints.size(); ++i) {
        QPointF start = m_tempPoints[i];
        QPointF end = m_tempPoints[(i + 1) % m_tempPoints.size()];
        m_walls.append(new Wall(start, end));
    }

    // compute room centroid to orient normals consistently
    QPointF centroid(0, 0);
    double signedArea = 0.0;
    for (int i = 0; i < m_tempPoints.size(); ++i) {
        const QPointF& p0 = m_tempPoints[i];
        const QPointF& p1 = m_tempPoints[(i + 1) % m_tempPoints.size()];
        double a = p0.x() * p1.y() - p1.x() * p0.y();
        signedArea += a;
        centroid += QPointF((p0.x() + p1.x()) * a, (p0.y() + p1.y()) * a);
    }
    if (std::abs(signedArea) > 1e-9) {
        centroid /= (3.0 * signedArea);
    } else if (!m_tempPoints.isEmpty()) {
        for (const auto& p : m_tempPoints) centroid += p;
        centroid /= m_tempPoints.size();
    }
    for (Wall* w : m_walls) {
        w->setRoomCenter(centroid);
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
        const auto& path = m_currentRay->path();
        if (path.size() < 2) return;

        qint64 nowMs = m_rayTimer.isValid() ? m_rayTimer.elapsed() : 0;

        int visibleStartSegment = 0;
        // segmentTimes хранит время старта каждого сегмента (path[i] -> path[i+1])
        for (int i = 0; i < m_segmentTimes.size(); ++i) {
            if (m_segmentTimes[i] >= 0 && nowMs - m_segmentTimes[i] > m_rayLifetimeMs) {
                visibleStartSegment = i + 1;
            } else {
                break;
            }
        }

        const int lastSegmentIdx = path.size() - 2;
        const int activeSegmentIdx = qMin(m_rayStepIndex, lastSegmentIdx);
        const bool animatingSegment = (m_rayStepIndex <= lastSegmentIdx);
        double progress = 1.0;
        int segmentDuration = m_animationIntervalMs;
        if (!m_segmentDurations.isEmpty() && activeSegmentIdx >= 0 && activeSegmentIdx < m_segmentDurations.size()) {
            segmentDuration = m_segmentDurations[activeSegmentIdx];
        }
        segmentDuration = qMax(segmentDuration, m_minFrameIntervalMs);
        if (animatingSegment) {
            progress = qBound(0.0, (nowMs - m_currentSegmentStartMs) / static_cast<double>(segmentDuration), 1.0);
        }

        // Build polyline with fading head/tail
        int firstPointIdx = qMin(visibleStartSegment, path.size() - 1);
        QPainterPath polyline(path[firstPointIdx]);
        bool hasPath = true;

        int lastCompletedSegment = animatingSegment ? activeSegmentIdx - 1 : lastSegmentIdx;
        for (int seg = visibleStartSegment; seg <= lastCompletedSegment; ++seg) {
            polyline.lineTo(path[seg + 1]);
        }

        if (animatingSegment && activeSegmentIdx >= visibleStartSegment && activeSegmentIdx + 1 < path.size()) {
            QPointF start = path[activeSegmentIdx];
            QPointF end = path[activeSegmentIdx + 1];
            QPointF partial = start + (end - start) * progress;
            if (!hasPath) {
                polyline.moveTo(start);
                hasPath = true;
            }
            polyline.lineTo(partial);
        }

        if (hasPath) {
            painter.setPen(QPen(Qt::yellow, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPath(polyline);
        }

        // Red marker at emission point (always the first path point)
        painter.setBrush(Qt::red);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(path.front(), 10, 10);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawLine(path.front() + QPointF(-5, 0), path.front() + QPointF(5, 0));
        painter.drawLine(path.front() + QPointF(0, -5), path.front() + QPointF(0, 5));
    }
}

QPointF MirrorRoom::calculateRegularPolygonPoint(int index, int totalPoints, const QRect& area)
{
    // Точка вершины правильного многоугольника для индекса index (счёт от верхней точки по часовой)
    double angle = 2 * M_PI * index / totalPoints - M_PI / 2;
    // Максимальный радиус по видимой области (area уже учитывает отступы)
    double maxRadius = qMin(area.width(), area.height()) / 2.0;
    double requestedRadius = maxRadius * (m_regularPolygonScale / 100.0); // то, что хочет пользователь
    // Жёстко ограничиваем радиус, чтобы вершины гарантированно помещались внутри области area
    // safetyMargin оставляет пару пикселей, чтобы окружность не касалась края
    double safetyMargin = 4.0;
    double limitRadius = qMax(0.0, qMin(area.width(), area.height()) / 2.0 - safetyMargin);
    double radius = qMin(requestedRadius, limitRadius);

    double x = area.center().x() + radius * cos(angle);
    double y = area.center().y() + radius * sin(angle);

    return QPointF(x, y);
}

bool MirrorRoom::isConvexPolygon(const QVector<QPointF>& points) const
{
    if (points.size() < 3) return false;
    int n = points.size();
    double prevCross = 0.0;
    const double eps = 1e-9;
    for (int i = 0; i < n; ++i) {
        QPointF a = points[i];
        QPointF b = points[(i + 1) % n];
        QPointF c = points[(i + 2) % n];
        QPointF ab = b - a;
        QPointF bc = c - b;
        double cross = ab.x() * bc.y() - ab.y() * bc.x();
        if (std::abs(cross) < eps) continue; // collinear, skip
        if (prevCross == 0.0) {
            prevCross = cross;
        } else if (cross * prevCross < 0) {
            return false;
        }
    }
    return true;
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

void MirrorRoom::advanceRayAnimation()
{
    // Пошаговое продвижение по сегментам луча с учётом времени жизни
    if (!m_currentRay) {
        m_rayAnimationTimer.stop();
        return;
    }
    const auto& path = m_currentRay->path();
    if (path.size() < 2) {
        m_rayAnimationTimer.stop();
        emit simulationStateChanged(false);
        return;
    }
    qint64 nowMs = m_rayTimer.isValid() ? m_rayTimer.elapsed() : 0;

    int lastSegmentIdx = path.size() - 2;
    if (m_segmentTimes.size() < lastSegmentIdx + 1) {
        m_segmentTimes.resize(lastSegmentIdx + 1, -1);
    }

    bool needsUpdate = false;
    if (m_rayStepIndex <= lastSegmentIdx) {
        int segmentDuration = m_animationIntervalMs;
        if (!m_segmentDurations.isEmpty() && m_rayStepIndex < m_segmentDurations.size()) {
            segmentDuration = m_segmentDurations[m_rayStepIndex];
        }
        segmentDuration = qMax(segmentDuration, m_minFrameIntervalMs);
        double progress = (nowMs - m_currentSegmentStartMs) / static_cast<double>(segmentDuration);
        if (progress >= 1.0) {
            m_segmentTimes[m_rayStepIndex] = m_currentSegmentStartMs;
            ++m_rayStepIndex;
            m_currentSegmentStartMs = nowMs;
            needsUpdate = true;
        }
    }

    if (m_rayStepIndex > lastSegmentIdx) {
        if (!m_segmentTimes.isEmpty() && m_segmentTimes.front() < 0) {
            m_segmentTimes.front() = m_currentSegmentStartMs;
        }
        if (m_segmentTimes.isEmpty()) {
            m_segmentTimes.resize(lastSegmentIdx + 1, nowMs);
        }
        if (nowMs - m_segmentTimes.front() > m_rayLifetimeMs) {
            m_rayAnimationTimer.stop();
            emit simulationStateChanged(false);
        } else {
            needsUpdate = true;
        }
    } else {
        needsUpdate = true;
    }

    if (needsUpdate) {
        update();
    }
}

void MirrorRoom::setAnimationSpeed(int milliseconds)
{
    // Keep speed within a sensible range
    m_animationIntervalMs = qMax(20, milliseconds);
    m_rayAnimationTimer.setInterval(m_minFrameIntervalMs);
    rebuildSegmentDurations();
}

void MirrorRoom::setPolygonScale(int percent)
{
    // Разрешаем крупные правильные многоугольники, но ограничиваем в разумных пределах
    m_regularPolygonScale = qBound(20, percent, 75);
    if (m_creationMode == RegularPolygon) {
        clearRoom();
        createRegularPolygon();
    }
}

void MirrorRoom::updateViewTransform()
{
    ensureCameraInitialized();
    QPointF viewCenter = rect().center(); // центр виджета — точка привязки масштабирования

    QTransform t;
    t.translate(viewCenter.x(), viewCenter.y());
    t.scale(m_zoomFactor, m_zoomFactor);
    t.translate(-m_cameraCenter.x(), -m_cameraCenter.y());
    m_viewTransform = t;
    m_invViewTransform = t.inverted();
}

void MirrorRoom::adjustZoom(double factor, const QPointF& focusPoint)
{
    ensureCameraInitialized();
    updateViewTransform();

    double newZoom = qBound(kMinZoom, m_zoomFactor * factor, kMaxZoom);
    if (std::abs(newZoom - m_zoomFactor) < 1e-6) {
        return;
    }

    // Сохраняем точку под курсором — сдвигаем центр камеры так, чтобы она осталась на месте
    QPointF worldFocus = m_invViewTransform.map(focusPoint);
    double scaleRatio = m_zoomFactor / newZoom;
    m_cameraCenter = worldFocus - (worldFocus - m_cameraCenter) * scaleRatio;

    m_zoomFactor = newZoom;
    updateViewTransform();
    update();
}

void MirrorRoom::ensureCameraInitialized()
{
    if (!m_cameraInitialized) {
        m_cameraCenter = rect().center();
        m_cameraInitialized = true;
    }
}

void MirrorRoom::rebuildSegmentDurations()
{
    m_segmentDurations.clear();
    if (!m_currentRay) {
        return;
    }
    const auto& path = m_currentRay->path();
    if (path.size() < 2) return;
    double baseLen = 150.0; // pixels that take exactly m_animationIntervalMs
    for (int i = 0; i < path.size() - 1; ++i) {
        double len = QLineF(path[i], path[i + 1]).length();
        int duration = static_cast<int>(m_animationIntervalMs * (len / baseLen));
        duration = qMax(m_minFrameIntervalMs, duration);
        duration = qMin(duration, m_animationIntervalMs * 4);
        m_segmentDurations.append(duration);
    }
}

