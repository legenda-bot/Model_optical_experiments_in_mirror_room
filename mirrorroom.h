// MirrorRoom: owns the room geometry and orchestrates rendering/interaction.

#ifndef MIRRORROOM_H
#define MIRRORROOM_H

#include <QElapsedTimer>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QString>
#include <QTimer>
#include <QTransform>
#include <QVector>
#include <QWidget>

#include "lightray.h"
#include "wall.h"

class QPaintEvent;
class QMouseEvent;
class QKeyEvent;
class QPainter;

class MirrorRoom : public QWidget
{
    Q_OBJECT

public:
    explicit MirrorRoom(QWidget *parent = nullptr);
    ~MirrorRoom();

    enum RoomCreationMode {
        DrawByClick,
        RegularPolygon
    };

    RoomCreationMode creationMode() const { return m_creationMode; }
    void setRoomCreationMode(RoomCreationMode mode);
    void setNumberOfWalls(int count);

    void startRayExperiment(const QPointF& startPoint, double angle);
    void startRayExperiment(double angle);

    void clearRay();
    void clearRoom();

    void setSelectingStartPoint(bool selecting) {
        m_selectingStartPoint = selecting;
        if (selecting) {
            m_selectingAngle = false;
            m_rayStartPoint = QPointF();
        }
        update();
    }

    void setSelectingAngle(bool selecting) {
        m_selectingAngle = selecting;
        if (selecting && !m_rayStartPoint.isNull()) {
            m_angleSelectionPoint = m_rayStartPoint + QPointF(50, 0);
        }
        update();
    }

    void setCurrentAngle(double angle) {
        m_currentAngle = angle;
        update();
    }

    QPointF getRayStartPoint() const { return m_rayStartPoint; }
    const QVector<Wall*>& getWalls() const { return m_walls; }
    Wall* getWall(int index) const {
        return (index >= 0 && index < m_walls.size()) ? m_walls[index] : nullptr;
    }

    void setAnimationSpeed(int milliseconds);
    void setPolygonScale(int percent);
    double currentAngle() const { return m_currentAngle; }

    void saveExperiment(const QString& filename);
    void loadExperiment(const QString& filename);

signals:
    void wallSelected(int wallIndex);
    void angleUpdated(double angle);
    void simulationStateChanged(bool running);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void drawAngleSelection(QPainter& painter);
    void drawStartPointAndTrajectory(QPainter& painter);
    void createRegularPolygon();
    void completeRoom();
    void drawWalls(QPainter& painter);
    void drawRay(QPainter& painter);
    QPointF calculateRegularPolygonPoint(int index, int totalPoints, const QRect& area);
    bool isConvexPolygon(const QVector<QPointF>& points) const;
    Wall* findWallAtPoint(const QPointF& point) const;
    double calculateAngle(const QPointF& start, const QPointF& end) const;
    QPointF calculateDirectionVector(double angle) const;
    void advanceRayAnimation();
    void updateViewTransform();
    void adjustZoom(double factor, const QPointF& focusPoint);
    void ensureCameraInitialized();
    void rebuildSegmentDurations();

    RoomCreationMode m_creationMode;
    QVector<Wall*> m_walls;
    QVector<QPointF> m_tempPoints;
    LightRay* m_currentRay;

    int m_rayStepIndex;
    int m_minFrameIntervalMs;
    qint64 m_currentSegmentStartMs;
    int m_animationIntervalMs;
    int m_rayLifetimeMs;
    int m_regularWallsCount;
    int m_regularPolygonScale;

    bool m_roomCompleted;
    QPointF m_rayStartPoint;
    bool m_selectingStartPoint;
    bool m_selectingAngle;
    QPointF m_angleSelectionPoint;
    double m_currentAngle;

    QVector<qint64> m_segmentTimes;
    QVector<int> m_segmentDurations;
    QTimer m_rayAnimationTimer;
    QElapsedTimer m_rayTimer;

    double m_zoomFactor;
    QPointF m_cameraCenter;
    bool m_cameraInitialized;
    bool m_isPanning;
    QPoint m_lastPanPos;
    QTransform m_viewTransform;
    QTransform m_invViewTransform;
};

#endif // MIRRORROOM_H
