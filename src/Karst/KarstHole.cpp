#include "KarstHole.h"

#include "Utils/Utils.h"

KarstHole::KarstHole(float size, KarstHolePredefinedShapes startingShape, KarstHolePredefinedShapes endingShape)
{
    this->startingProfile = KarstHoleProfile(startingShape).setSize(size, size);
    this->endingProfile = KarstHoleProfile(endingShape).setSize(size, size);
    this->path = BSpline();
    this->size = size;
}

KarstHole::KarstHole(Vector3 start, Vector3 end, float size,
                     KarstHolePredefinedShapes startingShape, KarstHolePredefinedShapes endingShape)
    : KarstHole(size, startingShape, endingShape)
{
    this->path = BSpline({start, end});
}

KarstHole::KarstHole(BSpline fullPath, float size,
                     KarstHolePredefinedShapes startingShape, KarstHolePredefinedShapes endingShape)
    : KarstHole(size, startingShape, endingShape)
{
    this->path = fullPath;
}

KarstHoleProfile KarstHole::interpolate(float t, float previousAcceptedTime, float nextAcceptedTime)
{
    /* Linear Interpolation, the simpliest */
    return this->startingProfile.interpolate(this->endingProfile, this->path, t, previousAcceptedTime, nextAcceptedTime); //KarstHoleProfile(interpolated);
}

std::vector<std::vector<Vector3>> KarstHole::computeClosingMesh(std::vector<Vector3>& vertices)
{
    // Compute the closing faces using the "ear clipping" method
    Vector3 middle(0, 0, 0);
    Vector3 ray; //Vector3(-1.1, -1.2, points[i].z);
    Vector3 kindOfTangent;
    std::vector<int> remaining_nodes(vertices.size()); // Starts with {0, 1, 2, 3, ...N}
    std::vector<std::vector<Vector3>> triangles;
    for (size_t i = 0; i < vertices.size(); i++) {
        middle += vertices[i];
        remaining_nodes[i] = i;
    }
    middle /= (float)number_of_points;
    for (int i = 0; i < number_of_points; i++) {
        if ((vertices[i] - middle).norm2() > ray.norm2()) {
            ray = (vertices[i] - middle) * 2.f;
            int second_indice = 1;
            while(kindOfTangent.norm2() == 0 || kindOfTangent.dot(ray) == 0) {
                kindOfTangent = (vertices[(i + second_indice)] - middle);
                second_indice++;
                if ((i + second_indice) % vertices.size() == 0) {
//                            std::cout << "Fuuuuuu..." << std::endl;
                    break;
                }
            }
        }
    }
    ray += kindOfTangent;
    ray += middle;

    int max_tries = vertices.size() * vertices.size();
    while(remaining_nodes.size() > 3) {
        max_tries --;
        if (max_tries < 0) {
            std::cout << "Breaking : too much iterations" << std::endl;
            break;
        }
        int previous = remaining_nodes[0],
                current = remaining_nodes[1],
                next = remaining_nodes[2];
        Vector3 midpoint = (vertices[previous] + vertices[next])/2.f;
        // Check if midpoint is in the (reduced) polygon
        int number_of_intersections = 0;
        bool valid = true;
        for (size_t j = 0; j < vertices.size(); j++) {
            // Count the number of intersection from the midpoint to somewhere outside
            if (intersection(ray, midpoint, vertices[j], vertices[(j + 1) % vertices.size()])) {
                number_of_intersections++;
            }
        }
        for (size_t j = 0; j < remaining_nodes.size() && valid; j++) {
            if (previous != remaining_nodes[j] && previous != remaining_nodes[(j + 1) % remaining_nodes.size()] &&
                    next != remaining_nodes[j] && next != remaining_nodes[(j + 1) % remaining_nodes.size()]) {
                // Also, check if the "previous-next" line intersects any other edge (except at the exact position of points)
                if (intersection(vertices[previous], vertices[next], vertices[remaining_nodes[j]], vertices[remaining_nodes[(j + 1) % remaining_nodes.size()]])) {
                    Vector3 inter = intersectionPoint(vertices[previous], vertices[next], vertices[remaining_nodes[j]], vertices[remaining_nodes[(j + 1) % remaining_nodes.size()]]);
                    if (inter != vertices[previous] && inter != vertices[next]) {
                        valid = false;

                    }
                }
            }
        }
        // If there is an odd number of itersection, point is inside the shape,
        // we can create a triangle and remove the current node
        if (valid && number_of_intersections % 2 == 1) {
            triangles.push_back({vertices[previous], vertices[current], vertices[next]});
            remaining_nodes.erase(remaining_nodes.begin() + 1);
        } else {
            // Otherwise, rotate the list just to change the first 3 values
            std::rotate(remaining_nodes.begin(), remaining_nodes.begin() + 1, remaining_nodes.end());
        }
    }
    // Put the last 3 in a triangle
    if (remaining_nodes.size() == 3)
        triangles.push_back({vertices[remaining_nodes[0]], vertices[remaining_nodes[1]], vertices[remaining_nodes[2]]});
    return triangles;
}

std::vector<std::vector<Vector3> > KarstHole::generateMesh()
{
    this->vertexGroups.clear();
    this->vertexCylinders.clear();
    this->cylinders.clear();

    Vector3 minVec = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vector3 maxVec = Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    number_of_intermediates = 2 * int(this->path.points.size()); // std::min(5, int(this->path.points.size())*2);
    float dt = 1.f / (float)(number_of_intermediates - 1);
    std::vector<float> validTimesInPath;
    for (int i = 0; i < number_of_intermediates; i++) {
        float t = i * dt;
        std::vector<Vector3> intermediateShape = this->interpolate(t).vertices.points;
        for (const Vector3& pos : intermediateShape) {
            minVec.x = std::min(minVec.x, pos.x);
            minVec.y = std::min(minVec.y, pos.y);
            minVec.z = std::min(minVec.z, pos.z);
            maxVec.x = std::max(maxVec.x, pos.x);
            maxVec.y = std::max(maxVec.y, pos.y);
            maxVec.z = std::max(maxVec.z, pos.z);
        }
        // Find non-vertical times in our path
        if (std::abs(this->path.getDirection(t).dot(Vector3(0, 0, 1))) < 0.9) {
            validTimesInPath.push_back(t);
        }
    }

    std::vector<std::vector<Vector3>> triangles;
    std::vector<std::vector<Vector3>> allIntermediateVertices(number_of_intermediates + 2);
    for (int iT = 0; iT < number_of_intermediates + 2; iT++) {
        float t = (iT-1.f) * dt;
        float previousValidTime = -1.f, nextValidTime = -1.f;
        for (const float& time : validTimesInPath) {
            if (time < t)
                previousValidTime = time;
            if (time > t) {
                nextValidTime = time;
                break; // no need to go further
            }
        }
        std::vector<Vector3> points;
        if (iT == 0) {
            points = this->interpolate(0.0).translate(path.getDirection(0.f) * -this->size).scale(.5f).vertices.getPath(1.f / (float)(number_of_points - 1));
            cylinders.push_back(std::make_tuple(path.getPoint(0.f) + path.getDirection(0.f) * -this->size, path.getPoint(0.f)));
        }
        else if(iT == number_of_intermediates + 1) {
            points = this->interpolate(1.0).translate(path.getDirection(1.f) * this->size).scale(.5f).vertices.getPath(1.f / (float)(number_of_points - 1));
            cylinders.push_back(std::make_tuple(path.getPoint(1.f), path.getPoint(1.f) + path.getDirection(1.f) * this->size));
        }
        else {
            points = this->interpolate(t, previousValidTime, nextValidTime).vertices.getPath(1.f / (float)(number_of_points - 1));
            if (iT < number_of_intermediates) // Avoid the last cylinder
                cylinders.push_back(std::make_tuple(path.getPoint(t), path.getPoint(t + dt)));
        }

        allIntermediateVertices[iT] = points;
    }

    for (size_t i = 0; i < allIntermediateVertices.size() - 1; i++) {
        // Compute the triangles that makes the actual tunnel
        for (int j = 0; j < number_of_points; j++) {
            int j_plus_1 = (j + 1) % number_of_points;
            triangles.push_back({
                                    (allIntermediateVertices[i][j]),
                                    (allIntermediateVertices[i][j_plus_1]),
                                    (allIntermediateVertices[i+1][j])
                                });
            triangles.push_back({
                                    (allIntermediateVertices[i+1][j]),
                                    (allIntermediateVertices[i+1][j_plus_1]),
                                    (allIntermediateVertices[i][j_plus_1])
                                });
        }

        std::vector<std::vector<Vector3>> firstClosingShape = this->computeClosingMesh(allIntermediateVertices[i]); // For the first tunnel, compute the front shape
        triangles.insert(triangles.end(), firstClosingShape.begin(), firstClosingShape.end()); // Compute the back closing shape and add the triangles.
        std::vector<std::vector<Vector3>> closingShape = this->computeClosingMesh(allIntermediateVertices[i + 1]);
        triangles.insert(triangles.end(), closingShape.begin(), closingShape.end()); // Compute the back closing shape and add the triangles.
    }
    return triangles;
}

std::tuple<Matrix3<float>, Vector3> KarstHole::generateMask(std::vector<std::vector<Vector3>>  precomputedTriangles)
{
    std::vector<std::vector<Vector3>> triangles = precomputedTriangles;
    if (triangles.empty())
        triangles = this->generateMesh();
    std::vector<Vector3> cylindersStart;
    std::vector<Vector3> cylindersEnd;

    Vector3 minVec = Vector3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vector3 maxVec = Vector3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    for (const auto& triangle : triangles) {
        for (const Vector3& pos : triangle) {
            minVec.x = std::min(minVec.x, pos.x);
            minVec.y = std::min(minVec.y, pos.y);
            minVec.z = std::min(minVec.z, pos.z);
            maxVec.x = std::max(maxVec.x, pos.x);
            maxVec.y = std::max(maxVec.y, pos.y);
            maxVec.z = std::max(maxVec.z, pos.z);
        }
    }
    for (auto& triangle : triangles) {
        for (Vector3& pos : triangle) {
            pos -= minVec;
        }
    }
    for (auto& cylinder : this->cylinders) {
        cylindersStart.push_back(std::get<0>(cylinder) - minVec);
        cylindersEnd.push_back(std::get<1>(cylinder) - minVec);
    }
    Matrix3<float> mask((maxVec - minVec));
    for (int x = 0; x < mask.sizeX; x++) {
        for (int y = 0; y < mask.sizeY; y++) {
            for (int z = 0; z < mask.sizeZ; z++) {
                Vector3 point(x, y, z);
                bool allCollisionsValidated = false;
                int numberOfCollisions = 0;
                int currentTry = 0;
                Vector3 ray;
                while (!allCollisionsValidated) {
                    currentTry++;
                    if (currentTry > 100) {
                        numberOfCollisions = 1; // Set it inside
                        std::cout << "Way too many iterations..." << std::endl;
                        break;
                    }
                    allCollisionsValidated = true;
                    numberOfCollisions = 0;
                    ray = Vector3(x, -200, z); // + Vector3::random() * 180.f; // (Vector3::random() * 2.f * (maxVec - minVec).norm()).translate((minVec - maxVec)/ 2.f);
                    int i = 0;
                    int lastTrianglesCylinder = -1;
//                    bool ignoreThisCylinder = false;
                    for (const std::vector<Vector3>& triangle : triangles) {
                        // Estimate the cylinder associated to this particular triangle.
                        int triangle_group = i / (2 * (number_of_points - 2) + 2 * number_of_points);
                        i++;
                        if (triangle_group != lastTrianglesCylinder) {
                            lastTrianglesCylinder = triangle_group;
                            if (numberOfCollisions % 2 == 1) {
                                break;
                            }/*
                            ignoreThisCylinder = false;
                            float distToCylinder = shortestDistanceBetweenSegments(point, ray, cylindersStart[triangle_group], cylindersEnd[triangle_group]);
                            if (distToCylinder > this->size) {
                                ignoreThisCylinder = true;
                            }
                        }
                        // Ignore this calculation if it's not needed.
                        if (ignoreThisCylinder) {
                            continue;*/
                        }

                        int collision_result = KarstHole::segmentToTriangleCollision(point, ray, triangle[0], triangle[1], triangle[2]);
                        if (collision_result == 1) {
                            numberOfCollisions ++;
                        }
                        else if (collision_result == -1) {
                            allCollisionsValidated = false;
                            break;
                        }
                    }
                }
                if (numberOfCollisions % 2 == 1) {
                    mask.at(x, y, z) = 1.f;
                }
            }
        }
    }
//    mask = mask.toDistanceMap();
//    float maxDistance = mask.max();//this->size/2.f;
//    /*for (float& m : mask) {
//        if (m > maxDistance) m = maxDistance;
//    }*/
////    std::cout << "Min distances : " << mask.min() << " -- max : " << mask.max() << " -- clamped to " << maxDistance << "\n";
////    std::cout << mask.displayValues() << std::endl;
//    for (float& m : mask) {
////        if (m > 0) m = 1.f;
//        m = interpolation::linear(m, 0.f, maxDistance);
////        m = interpolation::quadratic(interpolation::linear(m, 0.f, 5.f)); //(sigmoid(m) - s_0) / (s_1 - s_0);
//    }
    Vector3 anchor = this->path.points[0] - minVec;
    return std::make_tuple(mask, anchor);
}

float tetrahedronSignedVolume(Vector3 a, Vector3 b, Vector3 c, Vector3 d) {
    return (1.f/6.f) * (c-a).cross((b-a)).dot((d-a));
}
int sign(float a){return (a < 0 ? -1 : (a > 0 ? 1 : 0)); }
int KarstHole::segmentToTriangleCollision(Vector3 s1, Vector3 s2, Vector3 t1, Vector3 t2, Vector3 t3)
{
    // Möller–Trumbore intersection algorithm
//    if (int(s1.x) == 10 && int(s1.z) == 10) {
//        int a = 0;
//    }
    Vector3 rayOrigin = s1;
    Vector3 rayDir = (s2 - s1);

    Vector3 triEdge1 = (t2 - t1);
    Vector3 triEdge2 = (t3 - t1);

    Vector3 h = rayDir.cross(triEdge2);
    float dot = triEdge1.dot(h);
    if (std::abs(dot) <  1.e-6)
        return false; // Ray parallel to the triangle
    float f = 1.f/dot;
    Vector3 s = (rayOrigin - t1);
    float u = f * s.dot(h);
    if (u < 0.f || 1.f < u)
        return false; // Ray did not reach the triangle
    Vector3 q = s.cross(triEdge1);
    float v = f * rayDir.dot(q);
    if (v < 0.f || 1.f < (u + v))
        return false;

    float t = f * triEdge2.dot(q);
    return t > 0;
    /*
    Vector3 ray = (s1 - s2); //.normalize();
    Vector3 segment1 = (s1 - t1); //.normalize();
    Vector3 segment2 = (s1 - t2); //.normalize();
    Vector3 segment3 = (s1 - t3); //.normalize();
    if (ray.dot(segment1) > segment1.norm() * ray.norm() * 0.9999 ||
            ray.dot(segment2) > segment2.norm() * ray.norm() * 0.9999 ||
            ray.dot(segment3) > segment3.norm() * ray.norm() * 0.9999) {
//        std::cout << ray << "." << segment1 << " = " << ray.dot(segment1) << " " << ray << "." << segment2 << " = " << ray.dot(segment2) << " " << ray << "." << segment3 << " = " << ray.dot(segment3) << std::endl;
        return 1;
    }
    // Compute tetraheadron "signed" volume from one end of the segment and the triangle
    float product_of_volumes = tetrahedronSignedVolume(s1, t1, t2, t3) * tetrahedronSignedVolume(s2, t1, t2, t3);
    if (std::abs(product_of_volumes) < 0.000001 || sign(product_of_volumes) == 0)
        return -1;
    if (sign(product_of_volumes) > 0) // Same sign for the two volumes computed
        return 0;
    // Compute the tetrahedron built with the segment
    // and 2 points of the triangle, all configs should have
    // the same sign.
    int volume1 = tetrahedronSignedVolume(s1, s2, t1, t2);
    int volume2 = tetrahedronSignedVolume(s1, s2, t3, t1);
    int volume3 = tetrahedronSignedVolume(s1, s2, t2, t3);
    if (std::abs(volume1) < 0.000001 || std::abs(volume2) < 0.000001 || std::abs(volume3) < 0.000001)
        return -1;
    if (sign(volume1) == sign(volume2) && sign(volume1) == sign(volume3))
        return 1;
    return 0;
    */
}
