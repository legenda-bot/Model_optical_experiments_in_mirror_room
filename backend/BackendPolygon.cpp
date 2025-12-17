// BackendPolygon: non-UI polygon representation for persistence/computations.

#include "BackendPolygon.h"

bool BackendMirrorRoom::CheckCorrectness() const {
    if (size_ == 0) {
        return true;
    }
    // Проверка корректности полигона из стен:
    // хотим, чтобы стены образовывали один замкнутый цикл без "разветвлений".
    Point last_point = walls_[0].GetEdges().second;
    bool found_next = false;
    int current_next_wall = -1;
    Point next_point;
    std::vector<BackendWall> local = walls_;

    for (int i = 1; i < size_; i++) {
        found_next = false;
        for (int j = i; j < size_; j++) {
            std::pair<Point, Point> edges = local[j].GetEdges();
            if (edges.first == last_point || edges.second == last_point) {
                if (found_next) {
                    return false;
                }
                found_next = true;
                current_next_wall = j;
                next_point = (edges.first == last_point) ? edges.second : edges.first;
            }
        }
        if (!found_next) {
            return false;
        }
        std::swap(local[i], local[current_next_wall]);
        last_point = next_point;
    }
    return (local[0].GetEdges().first == local[size_ - 1].GetEdges().first) ||
           (local[0].GetEdges().second == local[size_ - 1].GetEdges().second);
}

void BackendMirrorRoom::CalcCorners() {
    corners_.clear();
    // Углы (вершины) — это точки пересечения пар стен, которые реально лежат на обеих стенах.
    for (int i = 0; i < size_; i++) {
        for (int j = i + 1; j < size_; j++) {
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
}

void BackendMirrorRoom::AddWall(const BackendWall& wall) {
    size_++;
    walls_.push_back(wall);
}

void BackendMirrorRoom::DeleteWall(const BackendWall& wall) {
    for (int i = 0; i < size_; i++) {
        if (wall == walls_[i]) {
            std::swap(walls_[i], walls_[size_ - 1]);
            walls_.pop_back();
            size_--;
            return;
        }
    }
}

void BackendMirrorRoom::FireBeam(const Ray& beam, std::vector<Point>& points_on_path, int number_of_points) { // vector will be cleared before filling
    points_on_path.clear();
    points_on_path.push_back(beam.GetVertex());

    if (CheckCorrectness()) {
        CalcCorners();
    } else {
        corners_.clear();
    }

    // Основной цикл трассировки:
    // на каждом шаге ищем ближайшее пересечение луча со всеми стенами (по направлению луча),
    // добавляем точку удара, затем отражаем луч от найденной стены.
    Ray last_reflection = beam;
    for (int i = 1; i < number_of_points; i++) {

        Point general_closest;
        int index_of_the_wall = -1;
        for (int j = 0; j < size_; j++) {

            Point potential_point = GetFirstIntersectionOfRayAndWall(last_reflection, walls_[j]);
            if (!last_reflection.IsPointIn(potential_point)) {
                continue;
            }
            if (index_of_the_wall == -1) {
                general_closest = potential_point;
                index_of_the_wall = j;
            } else if (last_reflection.FirstOnRay(general_closest, potential_point) == potential_point) {
                general_closest = potential_point;
                index_of_the_wall = j;
            }
        }

        if (index_of_the_wall == -1) {
            // Если пересечений нет — считаем, что луч покинул комнату.
            Vector direction = last_reflection.GetDirection();
            direction.SetLength(10000); // arbitrary length when beam exits the room (for UI rendering)
            points_on_path.push_back((points_on_path.back() + direction).ToPoint());
            return;
        }

        points_on_path.push_back(general_closest);

        bool on_corner = false;
        // Если попали ровно в вершину (пересечение стен), разворачиваем луч назад, чтобы не "застрять" в угле.
        for (const auto& corner : corners_) {
            if (general_closest == corner) {
                on_corner = true;
                break;
            }
        }
        if (on_corner) {
            last_reflection = Ray(general_closest, -1 * last_reflection.GetDirection());
        } else {
            last_reflection = walls_[index_of_the_wall].ReflectLight(last_reflection);
        }

        // Nudge the ray origin forward a little to avoid re-hitting the same wall due to
        // floating-point inaccuracies (especially on vertices and near-parallel hits).
        Vector step = last_reflection.GetDirection();
        if (step.SquaredLength() > 0) {
            step.SetLength(1e-3L);
            last_reflection = Ray((general_closest + step).ToPoint(), last_reflection.GetDirection());
        }
    }
}
