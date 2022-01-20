#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QSlider>
#include <QGridLayout>
#include "Interface/Viewer.h"
#include "Interface/RangeSlider.h"
#include "Interface/FancySlider.h"
#include "Interface/Spoiler.h"

class ViewerInterface : public QDialog{
public:
    ViewerInterface();

    void setupUi(QDialog *Dialog);

    void setupBindings(QDialog* Dialog);

    void retranslateUi(QDialog *Dialog);

    void setAllValuesToFitViewerDefaults(Viewer* viewer);

    void closeEvent(QCloseEvent* e);

    QGridLayout* mainLayout;
    QVBoxLayout* controlLayout;
    Viewer* viewer;


    QHBoxLayout* loadSaveLayout;
    Spoiler* loadSaveBox;
    QHBoxLayout* randomRocksLayout;
    Spoiler* randomRocksBox;
    QHBoxLayout* currentSimulationLayout;
    Spoiler* currentSimulationBox;
    QHBoxLayout* manualRocksLayout;
    Spoiler* manualRocksBox;
    QHBoxLayout* curvesErosionLayout;
    Spoiler* curvesErosionBox;
    QHBoxLayout* karstCreationLayout;
    Spoiler* karstCreationBox;
    QHBoxLayout* spaceColonizerLayout;
    Spoiler* spaceColonizerBox;
    QHBoxLayout* gravityLayout;
    Spoiler* gravityBox;
    QHBoxLayout* recordingLayout;
    Spoiler* recordingBox;
    QHBoxLayout* displayModeLayout;
    Spoiler* displayModeBox;
    QHBoxLayout* algorithmLayout;
    Spoiler* algorithmBox;
    QHBoxLayout* displayTypeLayout;
    Spoiler* displayTypeBox;
    QHBoxLayout* LoDChooserLayout;
    Spoiler* LoDChooserBox;
    QPushButton* loadButton;
    QPushButton* saveButton;
    FancySlider* randomRocksSizeSlider;
    FancySlider* randomRocksStrengthSlider;
    FancySlider* randomRocksQuantitySlider;
    QPushButton* sendRandomRocksFromCam;
    QPushButton* sendRandomRocks;
    QCheckBox* displayRandomRocks;
    QCheckBox* displayFailedRandomRocks;
    FancySlider* currentSimulationFlowfieldStrengthSlider;
    FancySlider* currentSimulationStrengthSlider;
    FancySlider* currentSimulationRandomDirectionSlider;
    QCheckBox* displayFlowfield;
    QPushButton* recomputeFlowfieldButton;
    FancySlider* manualRocksSizeSlider;
    FancySlider* manualRocksStrengthSlider;
    QRadioButton* addingMode;
    QRadioButton* suppressMode;
    QPushButton* curvesAddControlPointButton;
    FancySlider* curvesErosionSizeSlider;
    FancySlider* curvesErosionStrengthSlider;
    QPushButton* curvesErosionCreateMatter;
    QPushButton* curvesErosionRemoveMatter;
    QCheckBox* displayCurvesErosion;
    QPushButton* karstCreationPreviewButton;
    QCheckBox* karstCreationDisplay;
    QPushButton* karstCreationConfirmButton;
    FancySlider* karstCreationDistanceWeights;
    FancySlider* karstCreationFractureWeights;
    FancySlider* karstCreationWaterWeights;
    FancySlider* karstCreationPorosityWeights;
    FancySlider* karstCreationGamma;
    FancySlider* karstCreationTortuosity;
    QPushButton *spaceColonizerPreviewButton;
    QPushButton *spaceColonizerConfirmButton;
    QCheckBox *spaceColonizerDisplay;
    FancySlider *spaceColonizerSegmentSize;
    FancySlider *spaceColonizerRandomness;
    QPushButton* gravityGlobalButton;
    QPushButton* gravitySandButton;
    QPushButton* startStopRecording;
    QRadioButton* wireModeButton;
    QRadioButton* fillModeButton;
    QRadioButton* invisibleModeButton;
    RangeSlider* mapSliceSliderX;
    RangeSlider* mapSliceSliderY;
    RangeSlider* mapSliceSliderZ;
    QRadioButton* noAlgorithmButton;
    QRadioButton* marchingCubesButton;
    QRadioButton* gridModeButton;
    QRadioButton* voxelsModeButton;
    QRadioButton* layerModeButton;
    FancySlider* LoDChooserSlider;
    QPushButton* LoDChooserConfirmButton;

    std::map<QWidget*, QWidget*> widgetsMapping;
};

#endif // INTERFACE_H
