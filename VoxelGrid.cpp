#include "VoxelGrid.h"
#include "FastNoiseLit.h"
#include "UnderwaterErosion.h"

#include "Shader.h"

VoxelGrid::VoxelGrid(int nx, int ny, int nz, float blockSize, float noise_shifting)
    : sizeX(nx), sizeY(ny), sizeZ(nz), blockSize(blockSize), noise_shifting(noise_shifting) {
    chunkSize = (nx > chunkSize ? chunkSize : nx);


    sizeX -= (sizeX % chunkSize);
    sizeY -= (sizeY % chunkSize);

    // Create and configure FastNoise object
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(blockSize / (float) sizeX);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalLacunarity(2.0);
    noise.SetFractalGain(0.7);
    noise.SetFractalWeightedStrength(0.5);
    noise.SetFractalOctaves(10);

    float mean = 0.f, min = 1000.f, max = -1000.f;
    for (int x = 0; x < this->sizeX; x++)
        for (int y = 0; y < this->sizeY; y++)
            for (int h = 0; h < this->sizeZ; h++) {
                float n = noise.GetNoise((float)x, (float)y, (float)h);
                mean += n;
                min = std::min(min, n);
                max = std::max(max, n);
            }
    mean /= (this->sizeX * this->sizeY * this->sizeZ);
    std::cout << mean << " " << min << " " << max << std::endl;
//    exit(0);


    int numberOfChunksX = this->sizeX / chunkSize;
    int numberOfChunksY = this->sizeY / chunkSize;

    this->voxels.clear();
    this->chunks.clear();
    for (int xChunk = 0; xChunk < sizeX / chunkSize; xChunk++) {
        for (int yChunk = 0; yChunk < sizeY / chunkSize; yChunk++) {
            std::vector<std::vector<std::vector<float>>> iso_data;
            for (int x = 0; x < chunkSize; x++) {
                iso_data.push_back(std::vector<std::vector<float>>());
                for (int y = 0; y < chunkSize; y++) {
                    iso_data[x].push_back(std::vector<float>());
                    for(int h = 0; h < this->getSizeZ(); h++) {
                        float noise_val = ((noise.GetNoise((float)xChunk * chunkSize + x, (float)yChunk * chunkSize + y, (float)h) - min) / (max - min)) * 2.0 - 1.0;
                        noise_val += noise_shifting;
                        noise_val *= 2.0;
                        iso_data[x][y].push_back(noise_val);
                    }
                }
            }
            this->chunks.push_back(new VoxelChunk(xChunk * chunkSize, yChunk * chunkSize, chunkSize, chunkSize, this->getSizeZ(), iso_data, this));
            if (xChunk == numberOfChunksX - 1) {
                this->chunks[this->chunks.size() - 1]->lastChunkOnX = true;
            }
            if (yChunk == numberOfChunksY - 1) {
                this->chunks[this->chunks.size() - 1]->lastChunkOnY = true;
            }
        }
    }

    for (int xChunk = 0; xChunk < this->sizeX / chunkSize; xChunk++) {
        for (int yChunk = 0; yChunk < this->sizeY / chunkSize; yChunk++) {
            int current = xChunk * numberOfChunksY + yChunk;
            if (xChunk > 0) {
                this->chunks[current]->neighboring_chunks[LEFT] = this->chunks[current - numberOfChunksY];
                this->chunks[current - numberOfChunksY]->neighboring_chunks[RIGHT] = this->chunks[current];
            }
            if (yChunk > 0) {
                this->chunks[current]->neighboring_chunks[FRONT] = this->chunks[current - 1];
                this->chunks[current - 1]->neighboring_chunks[BACK] = this->chunks[current];
            }
        }
    }

    for (VoxelChunk* c: this->chunks) {
        c->applyToVoxels([](Voxel* v) -> void { v->resetNeighbors(); });
        c->createMesh();
    }

}
VoxelGrid::VoxelGrid(Grid& grid) : VoxelGrid(grid.getSizeX(), grid.getSizeY(), grid.getMaxHeight(), grid.getTileSize()) {
    this->from2DGrid(grid);
}
VoxelGrid::VoxelGrid() : VoxelGrid(10, 10, 10, 1.0) {

}
void VoxelGrid::from2DGrid(Grid grid) {
    chunkSize = (sizeX > chunkSize ? chunkSize : sizeX);
    this->voxels.clear();
    this->chunks.clear();
    this->sizeX = grid.getSizeX() - (grid.getSizeX() % chunkSize);
    this->sizeY = grid.getSizeY() - (grid.getSizeY() % chunkSize);
    this->sizeZ = grid.getMaxHeight();
    for (int xChunk = 0; xChunk < sizeX / chunkSize; xChunk++) {
        for (int yChunk = 0; yChunk < sizeY / chunkSize; yChunk++) {
            std::vector<std::vector<std::vector<float>>> data;
            for (int x = 0; x < chunkSize; x++) {
                data.push_back(std::vector<std::vector<float>>());
                for (int y = 0; y < chunkSize; y++) {
                    data[x].push_back(std::vector<float>());
                    float grid_height = grid.getHeight(xChunk * chunkSize + x, yChunk * chunkSize + y) * (this->sizeZ / grid.getMaxHeight());
                    int z = int(grid_height)+1;
                    for (int i = 0; i < z; i++)
                        data[x][y].push_back(1.0); // TerrainTypes::DIRT);
                    for (int i = z; i < this->getSizeZ(); i++)
                        data[x][y].push_back(-1.0); // TerrainTypes::AIR);
                }
            }
//            data[1][1][0] = TerrainTypes::AIR;
            this->chunks.push_back(new VoxelChunk(xChunk * chunkSize, yChunk * chunkSize, chunkSize, chunkSize, this->getSizeZ(), data, this));
            if (xChunk == int(sizeX / chunkSize) - 1) {
                this->chunks[this->chunks.size() - 1]->lastChunkOnX = true;
            }
            if (yChunk == int(sizeY / chunkSize) - 1) {
                this->chunks[this->chunks.size() - 1]->lastChunkOnY = true;
            }
        }
    }
    for(size_t i = 0; i < this->chunks.size(); i++) {
        if (i > this->sizeY/chunkSize - 1) {
            this->chunks[i]->neighboring_chunks[LEFT] = this->chunks[i - int(this->sizeY / chunkSize)];
            this->chunks[i - int(this->sizeY/chunkSize)]->neighboring_chunks[RIGHT] = this->chunks[i];
        }
        if (i % int(this->sizeY/chunkSize) >= 1) {
            this->chunks[i]->neighboring_chunks[FRONT] = this->chunks[i - 1];
            this->chunks[i - 1]->neighboring_chunks[BACK] = this->chunks[i];
        }
    }

    this->createMesh();
}

void VoxelGrid::createMesh()
{
    for(VoxelChunk* vc : this->chunks)
    {
        vc->needRemeshing = true;
        vc->createMesh(this->displayWithMarchingCubes, true);
        /*
        MarchingCubes mc(vc);
        mc.createMesh();

        this->vertexArray.insert(this->vertexArray.end(), mc.vertexArray.begin(), mc.vertexArray.end());
        this->vertexArrayFloat.insert(this->vertexArrayFloat.end(), mc.vertexArrayFloat.begin(), mc.vertexArrayFloat.end());
*/
    }
}

void VoxelGrid::makeItFall(int groupId)
{
    UnderwaterErosion erod(this, 10, 0.01, 100);
    for(int i = 0; i < 1; i++) {
        for(VoxelChunk* vc : this->chunks) {
            vc->makeItFall(groupId);
        }
    }
    remeshAll();
    erod.Apply();
}

void VoxelGrid::display() {
    for (VoxelChunk* vc : this->chunks)
    {
        Shader::shaders[0]->setFloat("offsetX", -this->sizeX/2 + vc->x); // + (vc.lastChunkOnX ? 0 : 1 * ((float)this->sizeX/(float)vc.x)));
        Shader::shaders[0]->setFloat("offsetY", -this->sizeY/2 + vc->y); // + (vc.lastChunkOnY ? 0 : 1 * ((float)this->sizeY/(float)vc.y)));
        vc->display();
    }
}

int VoxelGrid::getHeight(int x, int y) {
    int maxHeight = -1;
    for (Voxel v : this->voxels) {
        if (v.getX() == x && v.getY() == y && v.getZ() > maxHeight)
            maxHeight = v.getZ();
    }
    return maxHeight;
}

bool VoxelGrid::contains(Vector3 v) {
    return this->contains(v.x, v.y, v.z);
}

bool VoxelGrid::contains(float x, float y, float z) {
    return (0 <= x && x < this->sizeX && 0 <= y && y < this->sizeY && 0 <= z && z < this->sizeZ);
}
Voxel* VoxelGrid::getVoxel(Vector3 pos) {
    return this->getVoxel(pos.x, pos.y, pos.z);
}

Voxel* VoxelGrid::getVoxel(float x, float y, float z) {
    if(z < 0 || x < 0 || y < 0)
        return nullptr;
    int _x = x, _y = y, _z = z;
    int xChunk = int(_x / chunkSize);
    int voxPosX = _x % chunkSize;
    int yChunk = _y / chunkSize;
    int voxPosY = _y % chunkSize;

    if (xChunk < 0 || yChunk < 0 || _x >= getSizeX() || _y >= getSizeY() || _z >= this->getSizeZ())
        return nullptr;
    return this->chunks[xChunk * (this->sizeY / chunkSize) + yChunk]->voxels[voxPosX][voxPosY][_z];
}

void VoxelGrid::remeshAll()
{
    for (VoxelChunk* vc : this->chunks)
        vc->createMesh(this->displayWithMarchingCubes);
    /*for (VoxelChunk& vc : this->chunks)
        vc.applyToVoxels([](Voxel* v) -> void { v->resetNeighbors(); });*/
}

#include <sstream>
std::string VoxelGrid::toString()
{
    std::ostringstream ret;
    ret << "{";
    ret << "\n\t\"sizeX\": \"" << sizeX << "\",\n\t\"sizeY\": \"" << sizeY << "\",\n\t\"sizeZ\": \"" << sizeZ;
    ret << "\",\n\t\"blockSize\": \"" << blockSize << "\",\n\t\"noise_shifting\": \"" << noise_shifting;
    ret << "\",\n\t\"chunkSize\": \"" << chunkSize << "\",\n\t\"numberOfVoxels\": \"" << sizeX*sizeY*sizeZ;
    ret << "\",\n\t\"numberOfChunks\": \"" << chunks.size() << "\"\n}";
    return ret.str();
}
std::string VoxelGrid::toShortString()
{
    std::ostringstream ret;
    ret << "X" << sizeX << "Y" << sizeY << "Z" << sizeZ;
    ret << "BS" << blockSize << "NS" << noise_shifting;
    ret << "CS" << chunkSize;
    return ret.str();
}
