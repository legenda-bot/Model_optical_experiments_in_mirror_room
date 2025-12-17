// Circle: circle geometry primitives (center/radius and related helpers).

#include "Circle.h"
#include <cmath>

Circle::Circle(const Point& center, long double radius)
    : center_(center), radius_(radius) {
}

Circle::Circle(const Point& a, const Point& b, const Point& c)       {
    // Вычисляем центр окружности по трем точкам
    // Используем формулы для пересечения серединных перпендикуляров

    long double x1 = a.x_, y1 = a.y_;
    long double x2 = b.x_, y2 = b.y_;
    long double x3 = c.x_, y3 = c.y_;

    long double A = x2 - x1;
    long double B = y2 - y1;
    long double C = x3 - x1;
    long double D = y3 - y1;

    long double E = A * (x1 + x2) + B * (y1 + y2);
    long double F = C * (x1 + x3) + D * (y1 + y3);

    long double G = 2 * (A * (y3 - y2) - B * (x3 - x2));

    if (G == 0) {
        // Точки коллинеарны - устанавливаем произвольные значения
        center_ = a;
        radius_ = 1;
    }
    else {
        long double center_x = (D * E - B * F) / G;
        long double center_y = (A * F - C * E) / G;
        center_ = Point(center_x, center_y);
        radius_ = Vector(center_, a).Length();
    }
}

Point Circle::GetCenter() const {
    return center_;
}

bool Circle::IsPointIn(const Point& point) const {
    // Точка лежит на окружности, если расстояние до центра равно радиусу
    Vector to_center(point - center_);
    return to_center.SquaredLength() == radius_ * radius_;
}

Line Circle::GetTangentAtPoint(const Point& point) const {
    // Касательная перпендикулярна радиусу в точке касания
    Vector radius_vector(center_, point);
    Vector tangent_direction = radius_vector.GetNormal();
    return Line(point, tangent_direction);
}

long double Circle::GetRadius() const {
    return radius_;
}

std::pair<Point, Point> GetIntersection(const Circle& circle, const Line& line) {
    Line normal_line = Line(circle.GetCenter(), line.GetNormal());
    Point intersection = GetIntersection(normal_line, line);
    Vector center_to_line = intersection - circle.GetCenter();

    if (center_to_line.SquaredLength() > circle.GetRadius() * circle.GetRadius()) {
        return { {0, 0}, {0, 0} }; // return a random value
    }

    Vector line_vec = line.GetDirection();
    line_vec.SetLength(sqrt(circle.GetRadius() * circle.GetRadius() - center_to_line.SquaredLength()));

    return { (center_to_line + line_vec).ToPoint(), (center_to_line - line_vec).ToPoint()};
}

std::pair<Point, Point> GetIntersection(const Line& line, const Circle& circle) {
    return GetIntersection(circle, line);
}

std::pair<Point, Point> GetIntersection(const Circle& circle1, const Circle& circle2) {

    Point center1 = circle1.GetCenter();
    Point center2 = circle2.GetCenter();
    long double radius1 = circle1.GetRadius();
    long double radius2 = circle2.GetRadius();

    // Вектор между центрами окружностей
    Vector centers_vector = center2 - center1;
    long double distance = centers_vector.Length();

    // Проверка случаев отсутствия пересечения
    if (distance > radius1 + radius2 || distance < std::abs(radius1 - radius2)) {
        // Окружности не пересекаются
        return { Point(0, 0), Point(0, 0) };
    }

    // Случай совпадающих окружностей
    if (distance == 0 && radius1 == radius2) {
        // Бесконечное количество точек пересечения - возвращаем две произвольные
        return { Point(0, 0), Point(0, 0) };
    }

    // Вычисляем расстояние от центра первой окружности до линии пересечения
    long double a = (radius1 * radius1 - radius2 * radius2 + distance * distance) / (2 * distance);

    // Вычисляем высоту (расстояние от линии центров до точек пересечения)
    // Гарантируем, что выражение под корнем неотрицательно
    long double h_squared = radius1 * radius1 - a * a;
    if (h_squared < 0) {
        // Это может произойти из-за ошибок округления, берем 0
        h_squared = 0;
    }
    long double h = std::sqrt(h_squared);

    // Точка на линии центров, от которой откладываем перпендикуляр
    Vector normalized_centers = centers_vector / distance;
    Point midpoint = (center1 + normalized_centers * a).ToPoint();

    // Перпендикулярный вектор к линии центров
    Vector perpendicular = normalized_centers.GetNormal() * h;

    // Две точки пересечения
    Point intersection1 = (midpoint + perpendicular).ToPoint();
    Point intersection2 = (midpoint - perpendicular).ToPoint();

    return { intersection1, intersection2 };

}

bool IsIntersection(const Circle& circle, const Line& line) {
    return(circle.GetRadius() <= line.GetDistToPoint(circle.GetCenter()));
}

bool IsIntersection(const Line& line, const Circle& circle) {
    return(circle.GetRadius() <= line.GetDistToPoint(circle.GetCenter()));
}

bool IsIntersection(const Circle& first_circle, const Circle& second_circle) {
    return (first_circle.GetRadius() + second_circle.GetRadius() >= 
        Vector(first_circle.GetCenter() - second_circle.GetCenter()).Length());
}
