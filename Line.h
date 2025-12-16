#pragma once
#ifndef LINE_H
#define LINE_H

#include "Point.h"
#include "Vector.h"

class Line {
public:
    Line(const Point& first_point, const Point& second_point);
    Line(const Point& point, const Vector& direction);
    Line() = default;

    Vector GetDirection() const;
    Vector GetNormal() const;
    Point GetPoint() const;
    Line ToLine() const { return *this; }
    bool IsPointIn(const Point& point) const;
    long double GetDistToPoint(const Point& point) const;

private:
    Point point_;
    Vector direction_;
};

Point GetIntersection(const Line& first_line, const Line& second_line);
bool IsIntersection(const Line& first_line, const Line& second_line);
#endif // LINE_H
