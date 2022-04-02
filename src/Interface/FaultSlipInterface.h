#ifndef FAULTSLIPINTERFACE_H
#define FAULTSLIPINTERFACE_H

class FaultSlipInterface;
#include "Interface/ControlPoint.h"
#include "Interface/InteractiveVector.h"
//#include "Interface/Slider3D.h"
//#include "Karst/KarstPathsGeneration.h"
//#include "Graphics/Mesh.h"
//#include "Interface/FancySlider.h"
#include "Utils/BSpline.h"
#include <QWidget>
#include "TerrainGen/VoxelGrid.h"
//#include "Interface/PathCameraConstraint.h"
//#include <QGLViewer/manipulatedCameraFrame.h>
#include "TerrainModification/FaultSlip.h"

class FaultSlipInterface : public CustomInteractiveObject
{
    Q_OBJECT
public:
    FaultSlipInterface();
    void display();
    void remesh();

    void affectVoxelGrid(std::shared_ptr<VoxelGrid> voxelGrid);
    std::shared_ptr<VoxelGrid> voxelGrid;
//    std::shared_ptr<FaultSlip> faultSlip;
    FaultSlip faultSlip;

//    std::vector<BSpline> karstPaths;

    Mesh planeMesh;

    QHBoxLayout* createGUI();
    QHBoxLayout* faultSlipLayout;

Q_SIGNALS:
    void faultSlipApplied();

public Q_SLOTS:
    void updateSlipVector(Vector3 newSlipVector = Vector3());
    void updatePoints();
    void computeFaultSlip();
    void setSideAffected(bool isRightSide);

    void hide();
    void show();

public:
    ControlPoint *firstSlipControlPoint;
//    ControlPoint *secondSlipControlPoint;
    InteractiveVector *slipVector;
};

#endif // FAULTSLIPINTERFACE_H
