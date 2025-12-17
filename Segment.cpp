// Segment: line segment geometry helper.

#include "Segment.h"
#include <cmath>

Segment::Segment(const Point& start_point, const Point& end_point)
    : start_point_(start_point),
    direction_vector_(end_point - start_point) {
}

Segment::Segment(const Point& start_point, const Vector& direction_vector)
    : start_point_(start_point),
    direction_vector_(direction_vector) {
}

bool Segment::IsPointIn(const Point& point) const {
    // Сначала проверяем, что точка лежит на прямой отрезка
    Line segment_line = ToLine();
    if (!segment_line.IsPointIn(point)) {
        return false;
    }

    // Проверяем, что точка находится между началом и концом отрезка
    Vector to_point = point - start_point_;

    // Проверяем, что проекция точки на направление отрезка лежит в [0, |d|^2].
    // dot_product = d · (point-start), segment_length_sq = |d|^2.
    long double dot_product = direction_vector_ ^ to_point;
    long double segment_length_sq = direction_vector_.SquaredLength();

    // Точка лежит на отрезке, если проекция между 0 и длиной отрезка
    constexpr long double kEps = 1e-9L;
    const long double slack = kEps * (1.0L + segment_length_sq);
    return dot_product >= -slack && dot_product <= segment_length_sq + slack;
}
bool Segment:: operator == (const Segment& other) const {
    return start_point_ == other.start_point_ && direction_vector_ == other.direction_vector_;
}

Vector Segment::GetDirection() const {
    return direction_vector_;
}

Point Segment::GetStart() const {
    return start_point_;
}
Point Segment::GetEnd() const {
    return (start_point_ + direction_vector_).ToPoint();
}
Line Segment::ToLine() const {
    return Line(start_point_, direction_vector_);
}
