// Vector: 2D vector type with common operations.

#pragma once
#ifndef VECTOR_H
#define VECTOR_H

#include "Point.h"
#include <utility>

class Vector {
public:
    Vector(long double x, long double y);
    Vector(const Point& point);
    Vector(const Point& fisrt_point, const Point& second_point);
    Vector() = default;

    long double Length() const;
    long double SquaredLength() const;
    std::pair<long double, long double> GetCoefficients(const Vector& first_basis, const Vector& second_basis) const;
    void SetLength(long double new_length);
    Vector GetNormal() const;
    Point ToPoint() const;

    bool operator==(const Vector& other) const;
    Vector operator+(const Vector& other) const;
    Vector operator-(const Vector& other) const;
    Vector operator/(long double scalar) const;
    Vector operator*(long double scalar) const;

    long double operator* (Vector other) const;
    long double operator^ (Vector other) const;

private:
    long double x_;
    long double y_;
};
Vector operator * (const long double& scalar, const Vector& vector);

// Операторы для работы с Point (используют преобразование в Vector)
Vector operator+(const Point& a, const Point& b);
Vector operator+(const Point& point, const Vector& vector);
Vector operator+(const Vector& vector, const Point& point);
Vector operator-(const Point& a, const Point& b);
Vector operator-(const Point& point, const Vector& vector);
Vector operator-(const Vector& vector, const Point& point);
Vector operator*(const Point& point, long double scalar);
Vector operator*(long double scalar, const Point& point);
Vector operator/(const Point& point, long double scalar);

long double operator *(const Point& a, const Point& b);
long double operator *(const Point& a, const Vector& vector);
long double operator *(const Vector& vector, const Point& a);
long double operator ^(const Point& a, const Point& b);
long double operator ^(const Point& a, const Vector& vector);
long double operator ^(const Vector& vector, const Point& a);

#endif // VECTOR_H
