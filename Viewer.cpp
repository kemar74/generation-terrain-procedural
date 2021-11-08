#include "Viewer.h"

#include <QGLViewer/manipulatedCameraFrame.h>
#include <chrono>
#include "UnderwaterErosion.h"

using namespace qglviewer;
using namespace std;

Viewer::Viewer(Grid* grid, VoxelGrid* voxelGrid, MapMode map, ViewerMode mode)
    : QGLViewer(), mapMode(map), viewerMode(mode), grid(grid), voxelGrid(voxelGrid) {

}
Viewer::Viewer(Grid* g)
    : Viewer(g, NULL, GRID_MODE, FILL_MODE) {

}
Viewer::Viewer(VoxelGrid* g)
    : Viewer(NULL, g, VOXEL_MODE, FILL_MODE) {

}

void Viewer::init() {
    restoreStateFromFile();
    setSceneRadius(200.0);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
//    this->camera()->setType(Camera::ORTHOGRAPHIC);

    this->matter_adder = RockErosion(this->selectionSize, 1.0);
}

void Viewer::draw() {
    if (this->viewerMode == ViewerMode::WIRE_MODE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Place light at camera position
    const Vec cameraPos = camera()->position();
    const GLfloat pos[4] = {(float)cameraPos[0], (float)cameraPos[1],
                            (float)cameraPos[2], 1.0f};
//    const GLfloat pos[4] = {0, -5.0, -5.0, 1.0};
    glLightfv(GL_LIGHT1, GL_POSITION, pos);

    // Orientate light along view direction
//    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, camera()->viewDirection());
    const GLfloat orient[4] = {0.1, 0.1, 1.0};
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, orient);


    // Light default parameters
    const GLfloat light_ambient[4] = {1.0, .9, .9, 1.0};
    const GLfloat light_specular[4] = {1.0, 1.0, 1.0, 1.0};
    const GLfloat light_diffuse[4] = {1.0, 1.0, 1.0, 100.0};

    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 30.0);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 100.0);
//    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, .1f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, .00003f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0003f);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
//    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
//    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

//    glPushMatrix();
//    glRotatef(180.0, 1.0, 0.0, 0.0);
    drawAxis();
//    glPopMatrix();
//    drawLight(GL_LIGHT1);

//    glPushMatrix();
//    glTranslatef(-this->voxelGrid->getSizeX()/2.0, -this->voxelGrid->getSizeY()/2.0, -this->voxelGrid->getSizeZ()/2.0);
//    for(std::vector<Vector3> coords : this->lastRocksLaunched) {
//        glBegin(GL_LINE_STRIP);
//        glColor3f(1.0, 1.0, 1.0);
//        for(Vector3 pos : coords) {
//            glVertex3f(pos.x, pos.y, pos.z);
//        }
//        glEnd();
//    }
//    glPopMatrix();

    if (this->mapMode == GRID_MODE) {
        this->grid->display(true);
    }
    else if (this->mapMode == VOXEL_MODE) {
        this->voxelGrid->display((this->algorithm & MARCHING_CUBES), this->display_vertices);
    }
}


void Viewer::drawWithNames()
{
    this->draw();
}
void Viewer::postSelection(const QPoint &point)
{
    auto full_start = std::chrono::system_clock::now();
    camera()->convertClickToLine(point, orig, dir);

    // Find the selectedPoint coordinates, using camera()->pointUnderPixel().
    bool found;
    selectedPoint = camera()->pointUnderPixel(point, found);
    selectedPoint -= 0.01f * dir; // Small offset to make point clearly visible.
    // Note that "found" is different from (selectedObjectId()>=0) because of the
    // size of the select region.

    if (selectedName() == -1) {
        std::cout << "Nope." << std::endl;
    }
    else {
        intptr_t ptr_int = selectedName();
        if (ptr_int > 0) {
            Voxel* main_v = reinterpret_cast<Voxel*>(ptr_int);
            this->matter_adder.Apply(main_v, addingMatterMode);

        }
    }
    std::cout << "Total duration : " << (std::chrono::duration<double>(std::chrono::system_clock::now() - full_start)).count() << std::endl;
}


void Viewer::keyPressEvent(QKeyEvent *e)
{
    // Defines the Alt+R shortcut.
    if (e->key() == Qt::Key_Z)
    {
        setViewerMode(ViewerMode::WIRE_MODE);
        update(); // Refresh display
    } else if (e->key() == Qt::Key_S)
    {
        setViewerMode(ViewerMode::FILL_MODE);
        update(); // Refresh display
    } else if (e->key() == Qt::Key_Q)
    {
        setMapMode(MapMode::VOXEL_MODE);
        update(); // Refresh display
    } else if (e->key() == Qt::Key_D)
    {
        setMapMode(MapMode::GRID_MODE);
        update(); // Refresh display
    } else if (e->key() == Qt::Key_R) {
        if (this->algorithm == NONE)
            setSmoothingAlgorithm(MARCHING_CUBES);
        else if (this->algorithm == MARCHING_CUBES)
            setSmoothingAlgorithm(NONE);
//        else if (this->algorithm == DUAL_CONTOURING)
//            setSmoothingAlgorithm(NONE);
        update();
    } else if(e->key() == Qt::Key_V) {
        this->display_vertices = !this->display_vertices;
        update();
    } else if(e->key() == Qt::Key_P) {
        this->addingMatterMode = !this->addingMatterMode;
        update();
    } else if(e->key() == Qt::Key_Return) {
        UnderwaterErosion erod(this->voxelGrid, 10, 0.3, 1000);
        this->lastRocksLaunched = erod.Apply();
        update();
    } else {
        QGLViewer::keyPressEvent(e);
    }
}
