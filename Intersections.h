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

template <typename LineLike1, typename LineLike2>
bool LineIntersectLine(const LineLike1& first, const LineLike2& second) {
	Line first_main = first.ToLine();
	Line second_main = second.ToLine();
	Point intersection = GetIntersection(first_main, second_main);
	return (first.IsPointIn(intersection) && second.IsPointIn(intersection));
}

template <typename LineLike, typename CircleLike>
bool LineIntersectCircle(const LineLike& line, const CircleLike& circle) {
	Line main_line = line.ToLine();
	Circle main_circle = circle.ToCircle();
	
	std::pair<Point, Point> intersections = GetIntersection(main_line, main_circle);
	bool first_point_on = (line.IsPointIn(intersections.first) && circle.IsPointIn(intersections.first));
	bool second_point_on = (line.IsPointIn(intersections.second) && circle.IsPointIn(intersections.second));

	return (first_point_on || second_point_on);
}

template <typename CircleLike1, typename CircleLike2>
bool CircleIntersectCircle(const CircleLike1& first, const CircleLike2& second) {
	Circle main_first = first.ToCircle();
	Circle main_second = second.ToCircle();

	std::pair<Point, Point> intersections = GetIntersection(main_first, main_second);

	bool first_point_on = (first.IsPointIn(intersections.first) && second.IsPointIn(intersections.first));
	bool second_point_on = (first.IsPointIn(intersections.second) && second.IsPointIn(intersections.second));

	return (first_point_on || second_point_on);
}

template <typename LineLike1, typename LineLike2>
Point GetIntersectionLineAndLine(const LineLike1& first, const LineLike2& second) {
	Line first_main = first.ToLine();
	Line second_main = second.ToLine();
	Point intersection = GetIntersection(first_main, second_main);
	return intersection;
}

template  <typename LineLike, typename CircleLike>
std::pair<Point, Point> GetIntersectionLineAndCircle(const LineLike& line, const CircleLike& circle) {
	Line main_line = line.ToLine();
	Circle main_circle = circle.ToCircle();
	std::pair<Point, Point> intersections = GetIntersection(main_line, main_circle);
	bool first_point_on, second_point_on;
	first_point_on = (line.IsPointIn(intersections.first) && circle.IsPointIn(intersections.first));
	second_point_on = (line.IsPointIn(intersections.second) && circle.IsPointIn(intersections.second));
	std::pair<Point, Point> answer_pair = { {0, 0}, {0, 0} };
	if (first_point_on == true) {
		answer_pair.first = intersections.first;
		if (second_point_on == true) {
			answer_pair.second = intersections.second;
		}
	}
	else {
		if (second_point_on == true) {
			answer_pair.first = intersections.second;
		}
	}
	return answer_pair;
}

template <typename CircleLike1, typename CircleLike2>
std::pair<Point, Point> GetIntersectionCircleAndCircle(const CircleLike1& first, const CircleLike2& second) {
	Circle first_main = first.ToCircle();
	Circle second_main = second.ToCircle();

	std::pair<Point, Point> intersections = GetIntersection(first_main, second_main);

	if (first.IsPointIn(intersections.first) && second.IsPointIn(intersections.first)) {
		return intersections;
	}

	return { intersections.second, intersections.first };
}

template <typename CircleLike> 
Ray ShootBulletInCircle(const Ray& trajectory, const CircleLike& wall) {
	if (!LineIntersectCircle(trajectory, wall)) {
		return trajectory;
	}
	std::pair<Point, Point> intersections = GetIntersectionLineAndCircle(trajectory, wall);
	if (wall.IsPointIn(intersections.second)) {
		Point closest = trajectory.FirstOnRay(intersections.first, intersections.second);
		Line tangent = wall.ToCircle().GetTangentAtPoint(closest);
		return Shoot(trajectory, tangent);
	}
	return Shoot(trajectory, wall.ToCircle().GetTangentAtPoint(intersections.first));
}

template <typename LineLike>
Ray ShootBulletInLine(const Ray& trajectory, const LineLike& wall) {
	if (LineIntersectLine(trajectory.ToLine(), wall) == false) {
		return trajectory;
	}
	return Shoot(trajectory, wall.ToLine());
}
#endif // INTERSECTIONS_H
