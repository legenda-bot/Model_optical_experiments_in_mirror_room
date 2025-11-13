<<<<<<< HEAD
#ifndef WALL_H
#define WALL_H

#include <QPointF>
#include <QLineF>
#include <QPainter>
#include <QString>

class Wall
{
public:
    enum MirrorType {
        Flat,
        Spherical
    };

    enum SphericalType {
        Concave,
        Convex
    };

    Wall(const QPointF& start, const QPointF& end);

    // Setters
    void setMirrorType(MirrorType type) { m_mirrorType = type; }
    void setSphericalType(SphericalType type) { m_sphericalType = type; }
    void setRadius(double radius) { m_radius = radius; }

    // Getters
    MirrorType mirrorType() const { return m_mirrorType; }
    SphericalType sphericalType() const { return m_sphericalType; }
    double radius() const { return m_radius; }
    QLineF line() const { return m_line; }
    QPointF startPoint() const { return m_line.p1(); }
    QPointF endPoint() const { return m_line.p2(); }
    double length() const { return m_line.length(); }

    bool containsPoint(const QPointF& point) const;
    void draw(QPainter& painter) const;
    QPointF reflectPoint(const QPointF& point) const;
    double distanceToPoint(const QPointF& point) const;
    QPointF getClosestPoint(const QPointF& point) const;

    QString toString() const;

private:
    QLineF m_line;
    MirrorType m_mirrorType;
    SphericalType m_sphericalType;
    double m_radius;

    QColor getColor() const;
    QString getTypeString() const;
    QString getSphericalTypeString() const;
};

#endif // WALL_H
=======
#ifndef WALL_H
#define WALL_H

#include <QPointF>
#include <QLineF>
#include <QPainter>
#include <QString>

class Wall
{
public:
    enum MirrorType {
        Flat,
        Spherical
    };

    enum SphericalType {
        Concave,
        Convex
    };

    Wall(const QPointF& start, const QPointF& end);

    // Setters
    void setMirrorType(MirrorType type) { m_mirrorType = type; }
    void setSphericalType(SphericalType type) { m_sphericalType = type; }
    void setRadius(double radius) { m_radius = radius; }

    // Getters
    MirrorType mirrorType() const { return m_mirrorType; }
    SphericalType sphericalType() const { return m_sphericalType; }
    double radius() const { return m_radius; }
    QLineF line() const { return m_line; }
    QPointF startPoint() const { return m_line.p1(); }
    QPointF endPoint() const { return m_line.p2(); }
    double length() const { return m_line.length(); }

    bool containsPoint(const QPointF& point) const;
    void draw(QPainter& painter) const;
    QPointF reflectPoint(const QPointF& point) const;
    double distanceToPoint(const QPointF& point) const;

    QString toString() const;

private:
    QLineF m_line;
    MirrorType m_mirrorType;
    SphericalType m_sphericalType;
    double m_radius;

    QColor getColor() const;
    QString getTypeString() const;
    QString getSphericalTypeString() const;
};

#endif // WALL_H
>>>>>>> dbcaaaab8187a40d6d56fbc2b69f275e8cfb3978
