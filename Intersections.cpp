#include "Intersections.h"

namespace {
template <typename LineLike1, typename LineLike2>
bool LineLine(const LineLike1& first, const LineLike2& second) {
    Line first_main = first.ToLine();
    Line second_main = second.ToLine();
    Point intersection = GetIntersection(first_main, second_main);
    return first.IsPointIn(intersection) && second.IsPointIn(intersection);
}

template <typename LineLike, typename CircleLike>
bool LineCircle(const LineLike& line, const CircleLike& circle) {
    Line main_line = line.ToLine();
    Circle main_circle = circle.ToCircle();

    std::pair<Point, Point> intersections = GetIntersection(main_line, main_circle);
    bool first_point_on = line.IsPointIn(intersections.first) && circle.IsPointIn(intersections.first);
    bool second_point_on = line.IsPointIn(intersections.second) && circle.IsPointIn(intersections.second);
    return first_point_on || second_point_on;
}

template <typename CircleLike1, typename CircleLike2>
bool CircleCircle(const CircleLike1& first, const CircleLike2& second) {
    Circle main_first = first.ToCircle();
    Circle main_second = second.ToCircle();

    std::pair<Point, Point> intersections = GetIntersection(main_first, main_second);

    bool first_point_on = first.IsPointIn(intersections.first) && second.IsPointIn(intersections.first);
    bool second_point_on = first.IsPointIn(intersections.second) && second.IsPointIn(intersections.second);
    return first_point_on || second_point_on;
}

template <typename LineLike1, typename LineLike2>
Point IntersectionLineLine(const LineLike1& first, const LineLike2& second) {
    Line first_main = first.ToLine();
    Line second_main = second.ToLine();
    return GetIntersection(first_main, second_main);
}

template <typename LineLike, typename CircleLike>
std::pair<Point, Point> IntersectionLineCircle(const LineLike& line, const CircleLike& circle) {
    Line main_line = line.ToLine();
    Circle main_circle = circle.ToCircle();
    std::pair<Point, Point> intersections = GetIntersection(main_line, main_circle);
    bool first_on = line.IsPointIn(intersections.first) && circle.IsPointIn(intersections.first);
    bool second_on = line.IsPointIn(intersections.second) && circle.IsPointIn(intersections.second);

    std::pair<Point, Point> result{{0, 0}, {0, 0}};
    if (first_on) {
        result.first = intersections.first;
        if (second_on) {
            result.second = intersections.second;
        }
    } else if (second_on) {
        result.first = intersections.second;
    }
    return result;
}

template <typename CircleLike1, typename CircleLike2>
std::pair<Point, Point> IntersectionCircleCircle(const CircleLike1& first, const CircleLike2& second) {
    Circle first_main = first.ToCircle();
    Circle second_main = second.ToCircle();

    std::pair<Point, Point> intersections = GetIntersection(first_main, second_main);

    if (first.IsPointIn(intersections.first) && second.IsPointIn(intersections.first)) {
        return intersections;
    }
    return {intersections.second, intersections.first};
}

template <typename CircleLike>
Ray ShootCircle(const Ray& trajectory, const CircleLike& wall) {
    if (!LineCircle(trajectory, wall)) {
        return trajectory;
    }
    std::pair<Point, Point> intersections = IntersectionLineCircle(trajectory, wall);
    if (wall.IsPointIn(intersections.second)) {
        Point closest = trajectory.FirstOnRay(intersections.first, intersections.second);
        Line tangent = wall.ToCircle().GetTangentAtPoint(closest);
        return Shoot(trajectory, tangent);
    }
    return Shoot(trajectory, wall.ToCircle().GetTangentAtPoint(intersections.first));
}

template <typename LineLike>
Ray ShootLine(const Ray& trajectory, const LineLike& wall) {
    if (!LineLine(trajectory.ToLine(), wall)) {
        return trajectory;
    }
    return Shoot(trajectory, wall.ToLine());
}
} // namespace

bool LineIntersectLine(const Line& first, const Line& second) { return LineLine(first, second); }
bool LineIntersectLine(const Line& first, const Segment& second) { return LineLine(first, second); }
bool LineIntersectLine(const Segment& first, const Line& second) { return LineLine(first, second); }
bool LineIntersectLine(const Segment& first, const Segment& second) { return LineLine(first, second); }
bool LineIntersectLine(const Ray& first, const Line& second) { return LineLine(first.ToLine(), second); }
bool LineIntersectLine(const Ray& first, const Segment& second) { return LineLine(first.ToLine(), second); }

bool LineIntersectCircle(const Line& line, const Circle& circle) { return LineCircle(line, circle); }
bool LineIntersectCircle(const Line& line, const Arc& circle) { return LineCircle(line, circle); }
bool LineIntersectCircle(const Segment& line, const Circle& circle) { return LineCircle(line, circle); }
bool LineIntersectCircle(const Segment& line, const Arc& circle) { return LineCircle(line, circle); }
bool LineIntersectCircle(const Ray& line, const Circle& circle) { return LineCircle(line, circle); }
bool LineIntersectCircle(const Ray& line, const Arc& circle) { return LineCircle(line, circle); }

bool CircleIntersectCircle(const Circle& first, const Circle& second) { return CircleCircle(first, second); }
bool CircleIntersectCircle(const Circle& first, const Arc& second) { return CircleCircle(first, second); }
bool CircleIntersectCircle(const Arc& first, const Circle& second) { return CircleCircle(first, second); }
bool CircleIntersectCircle(const Arc& first, const Arc& second) { return CircleCircle(first, second); }

Point GetIntersectionLineAndLine(const Line& first, const Line& second) { return IntersectionLineLine(first, second); }
Point GetIntersectionLineAndLine(const Line& first, const Segment& second) { return IntersectionLineLine(first, second); }
Point GetIntersectionLineAndLine(const Segment& first, const Line& second) { return IntersectionLineLine(first, second); }
Point GetIntersectionLineAndLine(const Segment& first, const Segment& second) { return IntersectionLineLine(first, second); }
Point GetIntersectionLineAndLine(const Ray& first, const Line& second) { return IntersectionLineLine(first.ToLine(), second); }
Point GetIntersectionLineAndLine(const Ray& first, const Segment& second) { return IntersectionLineLine(first.ToLine(), second); }

std::pair<Point, Point> GetIntersectionLineAndCircle(const Line& line, const Circle& circle) { return IntersectionLineCircle(line, circle); }
std::pair<Point, Point> GetIntersectionLineAndCircle(const Line& line, const Arc& circle) { return IntersectionLineCircle(line, circle); }
std::pair<Point, Point> GetIntersectionLineAndCircle(const Segment& line, const Circle& circle) { return IntersectionLineCircle(line, circle); }
std::pair<Point, Point> GetIntersectionLineAndCircle(const Segment& line, const Arc& circle) { return IntersectionLineCircle(line, circle); }
std::pair<Point, Point> GetIntersectionLineAndCircle(const Ray& line, const Circle& circle) { return IntersectionLineCircle(line, circle); }
std::pair<Point, Point> GetIntersectionLineAndCircle(const Ray& line, const Arc& circle) { return IntersectionLineCircle(line, circle); }

std::pair<Point, Point> GetIntersectionCircleAndCircle(const Circle& first, const Circle& second) { return IntersectionCircleCircle(first, second); }
std::pair<Point, Point> GetIntersectionCircleAndCircle(const Circle& first, const Arc& second) { return IntersectionCircleCircle(first, second); }
std::pair<Point, Point> GetIntersectionCircleAndCircle(const Arc& first, const Circle& second) { return IntersectionCircleCircle(first, second); }
std::pair<Point, Point> GetIntersectionCircleAndCircle(const Arc& first, const Arc& second) { return IntersectionCircleCircle(first, second); }

Ray ShootBulletInCircle(const Ray& trajectory, const Circle& wall) { return ShootCircle(trajectory, wall); }
Ray ShootBulletInCircle(const Ray& trajectory, const Arc& wall) { return ShootCircle(trajectory, wall); }
Ray ShootBulletInLine(const Ray& trajectory, const Line& wall) { return ShootLine(trajectory, wall); }
Ray ShootBulletInLine(const Ray& trajectory, const Segment& wall) { return ShootLine(trajectory, wall); }
