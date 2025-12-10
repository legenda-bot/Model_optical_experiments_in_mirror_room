#include "ExperimentDatabase.h"

void ExperimentDatabase::AddExperiment(MirrorRoom room, Ray beam, std::string name) {

	if (names_.find(name) != names_.end()) {
		throw std::invalid_argument("there is already an experiment with this name");
	}
	names_[name] = size_;

	rooms_.push_back(room);
	beams_.push_back(beam);

	std::vector <Point> local;
	if (room.CheckCorrectness()) {
		room.FireBeam(beam, local, 1e5);
	}

	trajectories.push_back(local);
	size_++;
}

void ExperimentDatabase::ChangeExperiment(MirrorRoom room, Ray beam, std::string name) {

	if (names_.find(name) == names_.end()) {
		throw std::invalid_argument("there is no experiment with this name");
	}
	int index = names_[name];
	rooms_[index] = room;
	beams_[index] = beam;

	if (room.CheckCorrectness()) {
		room.FireBeam(beam, trajectories[index], 1e5);
	}
	else {
		trajectories[index].clear();
	}

}

void ExperimentDatabase::DeleteExperiment(std::string name) {
	names_.erase(name);
}

std::pair<MirrorRoom, std::vector <Point> > ExperimentDatabase::GetExperiment(std::string name) {
	int index = names_[name];
	return { rooms_[index], trajectories[index] };
}
