#pragma once
#ifndef POLYGON_H
#define POLYGON_H

#include "Point.h"
#include "Vector"
#include "Line.h"
#include "Ray.h"
#include "Segment.h"
#include "Circle.h"
#include "Arc.h"
#include "Intersections.h"
#include "Wall.h"

class MirrorRoom {
public:
	MirrorRoom() = default;

	bool CheckCorrectness() const;

	void CalcCorners();

	void AddWall(Wall wall);

	void DeleteWall(Wall wall);

	void FireBeam(const Ray& beam, std::vector<Point>& points_on_path, int number_of_points);

private:
	std::vector<Point> corners_;
	std::vector<Point> edges_;
	std::vector<Wall> walls_;
	int size_;
};

#endif // POLYGON_H
