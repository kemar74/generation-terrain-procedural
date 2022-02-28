#ifndef KARSTHOLEPROFILE_H
#define KARSTHOLEPROFILE_H

#include <vector>
#include "DataStructure/Vector3.h"
#include "Utils/BSpline.h"

enum KarstHolePredefinedShapes
{
    TUBE = 0,
    SOLUBLE_BED = 1,
    PASSAGE = 2,
    KEYHOLE = 3,
    CANYON = 4,
};

class KarstHoleProfile
{
public:
    KarstHoleProfile();
    KarstHoleProfile(KarstHolePredefinedShapes shape);
    KarstHoleProfile(BSpline shape);
    KarstHoleProfile(std::vector<Vector3> shape);

    KarstHoleProfile& rotateTowardVector(Vector3 new_dir);
    KarstHoleProfile& translate(Vector3 new_pos);
    KarstHoleProfile& scale(float scale_factor);
    KarstHoleProfile interpolate(KarstHoleProfile other, BSpline path, float t);
    std::pair<KarstHoleProfile, std::vector<std::vector<Vector3>>> interpolateAndGetMesh(KarstHoleProfile other, BSpline path, float t);

    KarstHoleProfile& setNumberOfVertices(int vertice_count);
    KarstHoleProfile& setSize(float sizeX, float sizeY);

    std::vector<std::vector<int>> computeTrianglesIndices(const std::vector<Vector3>& points);

    static BSpline createTubeProfile();
    static BSpline createSolubleBedProfile();
    static BSpline createPassageProfile();
    static BSpline createKeyholeProfile();
    static BSpline createCanyonProfile();

    BSpline vertices;
};

#endif // KARSTHOLEPROFILE_H
