#include "Polygon.h"

	bool MirrorRoom::CheckCorrectness() const {
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

	void MirrorRoom::CalcCorners() {
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

	void MirrorRoom::AddWall(Wall wall) {
		size_++;
		walls_.push_back(wall);
	}

	void MirrorRoom::DeleteWall(Wall wall) {
		for (int i = 0; i < size_; i++) {
			if (wall == walls_[i]) {
				std::swap(walls_[i], walls_[size_ - 1]);
				walls_.pop_back();
				size_--;
				return;
			}
		}
	}

	void MirrorRoom::FireBeam(const Ray& beam, std::vector<Point>& points_on_path, int number_of_points) { // vector will be cleared before fulling,
		//so there may be some trash there
		points_on_path.clear();
		points_on_path.push_back(beam.GetVertex());

		if (!CheckCorrectness()) {
			throw std::logic_error("room is not designed correctly");
		}

		CalcCorners();

		Ray last_reflection = beam;
		for (int i = 1; i < number_of_points; i++) {

			Point general_closest;
			int index_of_the_wall = -1;
			for (int j = 0; j < size_; j++) {

				Point potential_point = GetFirstIntersectionOfRayAndWall(last_reflection, walls_[j]);
				if (last_reflection.IsPointIn(potential_point) == false) {
					continue;
				}
				if (index_of_the_wall == -1) {
					general_closest = potential_point;
					index_of_the_wall = j;
				}
				else if (last_reflection.FirstOnRay(general_closest, potential_point) == potential_point) {
					general_closest = potential_point;
					index_of_the_wall = j;
				}
			}

			if (index_of_the_wall == -1) {
				for (; i < number_of_points; i++) {
					Vector direction = last_reflection.GetDirection();
					direction.SetLength(100); // just random number. This piece of code process the case when the beam flew out 
					// of the polygon
					points_on_path.push_back((points_on_path[i - 1] + direction).ToPoint());

				}
				break;
			}

			points_on_path.push_back(general_closest);

			bool flag = false;
			for (int k = 0; k < corners_.size(); k++) {
				if (general_closest == corners_[k]) {
					flag = true;
					break;
				}
			}
			if (flag) {
				last_reflection = Ray(general_closest, -1 * last_reflection.GetDirection());
			}
			else {
				last_reflection = walls_[index_of_the_wall].ReflectLight(last_reflection);
			}
		}
	}

