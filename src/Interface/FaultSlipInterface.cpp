#include "FaultSlipInterface.h"

#include "Utils/Utils.h"


FaultSlipInterface::FaultSlipInterface()
{

}

void FaultSlipInterface::display()
{
    if (voxelGrid != nullptr) {
        this->firstSlipControlPoint->display();
//        this->secondSlipControlPoint->display();
        this->slipVector->display();
        if (this->planeMesh.shader != nullptr)
            this->planeMesh.shader->setVector("color", std::vector<float>({.1f, .5f, 1.f, .5f}));
        this->planeMesh.display();
    }
}

void FaultSlipInterface::remesh()
{
    Vector3 minAABB = Vector3(0, 0, 0);
    Vector3 maxAABB = Vector3(voxelGrid->sizeX, voxelGrid->sizeY, voxelGrid->sizeZ);

    Vector3 p1 = this->firstSlipControlPoint->position;
    Vector3 p2 = this->slipVector->getStartingVector();
    Vector3 p3 = p1 + this->slipVector->getResultingVector();
    /*
    Vector3 x1 = intersectionPointRayPlane(p1, p1 - p2, minAABB, Vector3(1, 0, 0));
    Vector3 x2 = intersectionPointRayPlane(p1, p1 - p2, maxAABB, Vector3(1, 0, 0));
    Vector3 y1 = intersectionPointRayPlane(p1, p1 - p2, minAABB, Vector3(0, 1, 0));
    Vector3 y2 = intersectionPointRayPlane(p1, p1 - p2, maxAABB, Vector3(0, 1, 0));
    Vector3 z1 = intersectionPointRayPlane(p1, p1 - p2, minAABB, Vector3(0, 0, 1));
    Vector3 z2 = intersectionPointRayPlane(p1, p1 - p2, maxAABB, Vector3(0, 0, 1));*/

    Vector3 largeur  = (p2 - p1).normalize() * maxAABB;
    Vector3 hauteur = (p3 - p1).normalize() * maxAABB;

    this->planeMesh.fromArray({
                                  p1 - largeur - hauteur, p2 + largeur - hauteur, p1 - largeur + hauteur,
                                  p1 - largeur + hauteur, p2 + largeur - hauteur, p2 + largeur + hauteur
                              });
    this->planeMesh.update();
}

void FaultSlipInterface::affectVoxelGrid(std::shared_ptr<VoxelGrid> voxelGrid)
{
    this->firstSlipControlPoint = new ControlPoint(Vector3(voxelGrid->sizeX / 2.f, 0, voxelGrid->sizeZ), 5.f);
//    this->secondSlipControlPoint = new ControlPoint(Vector3(voxelGrid->sizeX / 2.f, 0, voxelGrid->sizeZ), 5.f);
    this->slipVector = new InteractiveVector(Vector3(0, voxelGrid->sizeY / 2.f, voxelGrid->sizeZ), Vector3(0, voxelGrid->sizeY / 2.f, 0));
    this->voxelGrid = voxelGrid;
    this->faultSlip = FaultSlip();

    QObject::connect(this->slipVector, &InteractiveVector::modified, this, &FaultSlipInterface::updateSlipVector);
    QObject::connect(this->firstSlipControlPoint, &ControlPoint::modified, this, &FaultSlipInterface::updatePoints);
//    QObject::connect(this->secondSlipControlPoint, &ControlPoint::modified, this, &FaultSlipInterface::updatePoints);
}
void FaultSlipInterface::updateSlipVector(Vector3 newSlipVector)
{
    this->remesh();
}

void FaultSlipInterface::updatePoints()
{
    this->remesh();
}

void FaultSlipInterface::computeFaultSlip()
{
    this->faultSlip.slippingDirection = this->slipVector->getResultingVector();
    this->faultSlip.slippingDistance = 1.f;
    this->faultSlip.firstPointFault = this->firstSlipControlPoint->position;
    this->faultSlip.secondPointFault = this->slipVector->getStartingVector();
//    this->faultSlip.secondPointFault = this->secondSlipControlPoint->position;

    this->faultSlip.Apply(this->voxelGrid, true);
}

void FaultSlipInterface::setSideAffected(bool isRightSide)
{
    this->faultSlip.positiveSideFalling = isRightSide;
}

void FaultSlipInterface::hide()
{
    this->firstSlipControlPoint->hide();
//    this->secondSlipControlPoint->hide();
    this->slipVector->hide();
    this->planeMesh.hide();
    CustomInteractiveObject::hide();
}

void FaultSlipInterface::show()
{
    this->firstSlipControlPoint->show();
//    this->secondSlipControlPoint->show();
    this->slipVector->show();
    this->planeMesh.show();
    CustomInteractiveObject::show();
}


QHBoxLayout *FaultSlipInterface::createGUI()
{
    this->faultSlipLayout = new QHBoxLayout;

    QPushButton* faultApplyButton = new QPushButton("Chuter");
    QCheckBox* faultSideApplied = new QCheckBox("Partie de droite chute");
    QCheckBox* faultDisplayButton = new QCheckBox("Afficher");
    faultSlipLayout->addWidget(faultApplyButton);
    faultSlipLayout->addWidget(faultSideApplied);
    faultSlipLayout->addWidget(faultDisplayButton);

    QObject::connect(faultApplyButton, &QPushButton::pressed, this, [=](){ this->computeFaultSlip(); } );
    QObject::connect(faultSideApplied, &QCheckBox::toggled, this, &FaultSlipInterface::setSideAffected);
    QObject::connect(faultDisplayButton, &QCheckBox::toggled, this, &FaultSlipInterface::setVisibility);

    faultSideApplied->setChecked(this->faultSlip.positiveSideFalling);

    return this->faultSlipLayout;
}
