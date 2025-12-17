// Line: 2D line utility helpers.

#include "Line.h"
#include <cmath>

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

    // Точка лежит на прямой, если векторы коллинеарны (псевдоскалярное произведение ≈ 0).
    // Используем относительный допуск, чтобы не ломаться на погрешностях double/long double.
    const long double cross = direction_ * to_point;
    const long double scale = direction_.Length() * to_point.Length();
    constexpr long double kEps = 1e-9L;
    return std::fabsl(cross) <= kEps * (1.0L + scale);
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

    constexpr long double kEps = 1e-12L;
    if (std::fabsl(first_vec * second_vec) <= kEps) {
        // Параллельные прямые (или почти параллельные): возвращаем произвольную точку,
        // вызывающий код обязан проверять факт пересечения отдельно.
        return first_start;
    }

    std::pair<long double, long double> coefficents = (first_start - second_start).GetCoefficients(second_vec, first_vec * -1);
    // Solve: second_start + a*second_vec == first_start + t*first_vec.
    // We compute coefficients for: (first_start - second_start) = a*second_vec + t*(-first_vec).
    // Here coefficents.first == a, coefficents.second == t.
    return (first_start + coefficents.second * first_vec).ToPoint();
}

long double Line::GetDistToPoint(const Point& point) const {
    // Расстояние от точки до прямой: строим перпендикуляр (нормаль) через точку,
    // находим его пересечение с прямой и меряем длину.
    Vector normal_vector = GetDirection().GetNormal();
    Line normal_line = Line(point, normal_vector);
    Point intersection = GetIntersection(normal_line, *this);
    return (intersection - point).Length();
}

bool IsIntersection(const Line& first_line, const Line& second_line) {
    return(first_line.GetDirection() * second_line.GetDirection() != 0);
}
