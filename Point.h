// Point: small 2D point helper class used across geometry code.

#pragma once
#ifndef POINT_H
#define POINT_H

class Point {
public:
    Point(long double x, long double y);
    Point() = default;

    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;

    long double x_;
    long double y_;
};

#endif // POINT_H
