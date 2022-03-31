#ifndef UNDERWATEREROSION_H
#define UNDERWATEREROSION_H

#include "TerrainGen/VoxelGrid.h"

#include <tuple>
#include "Utils/BSpline.h"
#include "Karst/KarstHoleProfile.h"

class UnderwaterErosion
{
public:
    UnderwaterErosion();
    UnderwaterErosion(std::shared_ptr<VoxelGrid> grid, int maxRockSize, float maxRockStrength, int rockAmount);

//    std::vector<std::vector<Vector3>> Apply(int avoidMatter = -1);
    std::tuple<std::vector<std::vector<Vector3>>, std::vector<std::vector<Vector3>>> Apply(std::shared_ptr<Vector3> startingPoint = nullptr, std::shared_ptr<Vector3> originalDirection = nullptr, int avoidMatter = 10, float flowfieldFactor = 0.1, float randomnessFactor = 0.05, bool returnEvenLostRocks = false);
    std::tuple<std::vector<std::vector<Vector3>>, std::vector<std::vector<Vector3>>> Apply(Vector3 startingPoint, Vector3 originalDirection, int avoidMatter = 10, float flowfieldFactor = 0.1, float randomnessFactor = 0.05, bool returnEvenLostRocks = false);
    std::vector<Vector3> CreateTunnel(int numberPoints = 2, bool addingMatter = false, bool applyChanges = true, KarstHolePredefinedShapes startingShape = SOLUBLE_BED, KarstHolePredefinedShapes endingShape = KEYHOLE);
    std::vector<Vector3> CreateTunnel(BSpline path, bool addingMatter = false, bool usingSpheres = true, bool applyChanges = true, KarstHolePredefinedShapes startingShape = SOLUBLE_BED, KarstHolePredefinedShapes endingShape = KEYHOLE);
    std::vector<std::vector<Vector3>> CreateMultipleTunnels(std::vector<BSpline> paths, bool addingMatter = false, bool usingSpheres = true, bool applyChanges = true, KarstHolePredefinedShapes startingShape = SOLUBLE_BED, KarstHolePredefinedShapes endingShape = KEYHOLE);
    std::vector<Vector3> CreateCrack(Vector3 start, Vector3 end, bool applyChanges = true);


    std::shared_ptr<VoxelGrid> grid;
    int maxRockSize;
    int rockAmount;
    float maxRockStrength;
};

#endif // UNDERWATEREROSION_H
