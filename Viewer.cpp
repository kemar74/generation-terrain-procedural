#include "Viewer.h"

#include <QGLViewer/manipulatedCameraFrame.h>

using namespace qglviewer;
using namespace std;

Viewer::Viewer(Grid* grid, VoxelGrid* voxelGrid, int mode)
    : QGLViewer(), viewerMode(mode), grid(grid), voxelGrid(voxelGrid) {

}
Viewer::Viewer(Grid* g)
    : Viewer(g, NULL, GRID_MODE) {

}
Viewer::Viewer(VoxelGrid* g)
    : Viewer(NULL, g, VOXEL_MODE) {

}

void Viewer::init() {
    restoreStateFromFile();
    setSceneRadius(200.0);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    this->camera()->setType(Camera::ORTHOGRAPHIC);



}

void Viewer::draw() {
    if (this->viewerMode & ViewerMode::WIRE_MODE)
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

    glPushMatrix();
    glRotatef(180.0, 1.0, 0.0, 0.0);
    drawAxis();
    glPopMatrix();
//    drawLight(GL_LIGHT1);

    if (this->viewerMode & GRID_MODE) {
        this->grid->display(true);
    }
    else if (this-> viewerMode & VOXEL_MODE) {
        this->voxelGrid->display();
    }
}
