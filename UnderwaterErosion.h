#ifndef UNDERWATEREROSION_H
#define UNDERWATEREROSION_H

#include "VoxelGrid.h"

#include <tuple>

class UnderwaterErosion
{
public:
    UnderwaterErosion();
    UnderwaterErosion(std::shared_ptr<VoxelGrid> grid, int maxRockSize, float maxRockStrength, int rockAmount);

//    std::vector<std::vector<Vector3>> Apply(int avoidMatter = -1);
    std::tuple<std::vector<std::vector<Vector3>>, std::vector<std::vector<Vector3>>> Apply(std::shared_ptr<Vector3> startingPoint = nullptr, std::shared_ptr<Vector3> originalDirection = nullptr, int avoidMatter = 10, float flowfieldFactor = 0.1, float randomnessFactor = 0.05, bool returnEvenLostRocks = false);
    std::tuple<std::vector<std::vector<Vector3>>, std::vector<std::vector<Vector3>>> Apply(Vector3 startingPoint, Vector3 originalDirection, int avoidMatter = 10, float flowfieldFactor = 0.1, float randomnessFactor = 0.05, bool returnEvenLostRocks = false);
    std::vector<Vector3> CreateTunnel(std::shared_ptr<Vector3> startingPoint = nullptr, std::shared_ptr<Vector3> endingPoint = nullptr, int numberPoints = 2, bool addingMatter = false);


    std::shared_ptr<VoxelGrid> grid;
    int maxRockSize, rockAmount;
    float maxRockStrength;
};

#endif // UNDERWATEREROSION_H
