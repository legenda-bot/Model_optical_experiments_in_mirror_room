#include "wall.h"
// Wall: mirror/wall element (flat or spherical) with drawing and hit testing.

#include <QPainter>
#include <QPainterPath>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <cmath>
#include <limits>

namespace {
QTextStream& wallLogStream()
{
    static QFile file("wall_debug.log");
    static QTextStream stream(&file);
    if (!file.isOpen()) {
        file.open(QIODevice::Append | QIODevice::Text);
        stream << "\n--- Wall log start " << QDateTime::currentDateTime().toString(Qt::ISODate) << " ---\n";
        stream.flush();
    }
    return stream;
}

void wallLog(const QString& msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QTextStream& stream = wallLogStream();
    stream << QDateTime::currentDateTime().toString("HH:mm:ss.zzz") << " | " << msg << '\n';
    stream.flush();
}
} // namespace

namespace {
// --- Сферическая стена (дуга) ---
// В проекте "стена" хранится как хорда (отрезок) между p1 и p2.
// Если стена сферическая, мы рисуем дугу окружности, проходящую через p1 и p2, с радиусом R.
//
// Важный факт: малая дуга (<= 180°) лежит на стороне, противоположной центру окружности.
// Поэтому, чтобы дуга "смотрела" внутрь/наружу, центр окружности выбирается на противоположной стороне.
struct SphericalGeom {
    QPointF center;
    QPointF mid;
    QPointF baseNormal;
    QPointF desiredBulge;
    double radius {0.0};
    double offset {0.0};
    double chordLen {0.0};
};

SphericalGeom computeSphericalGeom(const Wall& wall)
{
    SphericalGeom g;
    const QLineF chord = wall.line();
    const double dx = chord.dx();
    const double dy = chord.dy();
    const double chordLen = std::hypot(dx, dy);
    g.chordLen = chordLen;
    if (chordLen <= 1e-9) {
        return g;
    }

    // Геометрия сферической стенки:
    // задана хорда p1-p2 и радиус R. Центр окружности лежит на перпендикуляре к хорде
    // через её середину на расстоянии offset = sqrt(R^2 - (L/2)^2), где L — длина хорды.
    // Важно: R >= L/2, иначе окружность не проходит через концы хорды.
    const QPointF mid = (chord.p1() + chord.p2()) / 2.0;
    const double halfChord = chordLen * 0.5;
    const double radius = std::max(wall.radius(), halfChord);
    const double offset = std::sqrt(std::max(0.0, radius * radius - halfChord * halfChord));
    const QPointF baseNormal(-dy / chordLen, dx / chordLen);

    QPointF interiorNormal = baseNormal;
    if (wall.hasRoomCenter()) {
        const QPointF toCenter = wall.roomCenter() - mid;
        if (QPointF::dotProduct(baseNormal, toCenter) < 0) interiorNormal = -baseNormal;
    }

    // desiredBulge — в какую сторону должна "выпучиваться" дуга:
    // - Concave: внутрь комнаты
    // - Convex: наружу комнаты
    // Выбираем "выпуклость" дуги относительно центра комнаты:
    // Concave — дуга смотрит внутрь комнаты, Convex — наружу.
    const QPointF desiredBulge = (wall.sphericalType() == Wall::Concave) ? interiorNormal : -interiorNormal;
    // Центр окружности берём на противоположной стороне (иначе малая дуга будет не там, где надо).
    // Центр окружности находится на противоположной стороне хорды относительно bulge:
    // center = mid - bulge * offset.
    const QPointF center = mid - desiredBulge * offset;

    g.center = center;
    g.mid = mid;
    g.baseNormal = baseNormal;
    g.desiredBulge = desiredBulge;
    g.radius = radius;
    g.offset = offset;
    return g;
}

bool pointOnMinorArc(const QPointF& center, const QPointF& a, const QPointF& b, const QPointF& candidate)
{
    // Проверка "точка лежит на малой дуге между a и b".
    // Используется для кликов/прилипания к дуге сферической стены.
    // Проверка принадлежности candidate меньшей дуге (minor arc) между a и b:
    // считаем углы для a,b,candidate относительно center, нормализуем в [0, 2pi),
    // берём минимальный угловой разлёт между a и b и проверяем, что candidate внутри.
    auto toAngle = [](const QPointF& p) { return std::atan2(p.y(), p.x()); };
    auto normAngle = [](double ang) {
        const double twoPi = 2.0 * M_PI;
        double res = ang;
        while (res < 0) res += twoPi;
        while (res >= twoPi) res -= twoPi;
        return res;
    };
    auto angleDiff = [](double from, double to) {
        const double twoPi = 2.0 * M_PI;
        double d = std::fmod(to - from + twoPi, twoPi);
        if (d < 0) d += twoPi;
        return d;
    };

    const QPointF va = a - center;
    const QPointF vb = b - center;
    const QPointF vc = candidate - center;
    double angA = normAngle(toAngle(va));
    double angB = normAngle(toAngle(vb));
    double angC = normAngle(toAngle(vc));

    double arcSpan = angleDiff(angA, angB);
    if (arcSpan > M_PI) {
        std::swap(angA, angB);
        arcSpan = angleDiff(angA, angB);
    }
    const double candSpan = angleDiff(angA, angC);
    constexpr double kArcEps = 1e-3;
    return candSpan <= arcSpan + kArcEps;
}

QPointF closestPointOnSphericalArc(const Wall& wall, const QPointF& point)
{
    // Возвращает ближайшую точку на дуге сферической стены к произвольной точке point.
    // Алгоритм:
    // 1) проецируем point на окружность (вдоль радиуса от центра),
    // 2) если полученная точка лежит на дуге — это ответ,
    // 3) иначе ближайшая точка будет одним из концов хорды.
    const auto g = computeSphericalGeom(wall);
    const QLineF chord = wall.line();
    if (g.chordLen <= 1e-9 || g.radius <= 1e-9) {
        return chord.p1();
    }

    // Проецируем точку point на окружность (по направлению к/от центра).
    // Это ближайшая точка на окружности, но она может не лежать на активной дуге.
    QPointF v = point - g.center;
    const double vLen = std::hypot(v.x(), v.y());
    QPointF onCircle = (vLen > 1e-9) ? (g.center + v * (g.radius / vLen)) : (g.center + g.desiredBulge * g.radius);

    // Проверяем, что проекция лежит именно на выбранной (меньшей) дуге между концами хорды.
    bool onArc = false;
    if (std::abs(g.offset) < 1e-9) {
        // Semicircle: choose half-plane by bulge direction.
        onArc = QPointF::dotProduct(onCircle - g.mid, g.desiredBulge) >= -1e-4;
    } else {
        onArc = pointOnMinorArc(g.center, chord.p1(), chord.p2(), onCircle);
    }

    if (onArc) {
        return onCircle;
    }

    // Outside arc span: closest is one of endpoints.
    const double d1 = QLineF(point, chord.p1()).length();
    const double d2 = QLineF(point, chord.p2()).length();
    return (d1 <= d2) ? chord.p1() : chord.p2();
}

double distancePointToSegment(const QPointF& point, const QLineF& seg)
{
    QPointF A = seg.p1();
    QPointF B = seg.p2();

    double l2 = (B.x() - A.x())*(B.x() - A.x()) + (B.y() - A.y())*(B.y() - A.y());
    if (l2 == 0.0) return QLineF(point, A).length(); // A == B

    // Находим параметр t проекции точки на отрезок:
    // t = ((P-A)·(B-A)) / |B-A|^2, затем ограничиваем t в [0,1].
    double t = ((point.x() - A.x()) * (B.x() - A.x()) + (point.y() - A.y()) * (B.y() - A.y())) / l2;
    t = qMax(0.0, qMin(1.0, t)); // Clamp t to [0,1]

    QPointF projection(A.x() + t * (B.x() - A.x()), A.y() + t * (B.y() - A.y()));
    return QLineF(point, projection).length();
}
} // namespace

Wall::Wall(const QPointF& start, const QPointF& end)
    : m_line(start, end)
    , m_mirrorType(Flat)
    , m_sphericalType(Concave)
    , m_radius(qMax(1e-3, QLineF(start, end).length() * 0.5))
{
}

void Wall::setRadius(double radius)
{
    if (!std::isfinite(radius)) return;

    const double minRadius = qMax(1e-3, m_line.length() * 0.5);
    // Верхний предел делаем зависимым от длины стены, чтобы "базовый" радиус (L/2)
    // всегда проходил критерии даже для очень длинных стен.
    const double maxRadius = qMax(1.0e6, minRadius * 10.0);
    const double epsAbs = 1.0;
    const double epsRel = 1e-6 * qMax(1.0, minRadius);
    const double eps = epsAbs + epsRel;
    if (radius + eps < minRadius) radius = minRadius;
    if (radius > maxRadius + eps) radius = maxRadius;
    // В пределах допуска всегда делаем радиус корректным для геометрии окружности через концы хорды.
    if (radius < minRadius) radius = minRadius;
    m_radius = radius;
}

bool Wall::containsPoint(const QPointF& point) const
{
    double distance = distanceToPoint(point);
    return distance <= 30.0; // allow easy clicking near the wall, including edges
}

void Wall::draw(QPainter& painter) const
{
    painter.save();

    QPen pen(getColor(), 9); // base stroke
    if (m_mirrorType == Spherical) {
        pen.setStyle(Qt::DashLine); // show spherical mirrors with dashes
    }
    painter.setPen(pen);
    painter.drawLine(m_line);

    // dashed hit area
    QPen hitPen(QColor(0, 0, 0, 140), 3, Qt::DashLine);
    hitPen.setCosmetic(true);
    painter.setPen(hitPen);
    painter.drawLine(m_line);

    // Direction indicator for spherical mirrors (показывает нормаль внутрь комнаты)
    if (m_mirrorType == Spherical) {
        QPointF center = (m_line.p1() + m_line.p2()) / 2;
        QLineF wallLine = m_line;
        QLineF normal = wallLine.normalVector();
        normal.setLength(30);
        normal.translate(center - normal.p1());
        if (m_hasRoomCenter) {
            QPointF mid = (m_line.p1() + m_line.p2()) / 2.0;
            QPointF toCenter = m_roomCenter - mid;
            if (QPointF::dotProduct(normal.p2() - normal.p1(), toCenter) < 0) {
                normal = QLineF(normal.p2(), normal.p1());
            }
        }

        painter.setPen(QPen(Qt::white, 3));
        QLineF arrowLine = (m_sphericalType == Concave) ? normal : QLineF(normal.p2(), normal.p1());
        painter.drawLine(arrowLine);

        arrowLine.setLength(25);
        QPointF arrowEnd = arrowLine.p2();
        QLineF perpendicular = arrowLine.normalVector();
        perpendicular.setLength(8);
        perpendicular.translate(arrowEnd - perpendicular.p1());
        painter.drawLine(arrowEnd, perpendicular.p1());
        painter.drawLine(arrowEnd, perpendicular.p2());
    }

    // Circular boundary for spherical mirrors (дуга рабочей части зеркала)
    if (m_mirrorType == Spherical) {
        QLineF chord = m_line;
        double dx = chord.dx();
        double dy = chord.dy();
        double chordLen = std::hypot(dx, dy);
        if (chordLen > 0.0) {
            QPointF mid = (chord.p1() + chord.p2()) / 2.0;
            // Окружность задаётся радиусом кривизны; дуга должна начинаться/заканчиваться
            // в концах стены и быть с нужной стороны (внутрь/наружу комнаты).
            double halfChord = chordLen * 0.5;
            double radius = m_radius;
            if (radius < halfChord) {
                radius = halfChord; // иначе окружность через концы хорды невозможна
            }

            double offset = std::sqrt(std::max(0.0, radius * radius - halfChord * halfChord));
            QPointF baseNormal(-dy / chordLen, dx / chordLen);
            QPointF interiorNormal = baseNormal;
            if (m_hasRoomCenter) {
                QPointF toCenter = m_roomCenter - mid;
                if (QPointF::dotProduct(baseNormal, toCenter) < 0) interiorNormal = -baseNormal;
            }

            // В Qt дуга задаётся начальным углом и размахом (в градусах).
            // Важно: малая дуга между концами хорды лежит на ПРОТИВОПОЛОЖНОЙ стороне от центра окружности.
            // Поэтому, чтобы дуга была "внутрь" для Concave и "наружу" для Convex, центр выбираем
            // на противоположной стороне от требуемой выпуклости.
            QPointF desiredBulge = (m_sphericalType == Concave) ? interiorNormal : -interiorNormal;
            QPointF center = mid - desiredBulge * offset;

            const double startDeg = QLineF(center, chord.p1()).angle();
            const double endDeg = QLineF(center, chord.p2()).angle();
            double spanDeg = endDeg - startDeg;
            // Нормализуем к малой дуге (|span| <= 180°), чтобы дуга гарантированно шла от p1 к p2.
            while (spanDeg <= -180.0) spanDeg += 360.0;
            while (spanDeg > 180.0) spanDeg -= 360.0;

            // Особый случай: R == |chord|/2 (полуокружность, offset==0).
            // Центр совпадает с серединой хорды, поэтому знак 180° выбираем по стороне выпуклости.
            if (std::abs(offset) < 1e-9 && std::abs(std::abs(spanDeg) - 180.0) < 1e-6) {
                QPointF midCandidateA = center + baseNormal * radius;
                QPointF midCandidateB = center - baseNormal * radius;
                QPointF chosenMid =
                    (QPointF::dotProduct(midCandidateA - center, desiredBulge) >=
                     QPointF::dotProduct(midCandidateB - center, desiredBulge))
                        ? midCandidateA
                        : midCandidateB;

                const double midDeg = QLineF(center, chosenMid).angle();
                double delta = midDeg - startDeg;
                while (delta <= -180.0) delta += 360.0;
                while (delta > 180.0) delta -= 360.0;
                spanDeg = (delta >= 0.0) ? 180.0 : -180.0;
            }

            wallLog(QString("[draw] wall=%1 type=%2 mid=(%3,%4) center=(%5,%6) radius=%7 span=%8")
                        .arg(reinterpret_cast<quintptr>(this))
                        .arg(m_sphericalType == Convex ? "convex" : "concave")
                        .arg(mid.x()).arg(mid.y())
                        .arg(center.x()).arg(center.y())
                        .arg(radius)
                        .arg(spanDeg));

            QRectF rect(center.x() - radius, center.y() - radius, radius * 2, radius * 2);
            QColor arcColor = getColor();
            arcColor.setAlpha(180);
            QPen arcPen(arcColor, 5, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
            arcPen.setCosmetic(true);
            painter.setPen(arcPen);
            painter.drawArc(rect, static_cast<int>(startDeg * 16), static_cast<int>(spanDeg * 16));
        }
    }

    // Info label
    QPointF textPos = (m_line.p1() + m_line.p2()) / 2 + QPointF(0, -20);
    QString info = getTypeString();
    if (m_mirrorType == Spherical) {
        info += QString(" R=%1").arg((int)m_radius);
    }

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(textPos, info);

    painter.restore();
}

QPointF Wall::reflectPoint(const QPointF& point) const
{
    QLineF wallLine = m_line;

    if (m_mirrorType == Flat) {
        QPointF projected = wallLine.pointAt(
            ((point.x() - wallLine.x1()) * wallLine.dx() +
             (point.y() - wallLine.y1()) * wallLine.dy()) /
            (wallLine.dx() * wallLine.dx() + wallLine.dy() * wallLine.dy())
            );

        QPointF reflection = 2 * projected - point;
        return reflection;
    } else {
        QPointF center = (wallLine.p1() + wallLine.p2()) / 2;
        QLineF toPoint(center, point);

        if (m_sphericalType == Concave) {
            toPoint.setLength(toPoint.length() * 0.8);
        } else {
            toPoint.setLength(toPoint.length() * 1.2);
        }

        return center + (toPoint.p2() - center);
    }
}

double Wall::distanceToPoint(const QPointF& point) const
{
    // Используется для выбора стены кликом.
    // Для сферической стены считаем расстояние как min(до хорды, до дуги),
    // чтобы клик по дуге тоже работал.
    const double dChord = distancePointToSegment(point, m_line);
    if (m_mirrorType != Spherical) {
        return dChord;
    }

    const QPointF closestArc = closestPointOnSphericalArc(*this, point);
    const double dArc = QLineF(point, closestArc).length();
    // allow selecting the wall by either its chord or its arc
    return std::min(dChord, dArc);
}

QPointF Wall::getClosestPoint(const QPointF& point) const
{
    if (m_mirrorType == Spherical) {
        return closestPointOnSphericalArc(*this, point);
    }

    QPointF A = m_line.p1();
    QPointF B = m_line.p2();

    double l2 = (B.x() - A.x()) * (B.x() - A.x()) + (B.y() - A.y()) * (B.y() - A.y());
    if (l2 == 0.0) return A; // A == B

    double t = ((point.x() - A.x()) * (B.x() - A.x()) + (point.y() - A.y()) * (B.y() - A.y())) / l2;
    t = qMax(0.0, qMin(1.0, t)); // Clamp t to [0,1]

    return QPointF(A.x() + t * (B.x() - A.x()), A.y() + t * (B.y() - A.y()));
}

QColor Wall::getColor() const
{
    switch (m_mirrorType) {
    case Flat:
        return QColor(0, 120, 255); // Blue for flat mirrors
    case Spherical:
        return (m_sphericalType == Concave) ? QColor(255, 70, 70) : QColor(70, 220, 90); // Red/Green
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
