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
	bool CheckCorrectness() {
		if (size_ == 0) {
			return true;
		}
		Point last_point = walls_[0].GetEdges().second;
		bool flag = false;
		int current_next_wall;
		Point next_point;
		for (int i = 1; i < size_; i++) {
			for (int j = i; j < size_; j++) {
				std::pair<Point, Point> edges = walls_[j].GetEdges();
				if (edges.first == last_point || edges.second == last_point) {
					if (flag == true) {
						return false;
					}
					flag = true;
					current_next_wall = j;
					next_point = ((edges.first == last_point) ? edges.second : edges.first);
				}
			}
			if (flag == false) {
				return false;
			}
			std::swap(walls_[i], walls_[current_next_wall]);
			last_point = next_point;
		}
		if (walls_[0].GetEdges().first == walls_[size_ - 1].GetEdges().first ||
			walls_[0].GetEdges().second == walls_[size_ - 1].GetEdges().second) {
			return true;
		}
		else {
			return false;
		}
	}

	void CalcCorners() {
		for (int i = 0; i < size_; i++) {
			for (int j = i + 1; j < size_; j++) {

			}
			if (DoWallsIntersect(walls_[i], walls_[j])) {
				std::pair<Point, Point> intersections = IntersectWalls(walls_[i], walls_[j]);

				if (walls_[i].IsPointIn(intersections.first) && walls_[j].IsPointIn(intersections.first)) {
					corners_.push_back(intersections.first);
				}
				if (walls_[i].IsPointIn(intersections.second) && walls_[j].IsPointIn(intersections.second)) {
					corners_.push_back(intersections.second);
				}
			}
		}
	}

	void AddWall( Wall wall) {
		size_++;
		walls_.push_back(wall);
	}

	void DeleteWall(Wall wall) {
		for (int i = 0; i < size_; i++) {
			if (wall == walls_[i]) {
				std::swap(walls_[i], walls_[size_ - 1]);
				walls_.pop_back();
				return;
			}
		}
	}

private:
	std::vector<Point> corners_;
	std::vector<Point> edges_;
	std::vector<Wall> walls_;
	int size_;
};

#endif // POLYGON_H
