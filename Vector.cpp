#include "vector.h"
#include <cmath>

Vector::Vector(long double x, long double y) : x_(x), y_(y) {}

Vector::Vector(const Point& point) : x_(point.x_), y_(point.y_) {}

Vector::Vector(const Point& first_point, const Point& second_point) : x_(second_point.x_ - first_point.x_),
y_(second_point.y_ - first_point.y_) {};

long double Vector::Length() const {
    return std::sqrt(x_ * x_ + y_ * y_);
}

long double Vector::SquaredLength() const {
    return x_ * x_ + y_ * y_;
}

std::pair<long double, long double> Vector::GetCoefficients(const Vector& first_basis, const Vector& second_basis) const {
    long double determinant = first_basis * second_basis;
    if (determinant == 0) {
        return { 0, 0 }; // Базисные векторы коллинеарны
    }

    long double alpha = *this * second_basis / determinant;
    long double beta = first_basis * *this / determinant;
    return { alpha, beta };
}

void Vector::SetLength(long double new_length) {
    long double current_length = Length();
    if (current_length == 0) return;

    double scale = new_length / current_length;
    x_ *= scale;
    y_ *= scale;
}

Vector Vector::GetNormal() const {
    return Vector(-y_, x_);
}

Point Vector::ToPoint() const {
    return Point(x_, y_);
}

bool Vector::operator==(const Vector& other) const {
    return x_ == other.x_ && y_ == other.y_;
}

Vector Vector::operator+(const Vector& other) const {
    return Vector(x_ + other.x_, y_ + other.y_);
}

Vector Vector::operator-(const Vector& other) const {
    return Vector(x_ - other.x_, y_ - other.y_);
}

Vector Vector::operator/(long double scalar) const {
    return Vector(x_ / scalar, y_ / scalar);
}

Vector Vector::operator*(long double scalar) const {
    return Vector(x_ * scalar, y_ * scalar);
}

Vector operator* (const long double& scalar, const Vector& vector) {
    return vector * scalar;
}

long double Vector:: operator*(Vector other) const {
    return x_ * other.y_ - other.x_ * y_;
}

long double Vector:: operator^(const Vector other) const {
    return x_ * other.x_ + y_ * other.y_;
}

// Реализации операторов для работы с Point
Vector operator+(const Point& a, const Point& b) {
    return Vector(a) + Vector(b);
}

Vector operator+(const Point& point, const Vector& vector) {
    return Vector(point) + vector;
}

Vector operator+(const Vector& vector, const Point& point) {
    return vector + Vector(point);
}

Vector operator-(const Point& a, const Point& b) {
    return Vector(a) - Vector(b);
}

Vector operator-(const Point& point, const Vector& vector) {
    return Vector(point) - vector;
}

Vector operator-(const Vector& vector, const Point& point) {
    return vector - Vector(point);
}

Vector operator*(const Point& point, long double scalar) {
    return Vector(point) * scalar;
}

Vector operator*(long double scalar, const Point& point) {
    return Vector(point) * scalar;
}

Vector operator/(const Point& point, long double scalar) {
    return Vector(point) / scalar;
}

long double operator *(const Point& a, const Point& b) {
    return Vector(a) * Vector(b);
}

long double operator *(const Point& a, const Vector& vector) {
    return Vector(a) * vector;
}

long double operator *(const Vector& vector, const Point& a) {
    return a * vector;
}

long double operator ^(const Point& a, const Point& b) {
    return Vector(a) * Vector(b);
}

long double operator ^ (const Point& a, const Vector& vector) {
    return Vector(a) * vector;
}

long double operator ^ (const Vector& vector, const Point& a) {
    return a * vector;
}