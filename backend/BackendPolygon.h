#pragma once
#ifndef BACKEND_POLYGON_H
#define BACKEND_POLYGON_H

#include "BackendWall.h"
#include "Ray.h"

#include <utility>
#include <vector>

class BackendMirrorRoom {
public:
    BackendMirrorRoom() = default;

    bool CheckCorrectness() const;
    void CalcCorners();
    void AddWall(const BackendWall& wall);
    void DeleteWall(const BackendWall& wall);
    void FireBeam(const Ray& beam, std::vector<Point>& points_on_path, int number_of_points);

private:
    std::vector<Point> corners_;
    std::vector<BackendWall> walls_;
    int size_ {0};
};

#endif // BACKEND_POLYGON_H
