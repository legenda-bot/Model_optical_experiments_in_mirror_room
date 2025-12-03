#include "Wall.h"

Wall::Wall(const Arc& arc) :
	arc_(arc), segment_(Point(0, 0), Point(1, 1)), type_(true),
	start_(arc.GetStartPoint()), end_(arc.GetEndPoint()) {
};

Wall::Wall(const Segment& segment) : arc_(Point(0, 0), Point(1, 1), Point(1, 0)), segment_(segment),
type_(false), start_(segment.GetStart()), end_(segment_.GetEnd()) {
};

Wall::Wall(const Point& first, const Point& second) : type_(false), arc_(Point(0, 0), Point(1, 1), Point(1, 0)),
segment_(first, second), start_(first), end_(second) {
};

Wall::Wall(const Point& first, const Point& second, const Point& third): type_((first - second) * (first - third) != 0) {
	if (!type_) { // if all points are on the same line
		if (Segment(first, second).IsPointIn(third)) {
			*this = Wall(first, second);
			return;
		}
		if (Segment(first, third).IsPointIn(second)) {
			*this = Wall(first, third);
			return;
		}
		*this = Wall(second, third);
		return;
	}
	else {
		arc_ = Arc(first, second, third);
		segment_ = Segment(Point(0, 0), Point(1, 1));
		start_ = arc_.GetStartPoint();
		end_ = arc_.GetEndPoint();
	}
}

bool Wall::IsPointIn(const Point& point) const{
	if (type_) {
		return arc_.IsPointIn(point);
	}
	return segment_.IsPointIn(point);
}

Ray Wall::ReflectLight(const Ray& ray) const {
	if (type_) {
		return ShootBulletInCircle(ray, arc_);
	}
	else {
		return ShootBulletInLine(ray, segment_);
	}
}

Arc Wall::GetArc() const {
	if (type_) {
		return arc_;
	}
	throw std::logic_error("Wrong type of wall");
}

Segment Wall::GetSegment() const {
	if (!type_) {
		return segment_;
	}
	throw std::logic_error("Wrong type of wall");
}

std::string Wall::GetType() const {
	if (type_) {
		return "arc";
	}
	return "segment";
}

bool DoWallsIntersect(const Wall& first, const Wall& second) {
	if (first.GetType() == "arc" || second.GetType() == "arc") {
		return CircleIntersectCircle(first.GetArc(), second.GetArc());
	}
	if (first.GetType() == "arc" || second.GetType() == "segment") {
		return LineIntersectCircle(second.GetSegment(), first.GetArc());
	}
	if (first.GetType() == "segment" || second.GetType() == "arc") {
		return LineIntersectCircle(first.GetSegment(), second.GetArc());
	}
	if (first.GetType() == "segment" || second.GetType() == "segment") {
		return LineIntersectLine(first.GetSegment(), second.GetSegment());
	}
}

std::pair <Point, Point> IntersectWalls(const Wall& first, const Wall& second) {
	if (first.GetType() == "arc" && second.GetType() == "arc") {
		return GetIntersectionCircleAndCircle(first.GetArc(), second.GetArc());
	}
	if (first.GetType() == "arc" && second.GetType() == "segment") {
		return GetIntersectionLineAndCircle(second.GetSegment(), first.GetArc());
	}
	if (first.GetType() == "segment" && second.GetType() == "arc") {
		return GetIntersectionLineAndCircle(first.GetSegment(), second.GetArc());
	}
	if (first.GetType() == "segment" && second.GetType() == "segment") {
		Point intersection = GetIntersectionLineAndLine(first.GetSegment(), second.GetSegment());
		return { intersection, intersection };
	}
}

std::pair<Point, Point> Wall::GetEdges() const {
	return { start_, end_ };
}

bool Wall::operator== (const Wall& other) const{
	if (type_) {
		return (arc_ == other.arc_);
	}
	else {
		return segment_ == other.segment_;
	}
}
