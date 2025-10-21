#ifndef LIGHTRAY_H
#define LIGHTRAY_H

#include <QPointF>
#include <QVector>
#include "wall.h"

class LightRay
{
public:
    LightRay(const QPointF& startPoint, double startAngle, const QVector<Wall*>& walls);

    void calculatePath(int maxReflections = 50);
    void draw(QPainter& painter) const;
    const QVector<QPointF>& path() const { return m_path; }

private:
    QPointF m_startPoint;
    double m_startAngle;
    QVector<Wall*> m_walls;
    QVector<QPointF> m_path;

    QPointF calculateReflection(const QPointF& currentPoint, double currentAngle,
                                const Wall* wall, double& newAngle);
    const Wall* findNextWall(const QPointF& currentPoint, double currentAngle,
                             QPointF& intersection) const;
};

#endif // LIGHTRAY_H
