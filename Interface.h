#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include <QSlider>
#include <QGridLayout>
#include "Viewer.h"

class FancySlider : public QSlider{
    Q_OBJECT
public:
    explicit FancySlider(float multiplier = 1.0, QWidget *parent = nullptr);
    explicit FancySlider(Qt::Orientation orientation, float min = 0.0, float max = 100.0, float multiplier = 1.0, QWidget *parent = nullptr);
    virtual ~FancySlider();

    void setfValue(float val);
    void setfRange(float min, float max);

Q_SIGNALS:
    void floatValueChanged(float value);

public Q_SLOTS:
    void notifyValueChanged(int value);
protected:
    virtual void sliderChange(SliderChange change);
    float multiplier;
};

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
    QGroupBox* loadSaveBox;
    QHBoxLayout* randomRocksLayout;
    QGroupBox* randomRocksBox;
    QHBoxLayout* currentSimulationLayout;
    QGroupBox* currentSimulationBox;
    QHBoxLayout* manualRocksLayout;
    QGroupBox* manualRocksBox;
    QHBoxLayout* curvesErosionLayout;
    QGroupBox* curvesErosionBox;
    QHBoxLayout* gravityLayout;
    QGroupBox* gravityBox;
    QHBoxLayout* recordingLayout;
    QGroupBox* recordingBox;
    QHBoxLayout* displayModeLayout;
    QGroupBox* displayModeBox;
    QHBoxLayout* algorithmLayout;
    QGroupBox* algorithmBox;
    QHBoxLayout* displayTypeLayout;
    QGroupBox* displayTypeBox;
    QHBoxLayout* LoDChooserLayout;
    QGroupBox* LoDChooserBox;
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
    QPushButton* gravityGlobalButton;
    QPushButton* gravitySandButton;
    QPushButton* startStopRecording;
    QRadioButton* wireModeButton;
    QRadioButton* fillModeButton;
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
