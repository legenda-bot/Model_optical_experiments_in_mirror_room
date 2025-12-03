#include "segment.h"
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

    // Проверяем коллинеарность и что проекция находится в пределах отрезка
    long double dot_product = direction_vector_ ^ to_point;
    long double segment_length_sq = direction_vector_.SquaredLength();

    // Точка лежит на отрезке, если проекция между 0 и длиной отрезка
    return dot_product >= 0 && dot_product <= segment_length_sq;
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