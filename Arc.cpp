// Arc: circular arc geometry helpers used by the mirror room model.

#include "Arc.h"
#include <cmath>

Arc::Arc(const Point& start_point, const Point& middle_point, const Point& end_point)
    : start_point_(start_point),
    middle_point_(middle_point),
    end_point_(end_point),
    circle_(start_point, middle_point, end_point) {
}

bool Arc::IsPointIn(const Point& point) const {
    // Сначала проверяем, что точка лежит на окружности
    if (!circle_.IsPointIn(point)) {
        return false;
    }

    Point center = circle_.GetCenter();

    // Векторы от центра к точкам дуги
    Vector start_vector = start_point_ - center;
    Vector middle_vector = middle_point_ - center;
    Vector end_vector = end_point_ - center;
    Vector point_vector = point - center;

    // Проверяем, что точка лежит на "той же" части окружности, что и middle, между start и end.
    // Идея: сравниваем ориентацию (знак псевдоскалярного произведения) относительно стартового
    // и конечного направлений. Если point и middle по одну сторону — point принадлежит дуге.

    // Векторное произведение start->middle и start->point
    long double cross_start_middle = start_vector * middle_vector;
    long double cross_start_point = start_vector * point_vector;

    // Векторное произведение middle->end и point->end
    long double cross_middle_end = middle_vector * end_vector;
    long double cross_point_end = point_vector * end_vector;

    // Точка лежит на дуге, если она находится в том же секторе, что и средняя точка
    constexpr long double kEps = 1e-9L;
    bool same_side_as_middle_from_start = (cross_start_point * cross_start_middle >= -kEps);
    bool same_side_as_middle_from_end = (cross_point_end * cross_middle_end >= -kEps);

    return same_side_as_middle_from_start && same_side_as_middle_from_end;
}

bool Arc:: operator == (const Arc other)const {
    return (start_point_ == other.start_point_ && end_point_ == other.end_point_ && middle_point_ == other.middle_point_);
}

Circle Arc::ToCircle() const {
    return circle_;
}

Point Arc::GetStartPoint() const {
    return start_point_;
}

Point Arc::GetEndPoint() const {
    return end_point_;
}

Point Arc::GetMiddlePoint() const {
    return middle_point_;
}
