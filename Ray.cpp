// Ray: ray primitive (origin + direction) used by intersection helpers.

#include "Ray.h"
#include <cmath>

Ray::Ray(const Point& vertex, const Point& point_on_ray)
    : vertex_(vertex),
    direction_(point_on_ray - vertex) {
    // Обеспечиваем ненулевой направляющий вектор
    if (direction_.SquaredLength() == 0) {
        direction_ = Vector(1, 0);
    }
}

Ray::Ray(const Point& vertex, const Vector& direction)
    : vertex_(vertex),
    direction_(direction) {
    // Обеспечиваем ненулевой направляющий вектор
    if (direction_.SquaredLength() == 0) {
        direction_ = Vector(1, 0);
    }
}

bool Ray::IsPointIn(const Point& point) const {
    if (point == vertex_) {
        return false;
    }

    // Вектор от вершины до точки
    Vector to_point = point - vertex_;

    // Проверяем коллинеарность (cross ≈ 0) и что точка лежит "впереди" вершины (dot >= 0).
    // dot здесь — обычное скалярное произведение (operator^).
    const long double cross = direction_ * to_point;
    const long double dot = direction_ ^ to_point;
    const long double scale = direction_.Length() * to_point.Length();
    constexpr long double kEps = 1e-9L;
    return (std::fabsl(cross) <= kEps * (1.0L + scale)) && (dot >= -kEps * (1.0L + scale));
}

Point Ray::GetVertex() const {
    return vertex_;
}

Vector Ray::GetDirection() const {
    return direction_;
}

Point Ray::FirstOnRay(const Point& first_point, const Point& second_point) const {
    // Выбираем точку, которая расположена ближе к вершине луча, но приоритетно среди тех,
    // что действительно лежат на луче.
    bool first_on_ray = IsPointIn(first_point);
    bool second_on_ray = IsPointIn(second_point);

    if (first_on_ray && second_on_ray) {
        // Обе точки на луче - возвращаем ближайшую к вершине
        double dist1 = (first_point - vertex_).SquaredLength();
        double dist2 = (second_point - vertex_).SquaredLength();
        return (dist1 <= dist2) ? first_point : second_point;
    }
    else if (first_on_ray) {
        return first_point;
    }
    else if (second_on_ray) {
        return second_point;
    }
    else {
        // Ни одна точка не на луче - возвращаем ближайшую к вершине
        double dist1 = (first_point - vertex_).SquaredLength();
        double dist2 = (second_point - vertex_).SquaredLength();
        return (dist1 <= dist2) ? first_point : second_point;
    }
}

Line Ray::ToLine() const {
    return Line(vertex_, direction_);
}

Ray Shoot(const Ray& trajectory, const Line& wall) { // it is actually main function in all project

    Vector incident = trajectory.GetDirection();
    Point intersection = GetIntersection(trajectory.ToLine(), wall);

    // If the intersection is not on the ray (e.g. behind the vertex), do not change direction.
    if (!trajectory.IsPointIn(intersection)) {
        return trajectory;
    }

    // Отражение направления относительно прямой:
    // берём нормаль n к стене и отражаем вектор v по формуле
    // v' = v - 2*(v·n)/(n·n) * n.
    Vector normal = wall.GetNormal();
    const long double n2 = normal.SquaredLength();
    if (n2 <= 0) {
        return trajectory;
    }

    const long double dot = incident ^ normal;
    Vector reflected = incident - normal * (2.0L * dot / n2);
    return { intersection, reflected };
}
