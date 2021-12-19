#ifndef VECTOR3_H
#define VECTOR3_H

#include "Globals.h"
#include <iostream>
#include <vector>

class Vector3 {
public:
    float x, y, z;
    Vector3();
    Vector3(float x, float y, float z = 0.f);
    Vector3(const Vector3& copy);
    Vector3(Vector3* copy);

    static std::vector<float> toArray(Vector3 v);
    static std::vector<float> toArray(std::vector<Vector3> vs);
    std::tuple<int, int, int> toIntTuple() {return std::make_tuple<int, int, int>(int(this->x), int(this->y), int(this->z)); }


    friend std::ostream& operator<<(std::ostream& io, const Vector3& v);
    friend std::ostream& operator<<(std::ostream& io, std::shared_ptr<Vector3> v);

    float dot(Vector3 o);
    Vector3 cross(Vector3 o);
    Vector3 rounded(int precision = 0);

    float norm();
    float norm2();
    Vector3& normalize();
    Vector3 normalized();

    float divergence() { return x + y + z; }
/*
    static Matrix3<Vector3> gradient(Matrix3<float> field);
    static Matrix3<Vector3> grad(Matrix3<float> field);
    static Matrix3<float> divergence(Matrix3<Vector3> field);
    static Matrix3<float> div(Matrix3<Vector3> field);
    static Matrix3<Vector3> curl(Matrix3<Vector3> field);
    static Matrix3<Vector3> rot(Matrix3<Vector3> field);
    static Matrix3<float> laplacian(Matrix3<float> field);
    static Matrix3<Vector3> laplacian(Matrix3<Vector3> field);
*/

    static Vector3 random();

    static Vector3 nabla;

    operator float*() const { return new float[3]{this->x, this->y, this->z}; }
//    friend Vector3 operator+(Vector3 a, Vector3& b);
    friend Vector3 operator+(Vector3 a, Vector3 b);
    Vector3& operator+=(const Vector3& o);
    friend Vector3 operator-(Vector3 a, const Vector3& b);
    Vector3& operator-=(const Vector3& o);
    friend Vector3 operator*(Vector3 a, Vector3 o);
    Vector3& operator*=(Vector3& o);
    Vector3 operator/(Vector3& o);
    Vector3& operator/=(Vector3& o);
    Vector3 operator*(float o);
    Vector3& operator*=(float o);
    Vector3 operator/(float o);
    Vector3& operator/=(float o);
    Vector3 operator+(float o);
    Vector3& operator+=(float o);
    Vector3 operator-(float o);
    Vector3& operator-=(float o);
    Vector3& operator=(const Vector3& o);
    bool operator==(Vector3 o);

    std::string toString() const {return "Vector3 (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")"; }
//    const char* toHashString() const {return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z); }

};

/*class Vector3Hash
{

};*/

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
namespace std {
  template <> struct hash<Vector3>
  {
    size_t operator()(const Vector3 & x) const
    {
        size_t seed = 0;
        hash_combine(seed, int(x.x * 100));
        hash_combine(seed, int(x.y * 100));
        hash_combine(seed, int(x.z * 100));
        return seed;
    }
  };
}

#endif // VECTOR3_H
