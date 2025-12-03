#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H

#include "point.h"
#include "line.h"

class Circle {
public:
    Circle(const Point& center, long double radius);
    Circle(const Point& a, const Point& b, const Point& c);
    Circle() = default;

    bool IsPointIn(const Point& point) const;
    Point GetCenter() const;
    long double GetRadius() const;
    Circle ToCircle() const { return *this; }
    Line GetTangentAtPoint(const Point& point) const;

private:
    Point center_;
    long double radius_;
};

std::pair<Point, Point> GetIntersection(const Circle& circle, const Line& line);
std::pair<Point, Point> GetIntersection(const Line& line, const Circle& circle);
std::pair<Point, Point> GetIntersection(const Circle& circle1, const Circle& circle2);
bool IsIntersection(const Circle& circle, const Line& line);
bool IsIntersection(const Line& line, const Circle& circle);
bool IsIntersection(const Circle& first_circle, const Circle& second_circle);
#endif // CIRCLE_H
