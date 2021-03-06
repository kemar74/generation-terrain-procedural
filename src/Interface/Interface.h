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
#include "Interface/GravityInterface.h"
#include "Interface/UndoRedoInterface.h"
#include "Interface/TerrainGenerationInterface.h"
#include "Interface/ErosionInterface.h"
#include "Interface/HeightmapErosionInterface.h"
#include "Interface/BiomeInterface.h"
#include "Interface/StickyFrame.h"

class ViewerInterface : public QMainWindow{
public:
    ViewerInterface();
    virtual ~ViewerInterface();

    void setupUi();

    void setupBindings();

    void retranslateUi();

    void setAllValuesToFitViewerDefaults(Viewer* viewer);

    void closeEvent(QCloseEvent* e);

    void resizeEvent(QResizeEvent* e);

    void focusOutEvent(QFocusEvent* e);

public Q_SLOTS:
    void openInterface(std::string interfaceName);

    void openFaultSlipInterface();
    void openFlowfieldInterface();
    void openKarstInterface();
    void openKarstPeytavieInterface();
    void openTunnelInterface();
    void openManualEditionInterface();
    void openGravityInterface();
    void openErosionInterface();
    void openHeightmapErosionInterface();
    void openBiomeInterface();

    void hideAllInteractiveParts();

    void openMapUI();
    void saveMapUI();

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
    std::shared_ptr<GravityInterface> gravityInterface;
    std::shared_ptr<UndoRedoInterface> undoRedoInterface;
    std::shared_ptr<TerrainGenerationInterface> terrainGenerationInterface;
    std::shared_ptr<ErosionInterface> erosionInterface;
    std::shared_ptr<HeightmapErosionInterface> heightmapErosionInterface;
    std::shared_ptr<BiomeInterface> biomeInterface;

    std::map<std::string, std::shared_ptr<ActionInterface>> actionInterfaces;

    QMenuBar* toolbox;

    QHBoxLayout* displayModeLayout;
    Spoiler* displayModeBox;
    QHBoxLayout* LoDChooserLayout;
    Spoiler* LoDChooserBox;
    RangeSlider* mapSliceSliderX;
    RangeSlider* mapSliceSliderY;
    RangeSlider* mapSliceSliderZ;
    RangeSlider* isolevelSelectionSlider;

    std::string mapSavingFolder = "../saved_maps/";
    std::shared_ptr<std::vector<nlohmann::json>> actionsOnMap;
};

#endif // INTERFACE_H
