#ifndef VIEWER_H
#define VIEWER_H

#include "Grid.h"
#include "VoxelGrid.h"
#include "LayerBasedGrid.h"
#include <QGLViewer/qglviewer.h>
#include <QKeyEvent>
#include <qmessagebox.h>
#include "RockErosion.h"
#include "Shader.h"

enum MapMode {
    GRID_MODE  = 0b001,
    VOXEL_MODE = 0b010,
    LAYER_MODE = 0b100
};
enum ViewerMode {
    FILL_MODE  = 0b01,
    WIRE_MODE  = 0b10
};
enum SmoothingAlgorithm {
    NONE            = 0b001,
    MARCHING_CUBES  = 0b010,
    DUAL_CONTOURING = 0b100
};

class Viewer : public QGLViewer {
    Q_OBJECT
public:
    Viewer(QWidget *parent = nullptr);
  Viewer(Grid* grid, VoxelGrid* voxelGrid, LayerBasedGrid* layerGrid, MapMode map, ViewerMode mode = FILL_MODE, QWidget *parent = nullptr);
  Viewer(Grid* g, QWidget *parent = nullptr);
  Viewer(VoxelGrid* g, QWidget *parent = nullptr);
  ~Viewer();

  void setViewerMode(ViewerMode newMode) { this->viewerMode = newMode; }
  void setMapMode(MapMode newMode) { this->mapMode = newMode; }
  void setSmoothingAlgorithm(SmoothingAlgorithm newAlgo) { this->algorithm = newAlgo; }

  void saveScreenshot();

protected:
  virtual void init();
  virtual void draw();

//  virtual void drawWithNames();
//  virtual void postSelection(const QPoint &point);

  void mousePressEvent(QMouseEvent* e);
  void keyPressEvent(QKeyEvent *e);

  void mouseMoveEvent(QMouseEvent* e);

  void closeEvent(QCloseEvent* e);

  void animate();

  ViewerMode viewerMode;
  MapMode mapMode;
  SmoothingAlgorithm algorithm = MARCHING_CUBES;

private:
  Grid* grid;
  VoxelGrid* voxelGrid;
  LayerBasedGrid* layerGrid;
  bool display_vertices = true;
  qglviewer::Vec selectedPoint, orig, dir;

  bool displayRockTrajectories = false;

  int selectionSize = 15;
  float*** selectionShape;
  bool addingMatterMode = true;
  RockErosion matter_adder;
  std::vector<std::vector<Vector3>> lastRocksLaunched;
  bool mouseInWorld = false;
  Vector3 mousePosWorld;
  QPoint mousePos;

  Shader shader;
  PositionalLight light;
  GLuint rocksVBO;

  std::string screenshotFolder;
  bool isTakingScreenshots = false;
  int screenshotIndex = 0;

  bool applyLetItFall = false;
  bool applyLetSandFall = false;

  bool checkMouseOnVoxel();

  unsigned int frame_num = 0;

  Mesh rocksMeshes;
};


#endif // VIEWER_H
