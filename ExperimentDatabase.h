#include "Polygon.h"
#include <map>
#pragma once
class ExperimentDatabase
{
public:
	ExperimentDatabase() : size_(0) {};

	void AddExperiment(MirrorRoom room, Ray beam, std::string name);

	void ChangeExperiment(MirrorRoom room, Ray beam, std::string name);

	void DeleteExperiment(std::string name);

	std::pair<MirrorRoom, std::vector <Point> > GetExperiment(std::string name);

private:
	
	int size_;
	std::map<std::string, int> names_;
	std::vector <MirrorRoom> rooms_;
	std::vector <Ray> beams_;
	std::vector <std::vector<Point>> trajectories;
};

