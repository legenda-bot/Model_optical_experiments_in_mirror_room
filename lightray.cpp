#include "lightray.h"
// LightRay: backend-driven ray tracing (geometry engine lives in backend/).

#include "backend/BackendPolygon.h"
#include "backend/BackendWall.h"

#include <QPainter>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <vector>

namespace {
Point toBackendPoint(const QPointF& p)
{
    return Point(static_cast<long double>(p.x()), static_cast<long double>(p.y()));
}

QPointF toQtPoint(const Point& p)
{
    return QPointF(static_cast<double>(p.x_), static_cast<double>(p.y_));
}

BackendWall toBackendWall(const Wall* wall)
{
    const QPointF a = wall->startPoint();
    const QPointF b = wall->endPoint();

    if (wall->mirrorType() == Wall::Flat) {
        return BackendWall(toBackendPoint(a), toBackendPoint(b));
    }

    // Конвертация UI-сферической стенки в backend-дугу:
    // В UI дуга задаётся хордой (a,b) и радиусом R. Для backend нужен Arc(start,middle,end).
    // Выбираем "середину дуги" как точку на дуге, отстоящую от середины хорды на величину стрелы:
    // sagitta = R - sqrt(R^2 - (L/2)^2), где L — длина хорды.
    const double dx = b.x() - a.x();
    const double dy = b.y() - a.y();
    const double chordLen = std::hypot(dx, dy);
    if (chordLen <= 1e-9) {
        return BackendWall(toBackendPoint(a), toBackendPoint(b));
    }

    const QPointF mid = (a + b) / 2.0;
    const double halfChord = chordLen * 0.5;
    const double radius = std::max(wall->radius(), halfChord);
    const double offset = std::sqrt(std::max(0.0, radius * radius - halfChord * halfChord));
    const double sagitta = radius - offset;

    // Нормаль к хорде (единичная). Направление "внутрь" выбираем по центру комнаты, чтобы
    // выпуклость/вогнутость была стабильной при любой ориентации многоугольника.
    const QPointF baseNormal(-dy / chordLen, dx / chordLen);
    QPointF interiorNormal = baseNormal;
    if (wall->hasRoomCenter()) {
        const QPointF toCenter = wall->roomCenter() - mid;
        if (QPointF::dotProduct(baseNormal, toCenter) < 0) {
            interiorNormal = -baseNormal;
        }
    }

    const QPointF bulge = (wall->sphericalType() == Wall::Concave) ? interiorNormal : -interiorNormal;
    const QPointF middleOnArc = mid + bulge * sagitta;

    return BackendWall(toBackendPoint(a), toBackendPoint(middleOnArc), toBackendPoint(b));
}
} // namespace

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

    try {
        BackendMirrorRoom room;
        for (const Wall* wall : m_walls) {
            room.AddWall(toBackendWall(wall));
        }

        const Vector dir(static_cast<long double>(std::cos(m_startAngle)),
                         static_cast<long double>(std::sin(m_startAngle)));
        const Ray beam(toBackendPoint(m_startPoint), dir);

        std::vector<Point> points;
        room.FireBeam(beam, points, maxReflections + 2);

        for (const Point& p : points) {
            m_path.append(toQtPoint(p));
        }
    } catch (const std::exception& e) {
        m_path.append(m_startPoint);
        const double kFallbackLen = 10000.0;
        const QPointF dir(std::cos(m_startAngle), std::sin(m_startAngle));
        m_path.append(m_startPoint + dir * kFallbackLen);

        QFile file("ray_debug.log");
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << "\n--- LightRay backend failure " << QDateTime::currentDateTime().toString(Qt::ISODate) << " ---\n";
            out << "Exception: " << e.what() << "\n";
            out << "Walls: " << m_walls.size() << "\n";
        }
    } catch (...) {
        m_path.append(m_startPoint);
        const double kFallbackLen = 10000.0;
        const QPointF dir(std::cos(m_startAngle), std::sin(m_startAngle));
        m_path.append(m_startPoint + dir * kFallbackLen);
    }
}

void LightRay::draw(QPainter& painter) const
{
    painter.save();
    painter.setPen(QPen(Qt::yellow, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for (int i = 1; i < m_path.size(); ++i) {
        painter.drawLine(m_path[i - 1], m_path[i]);
    }
    painter.restore();
}
