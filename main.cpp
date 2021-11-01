#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qapplication.h>
#include <iostream>

#include "Viewer.h"

#include "Vector3.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif
    QGuiApplication app(argc, argv);
    QApplication a(argc, argv);

    Grid* grid = new Grid(100, 100, 100, 1.0);
//    VoxelGrid* vGrid = new VoxelGrid(40, 40, 20, 1.0);
    VoxelGrid* vGrid = new VoxelGrid(*grid);
//    grid->fromVoxelGrid(*vGrid);
    Viewer view(grid, vGrid, MapMode::VOXEL_MODE, ViewerMode::FILL_MODE);

    view.show();

    return app.exec();
}
