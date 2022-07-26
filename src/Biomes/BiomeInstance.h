#ifndef BIOMEINSTANCE_H
#define BIOMEINSTANCE_H

class BiomeInstance;
#include "Biomes/BiomeModel.h"
#include "DataStructure/Vector3.h"
#include "Utils/ShapeCurve.h"
#include <vector>

class BiomeInstance : public std::enable_shared_from_this<BiomeInstance>
{
public:
    BiomeInstance();

    static BiomeInstance fromClass(std::string className);
    int getLevel();
    void completeIfNeeded();

    std::shared_ptr<BiomeInstance> clone(ShapeCurve newArea, Vector3 newPosition = Vector3(false));
    std::shared_ptr<BiomeInstance> getPointInstance(int index);

    std::string getTextureName();

    BSpline depthShape;
    std::shared_ptr<BiomeModel> model;
    std::vector<std::shared_ptr<BiomeInstance>> instances;
    std::shared_ptr<BiomeInstance> parent;
    Vector3 position;
    std::string classname = "none";
    std::string textureClass;
    ShapeCurve area;
    int instanceID = -1;

    static std::map<int, std::shared_ptr<BiomeInstance>> instancedBiomes;
    static void registerBiomeInstance(std::shared_ptr<BiomeInstance> biome);

    void completeArch();
    void completeTrench();
    void completeArea();

    int getNumberOfPoints();

    bool valid = true;
};


#endif // BIOMEINSTANCE_H
