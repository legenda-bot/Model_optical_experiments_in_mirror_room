// Segment: segment representation and helper operations.

#pragma once
#ifndef SEGMENT_H
#define SEGMENT_H

#include "Point.h"
#include "Vector.h"
#include "Line.h"

class Segment {
public:
    Segment(const Point& start_point, const Point& end_point);
    Segment(const Point& start_point, const Vector& direction_vector);
    Segment() = default;

    bool IsPointIn(const Point& point) const;
    Vector GetDirection() const;
    Point GetStart() const;
    Point GetEnd() const;
    Line ToLine() const;
    bool operator == (const Segment& other) const;

private:
    Point start_point_;
    Vector direction_vector_;
};

#endif // SEGMENT_H
