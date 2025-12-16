#include "lightray.h"
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <limits>

namespace {
bool pointOnArcHelper(const QPointF& center, const QPointF& a, const QPointF& b,
                      const QPointF& candidate, bool allowMajor)
{
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

    QPointF va = a - center;
    QPointF vb = b - center;
    QPointF vc = candidate - center;
    double angA = normAngle(toAngle(va));
    double angB = normAngle(toAngle(vb));
    double angC = normAngle(toAngle(vc));
    double arcSpan = angleDiff(angA, angB);
    double majorSpan = (2.0 * M_PI) - arcSpan;
    if (arcSpan > M_PI) {
        std::swap(angA, angB);
        arcSpan = angleDiff(angA, angB);
        majorSpan = (2.0 * M_PI) - arcSpan;
    }
    double candSpan = angleDiff(angA, angC);
    constexpr double kArcEps = 1e-4;
    if (!allowMajor) {
        return candSpan <= arcSpan + kArcEps;
    }
    return candSpan <= arcSpan + kArcEps || candSpan >= majorSpan - kArcEps;
}
} // namespace

namespace {
struct SphericalGeom {
    QPointF center;
    QPointF mid;
    QPointF baseNormal;
    QPointF desiredBulge;
    double radius {0.0};
    double offset {0.0};
    double chordLen {0.0};
};

bool computeSphericalGeom(const Wall* wall, SphericalGeom& out)
{
    if (!wall || wall->mirrorType() != Wall::Spherical) return false;
    const QLineF chord = wall->line();
    const double dx = chord.dx();
    const double dy = chord.dy();
    const double chordLen = std::hypot(dx, dy);
    if (chordLen <= 1e-9) return false;

    const QPointF mid = (chord.p1() + chord.p2()) / 2.0;
    const double halfChord = chordLen * 0.5;
    const double radius = std::max(wall->radius(), halfChord);
    const double offset = std::sqrt(std::max(0.0, radius * radius - halfChord * halfChord));
    const QPointF baseNormal(-dy / chordLen, dx / chordLen);

    QPointF interiorNormal = baseNormal;
    if (wall->hasRoomCenter()) {
        const QPointF toCenter = wall->roomCenter() - mid;
        if (QPointF::dotProduct(baseNormal, toCenter) < 0) interiorNormal = -baseNormal;
    }

    // desired bulge direction for rendered/active arc
    const QPointF desiredBulge = (wall->sphericalType() == Wall::Concave) ? interiorNormal : -interiorNormal;
    // Center must be on the opposite side from the bulge (minor arc lies opposite the center).
    const QPointF center = mid - desiredBulge * offset;

    out.center = center;
    out.mid = mid;
    out.baseNormal = baseNormal;
    out.desiredBulge = desiredBulge;
    out.radius = radius;
    out.offset = offset;
    out.chordLen = chordLen;
    return true;
}

bool sphericalArcContainsPoint(const Wall* wall, const SphericalGeom& g, const QPointF& p)
{
    const QLineF chord = wall->line();
    constexpr double kArcEps = 1e-4;

    // Special case: R == |chord|/2 => semicircle, direction is ambiguous from angles alone.
    // Select the half-plane by bulge direction.
    if (std::abs(g.offset) < 1e-9) {
        const double side = QPointF::dotProduct(p - g.mid, g.desiredBulge);
        return side >= -kArcEps;
    }

    // Always use the minor arc between endpoints for reflections (matches rendering).
    return pointOnArcHelper(g.center, chord.p1(), chord.p2(), p, /*allowMajor*/ false);
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
    m_path.append(m_startPoint);

    QPointF currentPoint = m_startPoint;
    double currentAngle = m_startAngle;
    const Wall* lastWall = nullptr;

    for (int i = 0; i < maxReflections; ++i) {
        QPointF intersection;
        const Wall* nextWall = findNextWall(currentPoint, currentAngle, intersection, lastWall);

        if (!nextWall) {
            const double kFallbackLen = 10000.0;
            QPointF dir(std::cos(currentAngle), std::sin(currentAngle));
            m_path.append(currentPoint + dir * kFallbackLen);
            break;
        }

        m_path.append(intersection);
        double newAngle;
        calculateReflection(intersection, currentAngle, nextWall, newAngle);
        currentAngle = newAngle;
        lastWall = nextWall;

        constexpr double kStepAfterHit = 0.05;
        QPointF forward(std::cos(newAngle), std::sin(newAngle));
        currentPoint = intersection + forward * kStepAfterHit;
    }
}

const Wall* LightRay::findNextWall(const QPointF& currentPoint, double currentAngle,
                                   QPointF& intersection, const Wall* skipWall) const
{
    const Wall* closestWall = nullptr;
    QPointF closestIntersection;
    double minDistance = std::numeric_limits<double>::max();
    double bestIncidence = -1.0;
    const Wall* deferredSameWall = nullptr;
    QPointF deferredSameIntersection;
    double deferredSameDistance = std::numeric_limits<double>::max();

    const QPointF direction(std::cos(currentAngle), std::sin(currentAngle));
    const double dirLen = std::hypot(direction.x(), direction.y());
    QPointF dirUnit = (dirLen > 0.0) ? QPointF(direction.x() / dirLen, direction.y() / dirLen) : QPointF(1.0, 0.0);

    constexpr double kMinTravel = 1e-4;
    constexpr double kSkipSameWallDistance = 0.05;
    constexpr double kCornerMergeEps = 0.05;
    constexpr double kRayLength = 10000.0;

    for (const Wall* wall : m_walls) {
        QPointF intersect;
        double distance = std::numeric_limits<double>::max();
        double incidence = -1.0;

        QLineF chord = wall->line();
        double dx = chord.dx();
        double dy = chord.dy();
        double chordLen = std::hypot(dx, dy);
        if (chordLen <= kMinTravel) {
            continue;
        }

        const bool useSphericalArc = (wall->mirrorType() == Wall::Spherical && wall->sphericalType() == Wall::Concave);
        if (useSphericalArc) {
            SphericalGeom g;
            if (!computeSphericalGeom(wall, g)) {
                continue;
            }
            const QPointF center = g.center;
            const double radius = g.radius;

            QPointF m = currentPoint - center;
            double bq = dirUnit.x() * m.x() + dirUnit.y() * m.y();
            double c = m.x() * m.x() + m.y() * m.y() - radius * radius;
            double discriminant = bq * bq - c;
            if (discriminant < 0.0) {
                qDebug() << "[Sph] skip disc<0 wall" << wall << "disc" << discriminant;
                continue;
            }
            double sqrtDisc = std::sqrt(discriminant);
            double t1 = -bq - sqrtDisc;
            double t2 = -bq + sqrtDisc;
            if (t1 <= kMinTravel && t2 <= kMinTravel) {
                qDebug() << "[Sph] skip both t<=eps" << t1 << t2;
                continue;
            }
            double tCandidate = std::numeric_limits<double>::max();
            if (t1 > kMinTravel) tCandidate = t1;
            if (t2 > kMinTravel && t2 < tCandidate) tCandidate = t2;

            QPointF candidate = currentPoint + dirUnit * tCandidate;
            if (!sphericalArcContainsPoint(wall, g, candidate)) {
                qDebug() << "[Sph] skip off-arc" << candidate;
                continue;
            }

            QPointF nVec = candidate - center;
            double nLen = std::hypot(nVec.x(), nVec.y());
            if (nLen <= kMinTravel) {
                qDebug() << "[Sph] skip nLen<=eps" << nLen;
                continue;
            }
            QPointF normal(nVec.x() / nLen, nVec.y() / nLen);
            double facing = dirUnit.x() * normal.x() + dirUnit.y() * normal.y();
            // Note: allow reflecting from either side; for typical room setups this still hits the
            // intended "front" face, and avoids missing valid hits for rotated/semicircle cases.

            intersect = candidate;
            distance = tCandidate;
            incidence = std::abs(facing);
            qDebug() << "[Sph] hit wall" << wall << "candidate" << candidate << "t" << tCandidate << "facing" << facing;
        } else {
            QPointF p = currentPoint;
            QPointF r = dirUnit * kRayLength;
            QPointF q = chord.p1();
            QPointF s = chord.p2() - chord.p1();

            double rxs = r.x() * s.y() - r.y() * s.x();
            if (std::abs(rxs) < 1e-9) {
                qDebug() << "[Flat] skip parallel" << wall;
                continue;
            }
            QPointF qp = q - p;
            double t = (qp.x() * s.y() - qp.y() * s.x()) / rxs;
            double u = (qp.x() * r.y() - qp.y() * r.x()) / rxs;
            const double kSegEps = 1e-6;
            if (t <= kMinTravel || u < -kSegEps || u > 1.0 + kSegEps) {
                qDebug() << "[Flat] skip t/u" << t << u;
                continue;
            }

            intersect = p + dirUnit * (t * kRayLength);
            distance = QLineF(currentPoint, intersect).length();

            QPointF normal(-dy / chordLen, dx / chordLen);
            incidence = std::abs(dirUnit.x() * normal.x() + dirUnit.y() * normal.y());
            qDebug() << "[Flat] hit wall" << wall << "candidate" << intersect << "t" << t << "u" << u;
        }

        if (distance <= kMinTravel) {
            continue;
        }
        if (wall == skipWall && distance < kSkipSameWallDistance) {
            if (distance < deferredSameDistance) {
                deferredSameDistance = distance;
                deferredSameWall = wall;
                deferredSameIntersection = intersect;
            }
            qDebug() << "[SkipSame]" << wall << "dist" << distance;
            continue;
        }

        if (distance < minDistance - kCornerMergeEps) {
            minDistance = distance;
            bestIncidence = incidence;
            closestWall = wall;
            closestIntersection = intersect;
        } else if (std::abs(distance - minDistance) <= kCornerMergeEps && incidence > bestIncidence) {
            bestIncidence = incidence;
            closestWall = wall;
            closestIntersection = intersect;
        }
    }

    if (closestWall) {
        intersection = closestIntersection;
    } else if (deferredSameWall) {
        intersection = deferredSameIntersection;
        closestWall = deferredSameWall;
    }

    return closestWall;
}

QPointF LightRay::calculateReflection(const QPointF& currentPoint, double currentAngle,
                                      const Wall* wall, double& newAngle)
{
    QLineF wallLine = wall->line();
    double dx = wallLine.dx();
    double dy = wallLine.dy();
    double len = std::hypot(dx, dy);
    if (len == 0.0) {
        newAngle = currentAngle;
        return currentPoint;
    }

    QPointF dir(std::cos(currentAngle), std::sin(currentAngle));
    QPointF normal(-dy / len, dx / len);

    // Согласуем с отрисовкой:
    // - Concave: дуга обращена внутрь комнаты -> отражаем от дуги
    // - Convex: дуга снаружи комнаты, внутренняя граница — хорда -> отражаем как от плоской стены
    if (wall->mirrorType() == Wall::Spherical && wall->sphericalType() == Wall::Concave) {
        SphericalGeom g;
        if (computeSphericalGeom(wall, g)) {
            QPointF nVec = currentPoint - g.center;
            double nLen = std::hypot(nVec.x(), nVec.y());
            if (nLen > 1e-9) {
                normal = QPointF(nVec.x() / nLen, nVec.y() / nLen);
            }
        }
    }

    double dot = dir.x() * normal.x() + dir.y() * normal.y();
    QPointF reflected(dir.x() - 2 * dot * normal.x(),
                      dir.y() - 2 * dot * normal.y());
    newAngle = std::atan2(reflected.y(), reflected.x());
    return currentPoint;
}
