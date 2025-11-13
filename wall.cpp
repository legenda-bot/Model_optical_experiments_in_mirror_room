#include "wall.h"
#include <QPainter>
#include <cmath>

Wall::Wall(const QPointF& start, const QPointF& end)
    : m_line(start, end)
    , m_mirrorType(Flat)
    , m_sphericalType(Concave)
    , m_radius(100.0)
{
}

bool Wall::containsPoint(const QPointF& point) const
{
    // Проверяем расстояние до всей линии стены, а не только до конечных точек
    double distance = distanceToPoint(point);
    return distance < 25.0; // Достаточно большой порог для выбора всей стены
}

void Wall::draw(QPainter& painter) const
{
    // Сохраняем состояние painter
    painter.save();

    QPen pen(getColor(), 8); // Еще толще для легкого выбора
    painter.setPen(pen);
    painter.drawLine(m_line);

    // Draw direction indicator for spherical mirrors
    if (m_mirrorType == Spherical) {
        QPointF center = (m_line.p1() + m_line.p2()) / 2;

        // Вычисляем нормаль к стене
        QLineF wallLine = m_line;
        QLineF normal = wallLine.normalVector();

        // Устанавливаем длину нормали
        normal.setLength(30);

        // Центрируем нормаль на середине стены
        normal.translate(center - normal.p1());

        if (m_sphericalType == Concave) {
            // Вогнутое зеркало - стрелка внутрь
            painter.setPen(QPen(Qt::white, 3));
            painter.drawLine(center, normal.p2());

            // Стрелка
            QLineF arrowLine(center, normal.p2());
            arrowLine.setLength(25);
            QPointF arrowEnd = arrowLine.p2();

            // Перпендикуляры для стрелки
            QLineF perpendicular = arrowLine.normalVector();
            perpendicular.setLength(8);
            perpendicular.translate(arrowEnd - perpendicular.p1());

            QPointF arrow1 = perpendicular.p1();
            QPointF arrow2 = perpendicular.p2();

            painter.drawLine(arrowEnd, arrow1);
            painter.drawLine(arrowEnd, arrow2);
        } else {
            // Выпуклое зеркало - стрелка наружу
            QLineF reverseNormal(center, 2 * center - normal.p2());
            painter.setPen(QPen(Qt::white, 3));
            painter.drawLine(center, reverseNormal.p2());

            // Стрелка
            QLineF arrowLine(center, reverseNormal.p2());
            arrowLine.setLength(25);
            QPointF arrowEnd = arrowLine.p2();

            // Перпендикуляры для стрелки
            QLineF perpendicular = arrowLine.normalVector();
            perpendicular.setLength(8);
            perpendicular.translate(arrowEnd - perpendicular.p1());

            QPointF arrow1 = perpendicular.p1();
            QPointF arrow2 = perpendicular.p2();

            painter.drawLine(arrowEnd, arrow1);
            painter.drawLine(arrowEnd, arrow2);
        }
    }

    // Draw wall info
    QPointF textPos = (m_line.p1() + m_line.p2()) / 2 + QPointF(0, -20);
    QString info = getTypeString();
    if (m_mirrorType == Spherical) {
        info += QString(" R=%1").arg((int)m_radius);
    }

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(textPos, info);

    // Восстанавливаем состояние painter
    painter.restore();
}

QPointF Wall::reflectPoint(const QPointF& point) const
{
    // Simplified reflection logic
    QLineF wallLine = m_line;

    if (m_mirrorType == Flat) {
        // Basic flat mirror reflection
        QPointF projected = wallLine.pointAt(
            ((point.x() - wallLine.x1()) * wallLine.dx() +
             (point.y() - wallLine.y1()) * wallLine.dy()) /
            (wallLine.dx() * wallLine.dx() + wallLine.dy() * wallLine.dy())
            );

        QPointF reflection = 2 * projected - point;
        return reflection;
    } else {
        // Simplified spherical mirror reflection
        QPointF center = (wallLine.p1() + wallLine.p2()) / 2;
        QLineF toPoint(center, point);

        if (m_sphericalType == Concave) {
            toPoint.setLength(toPoint.length() * 0.8); // Simplified focus effect
        } else {
            toPoint.setLength(toPoint.length() * 1.2); // Simplified divergence effect
        }

        return center + (toPoint.p2() - center);
    }
}

double Wall::distanceToPoint(const QPointF& point) const
{
    // Вычисляем расстояние от точки до отрезка AB
    QPointF A = m_line.p1();
    QPointF B = m_line.p2();

    double l2 = (B.x() - A.x())*(B.x() - A.x()) + (B.y() - A.y())*(B.y() - A.y());
    if (l2 == 0.0) return QLineF(point, A).length(); // A == B

    // Consider the line extending the segment, parameterized as A + t (B - A)
    double t = ((point.x() - A.x()) * (B.x() - A.x()) + (point.y() - A.y()) * (B.y() - A.y())) / l2;
    t = qMax(0.0, qMin(1.0, t)); // Clamp t to [0,1]

    QPointF projection(A.x() + t * (B.x() - A.x()), A.y() + t * (B.y() - A.y()));
    return QLineF(point, projection).length();
}

QPointF Wall::getClosestPoint(const QPointF& point) const
{
    // Находим ближайшую точку на отрезке AB к заданной точке
    QPointF A = m_line.p1();
    QPointF B = m_line.p2();

    double l2 = (B.x() - A.x())*(B.x() - A.x()) + (B.y() - A.y())*(B.y() - A.y());
    if (l2 == 0.0) return A; // A == B

    // Consider the line extending the segment, parameterized as A + t (B - A)
    double t = ((point.x() - A.x()) * (B.x() - A.x()) + (point.y() - A.y()) * (B.y() - A.y())) / l2;
    t = qMax(0.0, qMin(1.0, t)); // Clamp t to [0,1]

    return QPointF(A.x() + t * (B.x() - A.x()), A.y() + t * (B.y() - A.y()));
}

QColor Wall::getColor() const
{
    switch (m_mirrorType) {
    case Flat:
        return QColor(0, 100, 200); // Blue for flat mirrors
    case Spherical:
        return (m_sphericalType == Concave) ? QColor(200, 0, 0) : QColor(0, 150, 0); // Red/Green
    default:
        return Qt::black;
    }
}

QString Wall::getTypeString() const
{
    switch (m_mirrorType) {
    case Flat: return "Flat";
    case Spherical: return getSphericalTypeString();
    default: return "Unknown";
    }
}

QString Wall::getSphericalTypeString() const
{
    switch (m_sphericalType) {
    case Concave: return "Concave";
    case Convex: return "Convex";
    default: return "Unknown";
    }
}

QString Wall::toString() const
{
    return QString("Wall: (%1,%2) -> (%3,%4), Type: %5, Radius: %6")
    .arg(m_line.p1().x())
        .arg(m_line.p1().y())
        .arg(m_line.p2().x())
        .arg(m_line.p2().y())
        .arg(getTypeString())
        .arg(m_radius);
}
