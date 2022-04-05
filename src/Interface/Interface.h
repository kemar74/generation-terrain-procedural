#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QSlider>
#include <QGridLayout>
#include "Interface/Viewer.h"
#include "Interface/RangeSlider.h"
#include "Interface/FancySlider.h"
#include "Interface/Spoiler.h"

#include "Interface/KarstPathGenerationInterface.h"
#include "Interface/SpaceColonizationInterface.h"
#include "Interface/FaultSlipInterface.h"
#include "Interface/FlowFieldInterface.h"
#include "Interface/TunnelInterface.h"
#include "Interface/ManualEditionInterface.h"
#include "Interface/StickyFrame.h"

class ViewerInterface : public QMainWindow{
public:
    ViewerInterface();
    ~ViewerInterface();

    void setupUi();

    void setupBindings();

    void retranslateUi();

    void setAllValuesToFitViewerDefaults(Viewer* viewer);

    void closeEvent(QCloseEvent* e);

    void resizeEvent(QResizeEvent* e);

public Q_SLOTS:
    void openFaultSlipInterface();
    void openFlowfieldInterface();
    void openKarstInterface();
    void openTunnelInterface();
    void openManualEditionInterface();

public:
    StickyFrame* frame;
    std::string lastPanelOpenedByStickyFrame;

    QGridLayout* mainLayout;
    QVBoxLayout* controlLayout;
    Viewer* viewer;

    std::shared_ptr<KarstPathGenerationInterface> karstPathGeneration;
    std::shared_ptr<SpaceColonizationInterface> spaceColonization;
    std::shared_ptr<FaultSlipInterface> faultSlip;
    std::shared_ptr<FlowFieldInterface> flowField;
    std::shared_ptr<TunnelInterface> tunnelInterface;
    std::shared_ptr<ManualEditionInterface> manualEditionInterface;

    QMenuBar* toolbox;

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
    QHBoxLayout* faultSlipLayout;
    Spoiler* faultSlipBox;
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
    QPushButton* curvesClearControlPointButton;
    FancySlider* curvesErosionSizeSlider;
    FancySlider* curvesErosionStrengthSlider;
    QPushButton* curvesErosionCreateMatter;
    QPushButton* curvesErosionRemoveMatter;
    QPushButton* curvesErosionCreateCrack;
    QCheckBox* displayCurvesErosion;

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
