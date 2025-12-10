#pragma once
#ifndef WALL_H
#define WALL_H

#include "Point.h"
#include "Ray.h"
#include "Segment.h"
#include "Intersections.h"
#include "Arc.h"

#include <deque>
#include <string>
#include <vector> 
#include <stdexcept>

class Wall {
public:
	Wall(const Arc& arc);
	Wall(const Segment& segemnt);
	Wall(const Point& first, const Point& second);
	Wall(const Point& first, const Point& second, const Point& third);
	Wall() = default;
	
	bool IsPointIn(const Point& point) const;
	Ray ReflectLight(const Ray& ray) const;
	Arc GetArc() const;
	Segment GetSegment() const;
	std::string GetType() const;
	std::pair<Point, Point> GetEdges() const;
	bool operator == (const Wall& other) const;


private:
	bool type_; // true means arc
	Arc arc_;
	Segment segment_;
	Point start_;
	Point end_;
};

bool DoWallsIntersect(const Wall& first_wall, const Wall& second_wall);

std::pair<Point, Point> IntersectWalls(const Wall& first, const Wall& second);

Point GetFirstIntersectionOfRayAndWall(const Ray& ray, const Wall& wall); // we count what the apex of the beam do not belong to the beam
#endif // WALL_H