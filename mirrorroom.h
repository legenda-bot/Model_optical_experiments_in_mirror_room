#ifndef MIRRORROOM_H
#define MIRRORROOM_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include "wall.h"
#include "lightray.h"

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

public slots:
    void setRoomCreationMode(MirrorRoom::RoomCreationMode mode);
    void setNumberOfWalls(int count);
    void startRayExperiment(const QPointF& startPoint, double angle);
    void startRayExperiment(double angle);
    void clearRoom();
    void saveExperiment(const QString& filename);
    void loadExperiment(const QString& filename);

    // Методы для доступа к стенам
    Wall* getWall(int index) const {
        return (index >= 0 && index < m_walls.size()) ? m_walls[index] : nullptr;
    }
    const QVector<Wall*>& getWalls() const { return m_walls; }

    // Методы для установки режима выбора точки и угла
    void setSelectingStartPoint(bool selecting) {
        m_selectingStartPoint = selecting;
        if (selecting) {
            m_selectingAngle = false;
        }
        update();
    }
    void setSelectingAngle(bool selecting) {
        m_selectingAngle = selecting;
        if (selecting && !m_rayStartPoint.isNull()) {
            // Начальное направление - вправо (0°)
            m_angleSelectionPoint = m_rayStartPoint + QPointF(50, 0);
        }
        update();
    }
    void setCurrentAngle(double angle) {
        m_currentAngle = angle;
        update();
    }
    QPointF getRayStartPoint() const { return m_rayStartPoint; }
    double getCurrentAngle() const { return m_currentAngle; }

signals:
    // Сигнал выбора стены
    void wallSelected(int wallIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    RoomCreationMode m_creationMode;
    QVector<Wall*> m_walls;
    QVector<QPointF> m_tempPoints;
    LightRay* m_currentRay;
    int m_regularWallsCount;
    bool m_roomCompleted;
    QPointF m_rayStartPoint;
    bool m_selectingStartPoint;
    bool m_selectingAngle;
    QPointF m_angleSelectionPoint;
    double m_currentAngle;

    void createRegularPolygon();
    void completeRoom();
    void drawWalls(QPainter& painter);
    void drawRay(QPainter& painter);
    void drawAngleSelection(QPainter& painter);
    void drawStartPointAndTrajectory(QPainter& painter);
    QPointF calculateRegularPolygonPoint(int index, int totalPoints, const QRect& area);
    Wall* findWallAtPoint(const QPointF& point) const;
    double calculateAngle(const QPointF& start, const QPointF& end) const;
    QPointF calculateDirectionVector(double angle) const;
};

#endif // MIRRORROOM_H
