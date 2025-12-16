#include "Line.h"

Line::Line(const Point& first_point, const Point& second_point)
    : point_(first_point),
    direction_(second_point - first_point) {
    // Обеспечиваем ненулевой направляющий вектор
    if (direction_.SquaredLength() == 0) {
        direction_ = Vector(1, 0);
    }
}

Line::Line(const Point& point, const Vector& direction)
    : point_(point),
    direction_(direction) {
    // Обеспечиваем ненулевой направляющий вектор
    if (direction_.SquaredLength() == 0) {
        direction_ = Vector(1, 0);
    }
}

Vector Line::GetDirection() const {
    return direction_;
}

bool Line::IsPointIn(const Point& point) const{
    // Вектор от точки на прямой до проверяемой точки
    Vector to_point = point - point_;

    // Точка лежит на прямой, если векторы коллинеарны
    return direction_ * to_point == 0;
}

Vector Line::GetNormal() const {
    return direction_.GetNormal();
}

Point Line::GetPoint() const {
    return point_;
}

Point GetIntersection(const Line& first_line, const Line& second_line) {
    Vector first_vec = first_line.GetDirection();
    Vector second_vec = second_line.GetDirection();
    Point first_start = first_line.GetPoint();
    Point second_start = second_line.GetPoint();

    if (first_vec * second_vec == 0) {
        return first_start;
    }

    std::pair<long double, long double> coefficents = (first_start - second_start).GetCoefficients(second_vec, first_vec * -1);
    return (first_start + coefficents.first * first_vec).ToPoint();
}

long double Line::GetDistToPoint(const Point& point) const {
    Vector normal_vector = GetDirection().GetNormal();
    Line normal_line = Line(point, normal_vector);
    Point intersection = GetIntersection(normal_line, *this);
    return (intersection - point).Length();
}

bool IsIntersection(const Line& first_line, const Line& second_line) {
    return(first_line.GetDirection() * second_line.GetDirection() != 0);
}
