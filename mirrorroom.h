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
    void clearRoom();
    void saveExperiment(const QString& filename);
    void loadExperiment(const QString& filename);

    // Добавляем методы для доступа к стенам
    Wall* getWall(int index) const {
        return (index >= 0 && index < m_walls.size()) ? m_walls[index] : nullptr;
    }
    const QVector<Wall*>& getWalls() const { return m_walls; }

signals:
    // Добавляем сигнал выбора стены
    void wallSelected(int wallIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    RoomCreationMode m_creationMode;
    QVector<Wall*> m_walls;
    QVector<QPointF> m_tempPoints;
    LightRay* m_currentRay;
    int m_regularWallsCount;
    bool m_roomCompleted;

    void createRegularPolygon();
    void completeRoom();
    void drawWalls(QPainter& painter);
    void drawRay(QPainter& painter);
    QPointF calculateRegularPolygonPoint(int index, int totalPoints, const QRect& area);
};

#endif // MIRRORROOM_H
