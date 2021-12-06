#ifndef BSPLINE_H
#define BSPLINE_H

#include <vector>
#include "Vector3.h"

class BSpline
{
public:
    BSpline();
    BSpline(int numberOfPoints);
    BSpline(std::vector<Vector3> points);

    Vector3 getPoint(float x);
    Vector3 getPoint(float x, Vector3 a, Vector3 b);

    std::vector<Vector3> points;

};

#endif // BSPLINE_H
