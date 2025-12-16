#pragma once
#ifndef ARC_H
#define ARC_H

#include "Point.h"
#include "Circle.h"

class Arc {
public:
    Arc(const Point& start_point, const Point& middle_point, const Point& end_point);
    Arc() = default;

    bool IsPointIn(const Point& point) const;
    Circle ToCircle() const;
    Point GetStartPoint() const;
    Point GetEndPoint() const;
    Point GetMiddlePoint() const;
    bool operator == (const Arc other) const;

private:
    Point start_point_;
    Point middle_point_;
    Point end_point_;
    Circle circle_;
};

#endif // ARC_H
