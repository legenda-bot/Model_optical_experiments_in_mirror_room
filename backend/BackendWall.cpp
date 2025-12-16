#include "BackendWall.h"

BackendWall::BackendWall(const Arc& arc)
    : arc_(arc),
      segment_(Point(0, 0), Point(1, 1)),
      type_(true),
      start_(arc.GetStartPoint()),
      end_(arc.GetEndPoint()) {}

BackendWall::BackendWall(const Segment& segment)
    : arc_(Point(0, 0), Point(1, 1), Point(1, 0)),
      segment_(segment),
      type_(false),
      start_(segment.GetStart()),
      end_(segment.GetEnd()) {}

BackendWall::BackendWall(const Point& first, const Point& second)
    : arc_(Point(0, 0), Point(1, 1), Point(1, 0)),
      segment_(first, second),
      type_(false),
      start_(first),
      end_(second) {}

BackendWall::BackendWall(const Point& first, const Point& second, const Point& third)
    : arc_(Point(0, 0), Point(1, 1), Point(1, 0)),
      segment_(Point(0, 0), Point(1, 1)),
      type_((first - second) * (first - third) != 0) {
    if (!type_) { // all points on one line
        if (Segment(first, second).IsPointIn(third)) {
            *this = BackendWall(first, second);
            return;
        }
        if (Segment(first, third).IsPointIn(second)) {
            *this = BackendWall(first, third);
            return;
        }
        *this = BackendWall(second, third);
        return;
    }
        arc_ = Arc(first, second, third);
        start_ = arc_.GetStartPoint();
        end_ = arc_.GetEndPoint();
}

bool BackendWall::IsPointIn(const Point& point) const {
    if (type_) {
        return arc_.IsPointIn(point);
    }
    return segment_.IsPointIn(point);
}

Ray BackendWall::ReflectLight(const Ray& ray) const {
    if (type_) {
        return ShootBulletInCircle(ray, arc_);
    }
    return ShootBulletInLine(ray, segment_);
}

Arc BackendWall::GetArc() const {
    if (!type_) {
        throw std::logic_error("Requested arc from a segment wall");
    }
    return arc_;
}

Segment BackendWall::GetSegment() const {
    if (type_) {
        throw std::logic_error("Requested segment from an arc wall");
    }
    return segment_;
}

std::string BackendWall::GetType() const {
    return type_ ? "arc" : "segment";
}

std::pair<Point, Point> BackendWall::GetEdges() const {
    return {start_, end_};
}

bool BackendWall::operator==(const BackendWall& other) const {
    if (type_ != other.type_) {
        return false;
    }
    return type_ ? (arc_ == other.arc_) : (segment_ == other.segment_);
}

bool DoWallsIntersect(const BackendWall& first, const BackendWall& second) {
    if (first.GetType() == "arc" && second.GetType() == "arc") {
        return CircleIntersectCircle(first.GetArc(), second.GetArc());
    }
    if (first.GetType() == "arc" && second.GetType() == "segment") {
        return LineIntersectCircle(second.GetSegment(), first.GetArc());
    }
    if (first.GetType() == "segment" && second.GetType() == "arc") {
        return LineIntersectCircle(first.GetSegment(), second.GetArc());
    }
    return LineIntersectLine(first.GetSegment(), second.GetSegment());
}

std::pair<Point, Point> IntersectWalls(const BackendWall& first, const BackendWall& second) {
    if (first.GetType() == "arc" && second.GetType() == "arc") {
        return GetIntersectionCircleAndCircle(first.GetArc(), second.GetArc());
    }
    if (first.GetType() == "arc" && second.GetType() == "segment") {
        return GetIntersectionLineAndCircle(second.GetSegment(), first.GetArc());
    }
    if (first.GetType() == "segment" && second.GetType() == "arc") {
        return GetIntersectionLineAndCircle(first.GetSegment(), second.GetArc());
    }
    Point intersection = GetIntersectionLineAndLine(first.GetSegment(), second.GetSegment());
    return {intersection, intersection};
}

Point GetFirstIntersectionOfRayAndWall(const Ray& ray, const BackendWall& wall) { // apex excluded
    if (wall.GetType() == "arc") {
        auto intersections = GetIntersectionLineAndCircle(ray, wall.GetArc());
        return ray.FirstOnRay(intersections.first, intersections.second);
    }
    return GetIntersectionLineAndLine(ray, wall.GetSegment());
}
