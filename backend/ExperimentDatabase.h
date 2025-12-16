#pragma once
#ifndef BACKEND_EXPERIMENTDATABASE_H
#define BACKEND_EXPERIMENTDATABASE_H

#include "BackendPolygon.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

// Simple in-memory storage for experiments (room + beam + trajectory).
class ExperimentDatabase
{
public:
    ExperimentDatabase() = default;

    void AddExperiment(const BackendMirrorRoom& room, const Ray& beam, const std::string& name);
    void ChangeExperiment(const BackendMirrorRoom& room, const Ray& beam, const std::string& name);
    void DeleteExperiment(const std::string& name);
    std::pair<BackendMirrorRoom, std::vector<Point>> GetExperiment(const std::string& name);

private:
    int size_ {0};
    std::map<std::string, int> names_;
    std::vector<BackendMirrorRoom> rooms_;
    std::vector<Ray> beams_;
    std::vector<std::vector<Point>> trajectories_;
};

#endif // BACKEND_EXPERIMENTDATABASE_H
