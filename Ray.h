#pragma once
#ifndef RAY_H
#define RAY_H

#include "point.h"
#include "vector.h"
#include "line.h"

class Ray {
public:
    Ray(const Point& vertex, const Point& point_on_ray);
    Ray(const Point& vertex, const Vector& direction);
    Ray() = default;

    bool IsPointIn(const Point& point) const;
    Point GetVertex() const;
    Vector GetDirection() const;
    Point FirstOnRay(const Point& first_point, const Point& second_point) const;
    Line ToLine() const;

private:
    Point vertex_;
    Vector direction_;
};

Ray Shoot(const Ray& trajectory, const Line& wall);

#endif // RAY_H;

