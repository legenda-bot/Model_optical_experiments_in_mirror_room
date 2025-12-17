// Point: lightweight 2D point wrapper (project-specific).

#include "Point.h"
#include <cmath>

Point::Point(long double x, long double y) : x_(x), y_(y) {}

bool Point::operator==(const Point& other) const {
    constexpr long double kEps = 1e-6L;
    // Сравнение с допуском: в геометрии почти всегда работаем с погрешностями,
    // поэтому точное равенство double/long double приводит к "пропускам" пересечений.
    return std::fabsl(x_ - other.x_) <= kEps && std::fabsl(y_ - other.y_) <= kEps;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}
