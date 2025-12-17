// Intersections: declarations for geometry intersection helpers.

#pragma once
#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H
#include "Point.h"
#include "Line.h"
#include "Vector.h"
#include "Circle.h"
#include "Arc.h"
#include "Segment.h"
#include "Ray.h"

bool LineIntersectLine(const Line& first, const Line& second);
bool LineIntersectLine(const Line& first, const Segment& second);
bool LineIntersectLine(const Segment& first, const Line& second);
bool LineIntersectLine(const Segment& first, const Segment& second);
bool LineIntersectLine(const Ray& first, const Line& second);
bool LineIntersectLine(const Ray& first, const Segment& second);

bool LineIntersectCircle(const Line& line, const Circle& circle);
bool LineIntersectCircle(const Line& line, const Arc& circle);
bool LineIntersectCircle(const Segment& line, const Circle& circle);
bool LineIntersectCircle(const Segment& line, const Arc& circle);
bool LineIntersectCircle(const Ray& line, const Circle& circle);
bool LineIntersectCircle(const Ray& line, const Arc& circle);

bool CircleIntersectCircle(const Circle& first, const Circle& second);
bool CircleIntersectCircle(const Circle& first, const Arc& second);
bool CircleIntersectCircle(const Arc& first, const Circle& second);
bool CircleIntersectCircle(const Arc& first, const Arc& second);

Point GetIntersectionLineAndLine(const Line& first, const Line& second);
Point GetIntersectionLineAndLine(const Line& first, const Segment& second);
Point GetIntersectionLineAndLine(const Segment& first, const Line& second);
Point GetIntersectionLineAndLine(const Segment& first, const Segment& second);
Point GetIntersectionLineAndLine(const Ray& first, const Line& second);
Point GetIntersectionLineAndLine(const Ray& first, const Segment& second);

std::pair<Point, Point> GetIntersectionLineAndCircle(const Line& line, const Circle& circle);
std::pair<Point, Point> GetIntersectionLineAndCircle(const Line& line, const Arc& circle);
std::pair<Point, Point> GetIntersectionLineAndCircle(const Segment& line, const Circle& circle);
std::pair<Point, Point> GetIntersectionLineAndCircle(const Segment& line, const Arc& circle);
std::pair<Point, Point> GetIntersectionLineAndCircle(const Ray& line, const Circle& circle);
std::pair<Point, Point> GetIntersectionLineAndCircle(const Ray& line, const Arc& circle);

std::pair<Point, Point> GetIntersectionCircleAndCircle(const Circle& first, const Circle& second);
std::pair<Point, Point> GetIntersectionCircleAndCircle(const Circle& first, const Arc& second);
std::pair<Point, Point> GetIntersectionCircleAndCircle(const Arc& first, const Circle& second);
std::pair<Point, Point> GetIntersectionCircleAndCircle(const Arc& first, const Arc& second);

Ray ShootBulletInCircle(const Ray& trajectory, const Circle& wall);
Ray ShootBulletInCircle(const Ray& trajectory, const Arc& wall);
Ray ShootBulletInLine(const Ray& trajectory, const Line& wall);
Ray ShootBulletInLine(const Ray& trajectory, const Segment& wall);

#endif // INTERSECTIONS_H
