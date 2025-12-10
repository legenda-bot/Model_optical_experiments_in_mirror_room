#include "ray.h"
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
    // Вектор от вершины до точки
    if (point == vertex_) {
        return false;
    }
    Vector to_point = point - vertex_;

    // Проверяем коллинеарность и что точка лежит в направлении луча
    return (direction_ * to_point == 0) && ((direction_ ^ to_point) >= 0);
}

Point Ray::GetVertex() const {
    return vertex_;
}

Vector Ray::GetDirection() const {
    return direction_;
}

Point Ray::FirstOnRay(const Point& first_point, const Point& second_point) const {
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

    Vector reflecting = wall.GetDirection();
    Vector bullet = trajectory.GetDirection();
    Point intersection = GetIntersection(trajectory.ToLine(), wall);
    
    if (!trajectory.IsPointIn(intersection) && reflecting * bullet == 0) { // if there is no intersection
        return trajectory;
    }

    std::pair<long double, long double> odds = reflecting.GetCoefficients(bullet, reflecting.GetNormal());
    Vector reflected = reflecting + reflecting.GetNormal() * 2 * odds.second;
    return { intersection, reflected };
}