#include "lightray.h"
#include <QPainter>
#include <cmath>

LightRay::LightRay(const QPointF& startPoint, double startAngle, const QVector<Wall*>& walls)
    : m_startPoint(startPoint)
    , m_startAngle(startAngle)
    , m_walls(walls)
{
    calculatePath();
}

void LightRay::calculatePath(int maxReflections)
{
    m_path.clear();
    m_path.append(m_startPoint);

    QPointF currentPoint = m_startPoint;
    double currentAngle = m_startAngle;

    for (int i = 0; i < maxReflections; ++i) {
        QPointF intersection;
        const Wall* nextWall = findNextWall(currentPoint, currentAngle, intersection);

        if (!nextWall) break;

        m_path.append(intersection);
        currentPoint = intersection;

        double newAngle;
        QPointF reflectionPoint = calculateReflection(currentPoint, currentAngle, nextWall, newAngle);
        currentAngle = newAngle;
    }
}

void LightRay::draw(QPainter& painter) const
{
    if (m_path.size() < 2) return;

    QPen pen(Qt::yellow, 2);
    painter.setPen(pen);

    for (int i = 1; i < m_path.size(); ++i) {
        painter.drawLine(m_path[i-1], m_path[i]);
    }

    // Draw ray direction indicators
    painter.setPen(QPen(Qt::red, 1));
    for (int i = 0; i < m_path.size() - 1; ++i) {
        QLineF segment(m_path[i], m_path[i+1]);
        QLineF unit = segment.unitVector();
        unit.setLength(10);
        QPointF arrowP1 = unit.p2();
        unit.setAngle(unit.angle() + 30);
        QPointF arrowP2 = unit.p2();
        unit.setAngle(unit.angle() - 60);
        QPointF arrowP3 = unit.p2();

        painter.drawLine(m_path[i+1], arrowP1);
        painter.drawLine(m_path[i+1], arrowP2);
        painter.drawLine(m_path[i+1], arrowP3);
    }
}

const Wall* LightRay::findNextWall(const QPointF& currentPoint, double currentAngle,
                                   QPointF& intersection) const
{
    const Wall* closestWall = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    QLineF ray(currentPoint, currentPoint + QPointF(cos(currentAngle) * 1000,
                                                    sin(currentAngle) * 1000));

    for (const Wall* wall : m_walls) {
        QPointF intersect;
        if (ray.intersects(wall->line(), &intersect) == QLineF::BoundedIntersection) {
            double distance = QLineF(currentPoint, intersect).length();
            if (distance < minDistance && distance > 1.0) {
                minDistance = distance;
                closestWall = wall;
                intersection = intersect;
            }
        }
    }

    return closestWall;
}

QPointF LightRay::calculateReflection(const QPointF& currentPoint, double currentAngle,
                                      const Wall* wall, double& newAngle)
{
    // Simplified reflection calculation
    QLineF wallLine = wall->line();
    double wallAngle = wallLine.angle() * M_PI / 180.0;

    // Basic reflection for flat surface
    newAngle = 2 * wallAngle - currentAngle;

    return currentPoint;
}
