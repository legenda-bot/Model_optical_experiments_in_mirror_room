// Point: lightweight 2D point wrapper (project-specific).

#include "Point.h"

Point::Point(long double x, long double y) : x_(x), y_(y) {}

bool Point::operator==(const Point& other) const {
    return x_ == other.x_ && y_ == other.y_;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}
