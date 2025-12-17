// BackendWall: backend/persistence model of a wall.

#pragma once
#ifndef BACKEND_WALL_H
#define BACKEND_WALL_H

#include "Arc.h"
#include "Intersections.h"
#include "Point.h"
#include "Ray.h"
#include "Segment.h"

#include <stdexcept>
#include <string>
#include <utility>

// Backend geometric wall that can be a segment or an arc.
class BackendWall {
public:
    BackendWall(const Arc& arc);
    BackendWall(const Segment& segment);
    BackendWall(const Point& first, const Point& second);
    BackendWall(const Point& first, const Point& second, const Point& third);
    BackendWall() = default;

    bool IsPointIn(const Point& point) const;
    Ray ReflectLight(const Ray& ray) const;
    Arc GetArc() const;
    Segment GetSegment() const;
    std::string GetType() const;
    std::pair<Point, Point> GetEdges() const;
    bool operator==(const BackendWall& other) const;

private:
    Arc arc_ {Point(0, 0), Point(1, 1), Point(1, 0)};
    Segment segment_ {Point(0, 0), Point(1, 1)};
    bool type_ {false}; // true means arc, false means segment
    Point start_ {0, 0};
    Point end_ {1, 1};
};

bool DoWallsIntersect(const BackendWall& first_wall, const BackendWall& second_wall);
std::pair<Point, Point> IntersectWalls(const BackendWall& first, const BackendWall& second);
Point GetFirstIntersectionOfRayAndWall(const Ray& ray, const BackendWall& wall); // apex is not considered

#endif // BACKEND_WALL_H
