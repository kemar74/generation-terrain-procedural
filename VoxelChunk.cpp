#include "VoxelChunk.h"

#include <set>
#include <unordered_set>
#include <chrono>

Vector3 HSVtoRGB(float H, float S,float V);

VoxelChunk::VoxelChunk(int x, int y, int sizeX, int sizeY, int height, Matrix3<float> iso_data, std::shared_ptr<VoxelGrid> parent)
    : iso_data(iso_data), x(x), y(y), sizeX(sizeX), sizeY(sizeY), height(height), parent(parent) {

    this->voxelGroups = Matrix3<int>(this->sizeX, this->sizeY, this->height);
    for(int v_x = 0; v_x < sizeX; v_x++) {
        for(int v_y = 0; v_y < sizeY; v_y++) {
            for(int h = 0; h < height; h++) {
                this->voxelGroups(v_x, v_y, h) = -1;
            }
        }
    }
    this->mesh.useIndices = false;
    this->voxelValues = iso_data;
    this->originalVoxelValues = this->voxelValues;
    this->flowField = Matrix3<Vector3>(this->sizeX, this->sizeY, this->height);
    this->updateLoDsAvailable();
    this->computeDistanceField();
}

VoxelChunk::VoxelChunk() : VoxelChunk(0, 0, 0, 0, 0, Matrix3<float>(), nullptr)
{

}
VoxelChunk::~VoxelChunk()
{
    this->voxels.clear();
}

void VoxelChunk::createVoxels()
{
    if (needRemeshing)
    {
        this->voxels = Matrix3<std::shared_ptr<Voxel>>(this->sizeX, this->sizeY, this->height);
        for(int x = 0; x < this->sizeX; x++) {
            for(int y = 0; y < this->sizeY; y++) {
                for(int z = 0; z < this->height; z++) {
                    std::shared_ptr<Voxel> v(new Voxel(x, y, z, voxelValues(x, y, z) > 0.0 ? DIRT : AIR, 1.0, voxelValues(x, y, z), this->shared_from_this()));
                }
            }
        }
    }
}
void VoxelChunk::computeFlowfield(Vector3 sea_current) //int blur_iterations)
{
    return;
//    if (!needRemeshing)
//        return;
    /*
    for(int v_x = 0; v_x < sizeX; v_x++) {
        for(int v_y = 0; v_y < sizeY; v_y++) {
            for(int h = 0; h < height; h++) {
                flowField(v_x, v_y, h) = this->computeNormal(v_x + this->x, v_y + this->y, h);
            }
        }
    }
    for (int iter_blur = 0; iter_blur < blur_iterations; iter_blur++) {
        Matrix3<Vector3> originalFlowField = this->flowField;
        for(int v_x = 1; v_x < sizeX - 1; v_x++) {
            for(int v_y = 1; v_y < sizeY - 1; v_y++) {
                for(int h = 1; h < height - 1; h++) {
                    for(int d_x = -1; d_x <= 1; d_x++) {
                        for(int d_y = -1; d_y <= 1; d_y++) {
                            for(int d_h = -1; d_h <= 1; d_h++) {
                                flowField(v_x, v_y, h) += originalFlowField(v_x + d_x, v_y + d_y, h + d_h) / 27.0;
                            }
                        }
                    }
                }
            }
        }
    }
    for(int v_x = 1; v_x < sizeX-1; v_x++) {
        for(int v_y = 1; v_y < sizeY-1; v_y++) {
            for(int h = 1; h < height-1; h++) {
                    flowField(v_x, v_y, h) /= (blur_iterations + 1);
//                flowField(v_x, v_y, h).normalize();
            }
        }
    }*/
    computeDistanceField();
    Matrix3<Vector3> pressionGradient = this->pressureField.gradient();
    this->flowField = pressionGradient * (-1.f) + sea_current;
//    if(sea_current.norm() == 0)
//        return;
    /*
    for (size_t x = 0; x < flowField.size(); x++) {
        for (size_t y = 0; y < flowField(x).size(); y++) {
            for (size_t z = 0; z < flowField(x, y).size(); z++) {
                flowField(x, y, z) *= -1.0; // Inverse the gradient
                flowField(x, y, z) += sea_current;
            }
        }
    }*/
}

void VoxelChunk::computeDistanceField()
{
    this->distanceField = Matrix3<int>(this->sizeX, this->sizeY, this->height, 9999999);
    this->pressureField = Matrix3<float>(this->sizeX, this->sizeY, this->height, 0);
    // Check left
    for (int x = 1; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            for (int z = 0; z < this->height; z++) {
                if (this->voxelValues(x, y, z) > 0) {
                    distanceField(x, y, z) = 0;
                } else {
                    distanceField(x, y, z) = std::min(distanceField(x, y, z), distanceField(x - 1, y, z) + 1);
                }
            }
        }
    }
    // Check right
    for (int x = 0; x < this->sizeX - 1; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            for (int z = 0; z < this->height; z++) {
                if (this->voxelValues(x, y, z) > 0) {
                    distanceField(x, y, z) = 0;
                } else {
                    distanceField(x, y, z) = std::min(distanceField(x, y, z), distanceField(x + 1, y, z) + 1);
                }
            }
        }
    }
    // Check front
    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 1; y < this->sizeY; y++) {
            for (int z = 0; z < this->height; z++) {
                if (this->voxelValues(x, y, z) > 0) {
                    distanceField(x, y, z) = 0;
                } else {
                    distanceField(x, y, z) = std::min(distanceField(x, y, z), distanceField(x, y - 1, z) + 1);
                }
            }
        }
    }
    // Check back
    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY - 1; y++) {
            for (int z = 0; z < this->height; z++) {
                if (this->voxelValues(x, y, z) > 0) {
                    distanceField(x, y, z) = 0;
                } else {
                    distanceField(x, y, z) = std::min(distanceField(x, y, z), distanceField(x, y + 1, z) + 1);
                }
            }
        }
    }
    // Check down
    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            for (int z = 1; z < this->height; z++) {
                if (this->voxelValues(x, y, z) > 0) {
                    distanceField(x, y, z) = 0;
                } else {
                    distanceField(x, y, z) = std::min(distanceField(x, y, z), distanceField(x, y, z - 1) + 1);
                }
            }
        }
    }
    // Check up
    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            for (int z = 0; z < this->height - 1; z++) {
                if (this->voxelValues(x, y, z) > 0) {
                    distanceField(x, y, z) = 0;
                } else {
                    distanceField(x, y, z) = std::min(distanceField(x, y, z), distanceField(x, y, z + 1) + 1);
                }
            }
        }
    }
    // compute pressure ( 1 / distance^2 ?)
    for (int x = 0; x < this->sizeX; x++) {
        for (int y = 0; y < this->sizeY; y++) {
            for (int z = 0; z < this->height; z++) {
                if (distanceField(x, y, z) > 0) // Works better with 1/dist than 1/dist^2
                    this->pressureField(x, y, z) = 1 / (float)this->distanceField(x, y, z); //std::min(1.f , 1 / ((float)std::pow(this->distanceField(x, y, z), 2)));
                else
                    this->pressureField(x, y, z) = 1.0;
            }
        }
    }
}

void VoxelChunk::updateLoDsAvailable() {
    this->LoDs.clear();
    int x_add = 1; //(this->x == 0 ? 0 : 0);
    int y_add = 1; //(this->y == 0 ? 0 : 0);
    int z_add = 1;
    for (int i = 1; i < std::min(std::min(this->sizeX, this->sizeY), this->height); i++)
        if ((this->sizeX-x_add) % i == 0 && (this->sizeY-y_add) % i == 0 && (this->height-z_add) % i == 0)
            this->LoDs.push_back(i);
}
void VoxelChunk::createMesh(bool applyMarchingCubes, bool updateMesh) {
    if (!needRemeshing)
        return;
//    this->toVoxels();
    Voxel::currentLabelIndex = 1;
    Voxel::voxelGroups.clear();
    Voxel::voxelGroups.push_back(std::set<int>()); // First group reserved for the ones touching the ground

    if (!updateMesh)
        return;
    needRemeshing = false;

    std::vector<Vector3> colors;
    std::vector<Vector3> voxelVertices;
    if (applyMarchingCubes) {
        std::cout << "Apply MC" << std::endl;
        voxelVertices = this->applyMarchingCubes(true, &colors);
        std::cout << "End MC" << std::endl;
    }
    else {
        colors.reserve(this->sizeX * this->sizeY * this->height * 6);
        voxelVertices.reserve(this->sizeX * this->sizeY * this->height * 6);
        for (int x = 0; x < this->sizeX; x++) {
            for (int y = 0; y < this->sizeY; y++) {
                for (int z = 0; z < this->height; z++) {
                    if (this->voxelValues(x, y, z) > 0.0) {
                        Voxel v(x, y, z, TerrainTypes::DIRT, 1.0, this->voxelValues(x, y, z), this->shared_from_this());
                        // Add the vertices to the global mesh
                        std::vector<Vector3> vertice = v.getMeshVertices(true);
                        voxelVertices.insert(voxelVertices.end(), vertice.begin(), vertice.end());
                        // Add the colors to each vertex
                        int X = 6; // Start with 6 faces
//                        for(auto& n : v->neighbors)
//                            if (n.second && (bool)*n.second)
//                                X--;    // Remove a face per neighbor
                        X *= 6; // Multiply the number of face by the 6 vertex that defines it (2 triangles)
                        for (int xx = 0; xx < X; xx++) {
                            colors.push_back(Vector3((this->voxelGroups(x, y, z) == 0 ? 1.0 : 0.0), (this->voxelGroups(x, y, z) == 0 ? 0.0 : 1.0), 1.0));
        //                        colors->push_back(HSVtoRGB((voxels(i, j, k)->group/((float)Voxel::voxelGroups.size()+1)), 1.0, 1.0));
                        }
                    }
                }
            }
        }
        /*this->applyToVoxels((&)(std::shared_ptr<Voxel> v) -> void {
            if ((bool)*v) {
                // Add the vertices to the global mesh
                std::vector<Vector3> vertice = v->getMeshVertices(true);
                voxelVertices.insert(voxelVertices.end(), vertice.begin(), vertice.end());
                // Add the colors to each vertex
                int X = 6; // Start with 6 faces
                for(auto& n : v->neighbors)
                    if (n.second && (bool)*n.second)
                        X--;    // Remove a face per neighbor
                X *= 6; // Multiply the number of face by the 6 vertex that defines it (2 triangles)
                for (int x = 0; x < X; x++) {
                    colors.push_back(Vector3((v->isOnGround ? 1.0 : 0.0), (v->isOnGround ? 0.0 : 1.0), 1.0));
//                        colors->push_back(HSVtoRGB((voxels(i, j, k)->group/((float)Voxel::voxelGroups.size()+1)), 1.0, 1.0));
                }
            }
        });*/
    }
    this->mesh.colorsArray = colors;
    std::cout << "creating mesh" << std::endl;
    this->mesh.fromArray(voxelVertices);
    std::cout << "Updating" << std::endl;
    this->mesh.update();
    std::cout << "End" << std::endl;
}

Vector3 VoxelChunk::computeNormal(Vector3 pos)
{
    return this->computeNormal(pos.x, pos.y, pos.z);
}
Vector3 VoxelChunk::computeNormal(int x, int y, int z)
{
    Vertex vertices[8] = {Vertex(x    , y    , z    , this->parent->getVoxelValue(x    , y    , z    )),
                          Vertex(x + 1, y    , z    , this->parent->getVoxelValue(x + 1, y    , z    )),
                          Vertex(x + 1, y + 1, z    , this->parent->getVoxelValue(x + 1, y + 1, z    )),
                          Vertex(x    , y + 1, z    , this->parent->getVoxelValue(x    , y + 1, z    )),
                          Vertex(x    , y    , z + 1, this->parent->getVoxelValue(x    , y    , z + 1)),
                          Vertex(x + 1, y    , z + 1, this->parent->getVoxelValue(x + 1, y    , z + 1)),
                          Vertex(x + 1, y + 1, z + 1, this->parent->getVoxelValue(x + 1, y + 1, z + 1)),
                          Vertex(x    , y + 1, z + 1, this->parent->getVoxelValue(x    , y + 1, z + 1))
                         };

    std::vector<Vector3> tris = this->computeMarchingCube(vertices, 0.0, false);
    Vector3 normal;
    for(size_t i = 0; i < tris.size(); i += 3)
    {
        normal += (tris[i+2] - tris[i]).cross((tris[i+1] - tris[i])) +
                    (tris[i] - tris[i+1]).cross((tris[i+2] - tris[i+1])) +
                    (tris[i+1] - tris[i+2]).cross((tris[i] - tris[i+2]));
    }
    if (tris.size() == 0)
        return normal;
    return normal.normalize();

}

std::vector<Vector3> VoxelChunk::computeMarchingCube(Vertex vertices[8], float isolevel, bool useGlobalCoords, std::vector<Vector3>* outColors)
{
    std::vector<Vector3> vertexArray;
    int cube_index = 0;
    for (int i = 0; i < 8; i++){
        if (vertices[i].isosurface > isolevel)
            cube_index ^= 1 << i;
    }
    int* edgesForTriangles = MarchingCubes::triangleTable[cube_index];
    Vertex originalVertex;
    Vertex firstVertex;
    Vertex secondVertex;
    for (int i = 0; i < 16; i++) {
        if (edgesForTriangles[i] == -1)
            continue;
        Vertex& v1 = vertices[MarchingCubes::edgeToCorner[edgesForTriangles[i]][0]];
        Vertex& v2 = vertices[MarchingCubes::edgeToCorner[edgesForTriangles[i]][1]];

        float interpolate = (isolevel - v1.isosurface) / (v2.isosurface - v1.isosurface);
        Vertex midpoint = v1 - ((v1 - v2) * interpolate);
        if (outColors != nullptr) {
//            float blueVal = this->parent->getVoxelValue(v1) - this->parent->getOriginalVoxelValue(v1) + this->parent->getVoxelValue(v2) - this->parent->getOriginalVoxelValue(v2);
//            blueVal = abs(blueVal / 2.0);$
            float blueVal = 30/255.0;
            outColors->push_back(vertices[0].isosurface > 0.5 ?  //map(x, y, z) > 0.5 ?
                                 Vector3(150/255.0, 100/255.0, blueVal) : //30/255.0) :
                                 Vector3(224/255.0, 209/255.0, blueVal)); //72/255.0));
        }
        if (i % 3 == 0) {
            originalVertex = midpoint;
        }
        else if (i % 3 == 1) {
            firstVertex = midpoint;
        }
        else {
            secondVertex = midpoint;
            Vector3 mapOffset = (useGlobalCoords ? Vector3(this->x, this->y, 0.0) : Vector3(0.0, 0.0, 0.0));
            vertexArray.insert(vertexArray.end(), {originalVertex + mapOffset,
                                                   firstVertex + mapOffset,
                                                   secondVertex + mapOffset});
        }
    }
    return vertexArray;
}
std::vector<Vector3> VoxelChunk::applyMarchingCubes(bool useGlobalCoords, std::vector<Vector3>* outColors)
{
    int LoD = this->LoDs[this->LoDIndex % this->LoDs.size()];//std::max(0, std::min(LoD, std::min(std::min(this->sizeX, this->sizeY), this->height)));
    std::vector<Vector3> colors;
    Matrix3<float> map = this->voxelValues;

    bool addedLeft = false;
    bool addedFront = false;
    if (this->neighboring_chunks.find(LEFT) != this->neighboring_chunks.end()) {
        std::shared_ptr<VoxelChunk> n = this->neighboring_chunks[LEFT];
        map.insertRow(0, 0); // Insert column at the begining of the X-axis
        for (int y = 0; y < this->sizeY; y++) {
            for (int z = 0; z < this->height; z++)
            {
                map(0, y, z) = n->voxelValues(n->sizeX - 1, y, z);
            }
        }
        addedLeft = true;
    }

    if (this->neighboring_chunks.find(FRONT) != this->neighboring_chunks.end()) {
        std::shared_ptr<VoxelChunk> n = this->neighboring_chunks[FRONT];
        int offset = addedLeft ? 1 : 0;
        map.insertRow(offset, 1);
        for (int x = 0; x < this->sizeX; x++) {
            for (int z = 0; z < this->height; z++)
            {
                map(x + offset, 0, z) = n->voxelValues(x, n->voxelValues.sizeY - 1, z);
            }
        }
        addedFront = true;
    }
    if (addedLeft && addedFront) {
        std::shared_ptr<VoxelChunk> n = this->neighboring_chunks[LEFT]->neighboring_chunks[FRONT];
        for (int z = 0; z < this->height; z++)
            map(0, 0, z) = n->voxelValues(n->voxelValues.sizeX - 1, n->voxelValues.sizeY - 1, z);
    }

    if (this->x == 0) {
        for (int y = 0; y < map.sizeY; y++)
            for (int z = 0; z < this->height; z++)
                map(0, y, z) = -1;
    }
    if (this->lastChunkOnX)
        for (int y = 0; y < map.sizeY; y++)
            for (int z = 0; z < this->height; z++)
                map(map.sizeX - 1, y, z) = -1;
    if (this->y == 0) {
        for (int x = 0; x < map.sizeX; x++)
            for (int z = 0; z < this->height; z++)
                map(x, 0, z) = -1;
    }
    if (this->lastChunkOnY)
        for (int x = 0; x < map.sizeX; x++)
            for (int z = 0; z < this->height; z++)
                map(x, map.sizeY - 1, z) = -1;

    for (int x = 0; x < map.sizeX; x++) {
        for (int y = 0; y < map.sizeY; y++) {
            map(x, y, 0) = -1;
            map(x, y, map.sizeZ - 1) = -1;
        }
    }

    float isolevel = 0.0;
    std::vector<Vector3> vertexArray;
    int x = 0, y = 0, z = 0;
    for (x = 0; x < map.sizeX - 1; x += LoD) {
        for (y = 0; y < map.sizeY - 1; y += LoD) {
            for (z = 0; z < map.sizeZ - 1; z += LoD) {
                int LoDX = std::min(LoD, int(map.sizeX - x - 1));
                int LoDY = std::min(LoD, int(map.sizeY - y - 1));
                int LoDZ = std::min(LoD, int(map.sizeZ - z - 1));
                float x_offset = x - (addedLeft ? 1.0 : 0.0);//-LoDIndex);
                float y_offset = y - (addedFront ? 1.0 : 0.0);//-LoDIndex);
                Vertex vertices[8] = {Vertex(x_offset       , y_offset       , z       , map(x       , y       , z       )),
                                      Vertex(x_offset + LoDX, y_offset       , z       , map(x + LoDX, y       , z       )),
                                      Vertex(x_offset + LoDX, y_offset + LoDY, z       , map(x + LoDX, y + LoDY, z       )),
                                      Vertex(x_offset       , y_offset + LoDY, z       , map(x       , y + LoDY, z       )),
                                      Vertex(x_offset       , y_offset       , z + LoDZ, map(x       , y       , z + LoDZ)),
                                      Vertex(x_offset + LoDX, y_offset       , z + LoDZ, map(x + LoDX, y       , z + LoDZ)),
                                      Vertex(x_offset + LoDX, y_offset + LoDY, z + LoDZ, map(x + LoDX, y + LoDY, z + LoDZ)),
                                      Vertex(x_offset       , y_offset + LoDY, z + LoDZ, map(x       , y + LoDY, z + LoDZ))
                                     };

                std::vector<Vector3> tempVerticesArray = this->computeMarchingCube(vertices, isolevel, useGlobalCoords, &colors);
                vertexArray.insert(vertexArray.end(), tempVerticesArray.begin(), tempVerticesArray.end());
            }
        }
    }
    if (outColors != nullptr)
        *outColors = colors;
    return vertexArray;
}

void VoxelChunk::makeItFall()
{
    /*this->applyToVoxels(()(std::shared_ptr<Voxel> v) -> void {
        std::shared_ptr<Voxel> v_1 = v->neighbors(TOP);
        if (v_1 == nullptr) {
            v->isosurface = -0.01; // Just destroy the top voxels
            v->manual_isosurface = 0.0;
            return;
        }
        if (v->isOnGround || v_1->isOnGround)
            return;
        v->isosurface = v_1->isosurface;
        v->manual_isosurface = v_1->manual_isosurface;
    });*/
    for (int x = 0; x < this->sizeX; x++) {
        for(int y = 0; y < this->sizeY; y++) {
            for(int z = 0; z < this->height - 1; z++) {
                if(this->voxelGroups(x, y, z) == 0 || this->voxelGroups(x, y, z+1) == 0)
                    continue;
                this->voxelValues(x, y, z) = this->voxelValues(x, y, z+1);
            }
            this->voxelValues(x, y, this->height - 1) = -1.0;
        }
    }
//    toFloat();
    this->needRemeshing = true;
}
void VoxelChunk::letGravityMakeSandFall()
{
    /*
    bool sandHadAction = false;
    this->applyToVoxels((&)(std::shared_ptr<Voxel> v) -> void {
        std::shared_ptr<Voxel> v_1 = v->neighbors(TOP);
        if(v_1 && !*v_1) // If the top neighbor is air, don't care?
            return;
        if (v->getIsosurface() > 1.5)
            return;
        if(v->z == 0)
            return;
        if(v->getType() == DIRT) { // The bottom voxel is dirt
            return;
            if (v_1 == nullptr || v_1->getType() == DIRT)
                return;
            v->isosurface += v_1->isosurface;
            v->manual_isosurface += v_1->manual_isosurface;
            sandHadAction = true;
        } else { // The bottom voxel is sand or air
            if(!v_1 || v_1->getType() == DIRT) {
                // If top voxel is dirt or air,
                v->isosurface = 0.0;
                v->manual_isosurface = 0.0;
                sandHadAction = true;
                return;
            } else {
                v->isosurface += v_1->isosurface;
                v->manual_isosurface += v_1->manual_isosurface;
                sandHadAction = true;
            }
        }
        if (v_1) {
            v_1->isosurface = -0.01;
            v_1->manual_isosurface = -0.01;
        }
    });
    this->needRemeshing = sandHadAction;
//    this->computeGroups();*/
    /*
    for (int x = 0; x < this->sizeX; x++) {
        for(int y = 0; y < this->sizeY; y++) {
            for(int z = 0; z < this->height - 1; z++) {
//                if(this->voxelValues(x, y, z+1) <= 0.0) // If it's air, don't mind
//                    continue;
                if(this->voxelValues(x, y, z+1) >= 1.0) // If top is dirt, don't
                    continue;
                if(this->voxelValues(x, y, z) >= 1.0) // If the block is staturated, skip.
                    continue;

                this->voxelValues(x, y, z) += 1 + (this->voxelValues(x, y, z+1));
                this->voxelValues(x, y, z+1) = -1.0;
            }
            this->voxelValues(x, y, this->height - 1) = -1.0;
        }
    }
    */
    float sandLowerLimit = 0.0, sandUpperLimit = 1.0;
    for (int x = 0; x < this->sizeX; x++) {
        for(int y = 0; y < this->sizeY; y++) {
            int lastSandyUnsaturatedHeight = 0;
            for(int z = 0; z < this->height; z++) {
                float cellValue = this->parent->getVoxelValue(this->x + x, this->y + y, z);
                if(cellValue < sandLowerLimit) continue;
                else if (cellValue > sandUpperLimit) {
                    lastSandyUnsaturatedHeight = z;
                } else // If it's not dirt nor air
                {
                    // Make sand fall until the cell is empty
                    while (cellValue > 0 && lastSandyUnsaturatedHeight != z) {
                        float lastSandyCellValue = this->parent->getVoxelValue(this->x + x, this->y + y, lastSandyUnsaturatedHeight);
                        float d_transported = std::min(cellValue, sandUpperLimit - lastSandyCellValue);
                        lastSandyCellValue += d_transported;
                        cellValue -= d_transported;
                        this->parent->setVoxelValue(this->x + x, this->y + y, lastSandyUnsaturatedHeight, lastSandyCellValue);
                        if(lastSandyCellValue >= sandUpperLimit) {
                            lastSandyUnsaturatedHeight ++;
                        }
                    }
                    this->parent->setVoxelValue(this->x + x, this->y + y, z, cellValue);
                }
            }
        }
    }
    this->needRemeshing = true;
}

void VoxelChunk::resetVoxelsNeighbors() {
    this->applyToVoxels([](std::shared_ptr<Voxel> v) -> void { v->resetNeighbors(); });
}

void VoxelChunk::computeGroups()
{
    // Not clean, to refactor in VoxelGrid
    if(this->x == 0 && this->y == 0) { // First chunk, reset all the voxels of all chunks
        for(std::shared_ptr<VoxelChunk>& vc : this->parent->chunks) {
            vc->applyToVoxels([](std::shared_ptr<Voxel> v) -> void {
                v->group = -1;
            });
        }
    }
    /*
    Voxel::currentLabelIndex = 1;
    Voxel::voxelGroups.clear();
    Voxel::voxelGroups.push_back(std::set<int>()); // First group reserved for the ones touching the ground

    this->applyTo(this->voxelGroups, ()(int& grp) -> void {
        grp = -1;
    });
    this->applyToVoxels(()(std::shared_ptr<Voxel> v) -> void {
        v->group = -1;
        v->isOnGround = false;
    });*/
    int calls = 0;
    std::unordered_set<std::shared_ptr<Voxel>> groundNeighbors;
    this->applyToVoxels([&calls, &groundNeighbors](std::shared_ptr<Voxel> v) -> void {
        if(!(bool)*v)
            return;
        if(v->group == 0)
            return;
        v->group = v->getZ() == 0 ? 0 : -1; // If it's touching the ground, it's directly in first group
        if (v->getZ() == 0) {
            groundNeighbors.insert(v);
            while (groundNeighbors.size() != 0) {
                std::shared_ptr<Voxel> n = (*groundNeighbors.begin());
                calls ++;
                n->isOnGround = true;
                n->group = 0;
                groundNeighbors.erase(groundNeighbors.begin());
                n->applyToNeighbors([&groundNeighbors](std::shared_ptr<Voxel> v_2) -> void {
                    if(v_2 != nullptr && !v_2->isOnGround && (bool)*v_2) {
                        groundNeighbors.insert(v_2);
                    }
                });
            }
        }
    });
}


void VoxelChunk::display()
{
    this->mesh.display();
}

bool VoxelChunk::contains(Vector3 v) {
    return this->contains(v.x, v.y, v.z);
}

bool VoxelChunk::contains(float x, float y, float z) {
    return (this->x <= x && x < this->x + this->sizeX && this->y <= y && y < this->y + this->sizeY && 0 <= z && z < this->height);
}
Matrix3<float>& VoxelChunk::toFloat() {
    this->voxelValues = Matrix3<float>(this->sizeX, this->sizeY, this->height);
    for(int x = 0; x < this->sizeX; x++) {
        for(int y = 0; y < this->sizeY; y++) {
            for(int z = 0; z < this->height; z++) {
                this->voxelValues(x, y, z) = this->voxels(x, y, z)->getIsosurface();
            }
        }
    }
    return this->voxelValues;
}
Matrix3<std::shared_ptr<Voxel>>& VoxelChunk::toVoxels() {
    return this->voxels; // Remove the function
    createVoxels();
    for(int x = 0; x < this->sizeX; x++) {
        for(int y = 0; y < this->sizeY; y++) {
            for(int z = 0; z < this->height; z++) {
                this->voxels(x, y, z)->isosurface = this->voxelValues(x, y, z);
            }
        }
    }
    return this->voxels;
}



Vector3 HSVtoRGB(float H, float S,float V){
    H *= 360;
    float s = S;
    float v = V;
    float C = s*v;
    float X = C*(1-abs(fmod(H/60.0, 2)-1));
    float m = v-C;
    float r,g,b;
    if(H >= 0 && H < 60){
        r = C,g = X,b = 0;
    }
    else if(H >= 60 && H < 120){
        r = X,g = C,b = 0;
    }
    else if(H >= 120 && H < 180){
        r = 0,g = C,b = X;
    }
    else if(H >= 180 && H < 240){
        r = 0,g = X,b = C;
    }
    else if(H >= 240 && H < 300){
        r = X,g = 0,b = C;
    }
    else{
        r = C,g = 0,b = X;
    }
    float R = (r+m);
    float G = (g+m);
    float B = (b+m);
    return Vector3(R, G, B);
}
