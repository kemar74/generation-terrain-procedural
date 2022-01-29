#include "KarstPathGenerationInterface.h"

#include "TerrainModification/UnderwaterErosion.h"
#include "Interface/InterfaceUtils.h"


KarstPathGenerationInterface::KarstPathGenerationInterface()
{
}

/*
KarstPathGenerationInterface::KarstPathGenerationInterface(KarstPathsGeneration karstCreator, Vector3 AABBoxMinPos, Vector3 AABBoxMaxPos)
    : AABBoxMinPos(AABBoxMinPos), AABBoxMaxPos(AABBoxMaxPos)
{
    this->karstCreator = karstCreator;

    this->fractureVector = new InteractiveVector(AABBoxMinPos, Vector3(AABBoxMinPos.x, AABBoxMinPos.y, AABBoxMaxPos.z));
    this->waterHeightSlider = new Slider3D(Vector3(AABBoxMinPos.x - 10, AABBoxMinPos.y - 10, AABBoxMinPos.z), Vector3(AABBoxMinPos.x - 10, AABBoxMinPos.y - 10, AABBoxMaxPos.z), 0.f, AABBoxMinPos.z, AABBoxMaxPos.z);

    QObject::connect(this->fractureVector, &InteractiveVector::modified, this, &KarstPathGenerationInterface::updateFracture);
    QObject::connect(this->waterHeightSlider, &Slider3D::valueChanged, this, &KarstPathGenerationInterface::updateWaterHeight);

    Vector3 offX = Vector3(AABBoxMaxPos.x - AABBoxMinPos.x, 0, 0);
    Vector3 offY = Vector3(0, AABBoxMaxPos.y - AABBoxMinPos.y, 0);
    Vector3 offZ = Vector3(0, 0, this->waterHeightSlider->sliderControlPoint->position.z);
    this->waterLevelMesh.fromArray({
                                       {AABBoxMinPos + offZ, AABBoxMinPos + offX + offZ,
                                        AABBoxMinPos + offX + offZ, AABBoxMinPos + offX + offY + offZ,
                                        AABBoxMinPos + offX + offY + offZ, AABBoxMinPos + offY + offZ,
                                        AABBoxMinPos + offY + offZ, AABBoxMinPos + offZ}
                                   });
}
*/
void KarstPathGenerationInterface::display()
{
    if (!isHidden)
    {
        this->fractureVector->display();
        this->pathsMeshes.shader->setVector("color", std::vector<float>({0/255.f, 0/255.f, 255/255.f, 1.f}));
        this->waterLevelMesh.display(GL_LINES, 5.f);
        this->pathsMeshes.shader->setVector("color", std::vector<float>({0/255.f, 255/255.f, 0/255.f, 1.f}));
        this->pathsMeshes.display(GL_LINES, 5.f);
        this->waterHeightSlider->display();
    }
}

void KarstPathGenerationInterface::affectVoxelGrid(std::shared_ptr<VoxelGrid> voxelGrid)
{
    this->voxelGrid = voxelGrid;
    this->AABBoxMinPos = Vector3(0, 0, 0);
    this->AABBoxMaxPos = Vector3(voxelGrid->sizeX, voxelGrid->sizeY, voxelGrid->sizeZ) * voxelGrid->blockSize;
    this->fractureVector = new InteractiveVector(AABBoxMinPos, Vector3(AABBoxMinPos.x, AABBoxMinPos.y, AABBoxMaxPos.z));
    this->waterHeightSlider = new Slider3D(Vector3(AABBoxMinPos.x - 10, AABBoxMinPos.y - 10, AABBoxMinPos.z), Vector3(AABBoxMinPos.x - 10, AABBoxMinPos.y - 10, AABBoxMaxPos.z), 0.f, AABBoxMinPos.z, AABBoxMaxPos.z);
    this->waterHeightSlider->sliderControlPoint->setGrabberStateColor({
                                                                          {INACTIVE, {0/255.f, 0/255.f, 180/255.f, 1.f}},
                                                                          {ACTIVE, {0/255.f, 0/255.f, 255/255.f, 1.f}},
                                                                      });

    QObject::connect(this->fractureVector, &InteractiveVector::modified, this, &KarstPathGenerationInterface::updateFracture);
    QObject::connect(this->waterHeightSlider, &Slider3D::valueChanged, this, &KarstPathGenerationInterface::updateWaterHeight);

    Vector3 offX = Vector3(AABBoxMaxPos.x - AABBoxMinPos.x, 0, 0);
    Vector3 offY = Vector3(0, AABBoxMaxPos.y - AABBoxMinPos.y, 0);
    Vector3 offZ = Vector3(0, 0, this->waterHeightSlider->sliderControlPoint->position.z);
    this->waterLevelMesh.fromArray({
                                       {AABBoxMinPos + offZ, AABBoxMinPos + offX + offZ,
                                        AABBoxMinPos + offX + offZ, AABBoxMinPos + offX + offY + offZ,
                                        AABBoxMinPos + offX + offY + offZ, AABBoxMinPos + offY + offZ,
                                        AABBoxMinPos + offY + offZ, AABBoxMinPos + offZ}
                                   });


    Matrix3<int> availableGrid(voxelGrid->sizeX, voxelGrid->sizeY, voxelGrid->sizeZ, 0);
    for (int x = 0; x < availableGrid.sizeX; x++) {
        for (int y = 0; y < availableGrid.sizeY; y++) {
            for (int z = 0; z < availableGrid.sizeZ; z++) {
                float voxelVal = voxelGrid->getVoxelValue(x, y, z);
                if (voxelVal > 0) {
                    availableGrid.at(x, y, z) = 1;
                }
            }
        }
    }

    this->karstCreator = new KarstPathsGeneration(availableGrid, Vector3(voxelGrid->sizeX, voxelGrid->sizeY, voxelGrid->sizeZ), 10.f);
}

void KarstPathGenerationInterface::updateFracture(Vector3 newFractureDir)
{
    if (karstCreator->fracturesDirections.empty())
        this->karstCreator->addFractureDirection(FractureDirection(Vector3(0, 0, 0), 1.f));
    karstCreator->fracturesDirections.back().direction = newFractureDir;
}

void KarstPathGenerationInterface::updateWaterHeight(float newHeight)
{
    if (karstCreator->waterHeights.empty())
        this->karstCreator->addWaterHeight(WaterHeight(0.f, 1.f));
    karstCreator->waterHeights.back().height = newHeight;
    Vector3 offX = Vector3(AABBoxMaxPos.x - AABBoxMinPos.x, 0, 0);
    Vector3 offY = Vector3(0, AABBoxMaxPos.y - AABBoxMinPos.y, 0);
    Vector3 offZ = Vector3(0, 0, this->waterHeightSlider->sliderControlPoint->position.z);
    this->waterLevelMesh.fromArray({
                                       {AABBoxMinPos + offZ, AABBoxMinPos + offX + offZ,
                                        AABBoxMinPos + offX + offZ, AABBoxMinPos + offX + offY + offZ,
                                        AABBoxMinPos + offX + offY + offZ, AABBoxMinPos + offY + offZ,
                                        AABBoxMinPos + offY + offZ, AABBoxMinPos + offZ}
                                   });
}

void KarstPathGenerationInterface::computeKarst()
{
    Matrix3<float> porosityMap(voxelGrid->sizeX, voxelGrid->sizeY, voxelGrid->sizeZ, 1.f);
    for (int x = 0; x < porosityMap.sizeX; x++) {
        for (int y = 0; y < porosityMap.sizeY; y++) {
            for (int z = 0; z < porosityMap.sizeZ; z++) {
                float voxelVal = voxelGrid->getVoxelValue(x, y, z);
                if (voxelVal > 0) {
                    porosityMap.at(x, y, z) = voxelVal;
                }
            }
        }
    }
    this->karstCreator->porosityMap = porosityMap;
    int nb_special_nodes = 10;
    this->karstCreator->resetSpecialNodes();
    this->karstCreator->setSpecialNode(0, SOURCE);
    for (int i = 0; i < nb_special_nodes - 1; i++)
        this->karstCreator->setSpecialNode((i + 1) * this->karstCreator->graph.nodes.size() / nb_special_nodes, FORCED_POINT);

    this->karstCreator->createEdges(10, 50.f);
    this->karstCreator->computeAllPathsBetweenSpecialNodes();

    this->updateKarstPath();

}

void KarstPathGenerationInterface::updateKarstPath()
{
    this->karstPaths.clear();
    std::vector<Vector3> pathPositions;
    for (const auto& path : this->karstCreator->finalPaths)
    {
        for (size_t i = 0; i < path.size() - 1; i++) {
            pathPositions.push_back(this->karstCreator->getNodePos(path[i    ])); // - Vector3(voxelGrid->sizeX/2.0, voxelGrid->sizeY/2.0, 0.0));
            pathPositions.push_back(this->karstCreator->getNodePos(path[i + 1])); // - Vector3(voxelGrid->sizeX/2.0, voxelGrid->sizeY/2.0, 0.0));
            karstPaths.push_back(BSpline(std::vector<Vector3>({this->karstCreator->getNodePos(path[i    ]),
                                                               this->karstCreator->getNodePos(path[i + 1])})));
        }
    }
    this->pathsMeshes.fromArray(pathPositions);
//    this->display();
}

void KarstPathGenerationInterface::createKarst()
{
    UnderwaterErosion erod(this->voxelGrid, 20.f, 2.f, 10);
    erod.CreateMultipleTunnels(this->karstPaths);
    update();
}



QHBoxLayout *KarstPathGenerationInterface::createGUI()
{
    this->karstCreationLayout = new QHBoxLayout;

    QPushButton* karstCreationPreviewButton = new QPushButton("Calculer");
    QPushButton* karstCreationConfirmButton = new QPushButton("Creer le karst");
    QCheckBox* karstCreationDisplay = new QCheckBox("Afficher");
    FancySlider* karstCreationDistanceWeights = new FancySlider(Qt::Orientation::Horizontal, 0, 10, 0.1);
    FancySlider* karstCreationFractureWeights = new FancySlider(Qt::Orientation::Horizontal, 0, 10, 0.1);
    FancySlider* karstCreationWaterWeights = new FancySlider(Qt::Orientation::Horizontal, 0, 10, 0.1);
    FancySlider* karstCreationPorosityWeights = new FancySlider(Qt::Orientation::Horizontal, 0, 10, 0.1);
    FancySlider* karstCreationGamma = new FancySlider(Qt::Orientation::Horizontal, 1, 10, 0.1);
    FancySlider* karstCreationTortuosity = new FancySlider(Qt::Orientation::Horizontal, 0, 10, 0.1);
    karstCreationLayout->addWidget(createVerticalGroup({karstCreationPreviewButton, karstCreationConfirmButton}));
    karstCreationLayout->addWidget(createMultipleSliderGroup({
                                                           {"Distances", karstCreationDistanceWeights},
                                                           {"Fractures", karstCreationFractureWeights},
                                                           {"Porosite", karstCreationPorosityWeights},
                                                           {"Niveau d'eau", karstCreationWaterWeights},
                                                       }));
    karstCreationLayout->addWidget(createVerticalGroup({
                                                           createSliderGroup("Gamma", karstCreationGamma),
                                                           createSliderGroup("Tortuosite (m)", karstCreationTortuosity)
                                                       }));
    karstCreationLayout->addWidget(karstCreationDisplay);

    QObject::connect(karstCreationPreviewButton, &QPushButton::pressed, this, [=](){ this->computeKarst(); } );
    QObject::connect(karstCreationConfirmButton, &QPushButton::pressed, this, [=](){ this->createKarst(); } );
    QObject::connect(karstCreationDistanceWeights, &FancySlider::floatValueChanged, this, [=](float val){ this->karstCreator->distanceWeight = val; } );
    QObject::connect(karstCreationFractureWeights, &FancySlider::floatValueChanged, this, [=](float val){ this->karstCreator->fractureWeight = val; } );
    QObject::connect(karstCreationWaterWeights, &FancySlider::floatValueChanged, this, [=](float val){ this->karstCreator->waterHeightWeight = val; } );
    QObject::connect(karstCreationPorosityWeights, &FancySlider::floatValueChanged, this, [=](float val){ this->karstCreator->porosityWeight = val; } );
    QObject::connect(karstCreationGamma, &FancySlider::floatValueChanged, this, [=](float val){ this->karstCreator->gamma = val; } );
    QObject::connect(karstCreationTortuosity, &FancySlider::floatValueChanged, this, [=](float val){ this->karstCreator->updateTortuosity(val); this->updateKarstPath(); } );
    QObject::connect(karstCreationDisplay, &QCheckBox::toggled, this, [=](bool display){ this->isHidden = !display; } );


    if (this->karstCreator) {
        karstCreationDistanceWeights->setfValue(this->karstCreator->distanceWeight);
        karstCreationFractureWeights->setfValue(this->karstCreator->fractureWeight);
        karstCreationWaterWeights->setfValue(this->karstCreator->waterHeightWeight);
        karstCreationPorosityWeights->setfValue(this->karstCreator->porosityWeight);
        karstCreationGamma->setfValue(this->karstCreator->gamma);
    }
    karstCreationTortuosity->setfValue(0.f);
    karstCreationDisplay->setChecked(!this->isHidden);

    return this->karstCreationLayout;
}
