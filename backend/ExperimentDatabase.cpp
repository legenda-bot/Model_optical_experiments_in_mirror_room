// ExperimentDatabase: save/load experiments (room + ray parameters).

#include "ExperimentDatabase.h"

#include <stdexcept>

void ExperimentDatabase::AddExperiment(const BackendMirrorRoom& room, const Ray& beam, const std::string& name) {
    if (names_.find(name) != names_.end()) {
        throw std::invalid_argument("there is already an experiment with this name");
    }
    names_[name] = size_;

    rooms_.push_back(room);
    beams_.push_back(beam);

    std::vector<Point> local;
    BackendMirrorRoom roomCopy = room;
    if (roomCopy.CheckCorrectness()) {
        roomCopy.FireBeam(beam, local, 1e5);
    }

    trajectories_.push_back(local);
    size_++;
}

void ExperimentDatabase::ChangeExperiment(const BackendMirrorRoom& room, const Ray& beam, const std::string& name) {
    if (names_.find(name) == names_.end()) {
        throw std::invalid_argument("there is no experiment with this name");
    }
    int index = names_[name];
    rooms_[index] = room;
    beams_[index] = beam;

    BackendMirrorRoom roomCopy = room;
    if (roomCopy.CheckCorrectness()) {
        roomCopy.FireBeam(beam, trajectories_[index], 1e5);
    } else {
        trajectories_[index].clear();
    }
}

void ExperimentDatabase::DeleteExperiment(const std::string& name) {
    names_.erase(name);
}

std::pair<BackendMirrorRoom, std::vector<Point>> ExperimentDatabase::GetExperiment(const std::string& name) {
    int index = names_[name];
    return { rooms_[index], trajectories_[index] };
}
