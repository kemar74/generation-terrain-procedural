#ifndef VIEWER_H
#define VIEWER_H

#include "Grid.h"
#include "VoxelGrid.h"
#include <QGLViewer/qglviewer.h>
#include <QKeyEvent>
#include <qmessagebox.h>

enum MapMode {
    GRID_MODE  = 0b01,
    VOXEL_MODE = 0b10
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
public:
    Viewer(Grid* grid, VoxelGrid* voxelGrid, MapMode map, ViewerMode mode = FILL_MODE);
  Viewer(Grid* g);
  Viewer(VoxelGrid* g);

  void setViewerMode(ViewerMode newMode) { this->viewerMode = newMode; }
  void setMapMode(MapMode newMode) { this->mapMode = newMode; }
  void setSmoothingAlgorithm(SmoothingAlgorithm newAlgo) { this->algorithm = newAlgo; }

protected:
  virtual void init();
  virtual void draw();

  virtual void drawWithNames();
  virtual void postSelection(const QPoint &point);

  void keyPressEvent(QKeyEvent *e);

  ViewerMode viewerMode;
  MapMode mapMode;
  SmoothingAlgorithm algorithm = MARCHING_CUBES;

private:
  Grid* grid;
  VoxelGrid* voxelGrid;
  bool display_vertices = true;
  qglviewer::Vec selectedPoint, orig, dir;

  float selectionShape[5][5] = {
      {.0, .0, .0, .0, .0},
      {.0, .3, .5, .3, .0},
      {.0, .5, 1., .5, .0},
      {.0, .3, .5, .3, .0},
      {.0, .0, .0, .0, .0}
  };
};


#endif // VIEWER_H
