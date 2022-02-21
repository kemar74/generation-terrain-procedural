#include "DataStructure/Vector3.h"
#include <math.h>
#include "Utils/Globals.h"

Vector3 Vector3::nabla = Vector3(1.f, 1.f, 1.f).normalize();

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {

}
Vector3::Vector3() : Vector3(0.f, 0.f, 0.f) {

}
Vector3::Vector3(const Vector3& copy) : Vector3(copy.x, copy.y, copy.z) {

}
Vector3::Vector3(Vector3* copy) : Vector3(copy->x, copy->y, copy->z) {

}

Vector3::Vector3(qglviewer::Vec other)
    : Vector3(other.x, other.y, other.z)
{

}

float Vector3::norm() {
    if(this->x == 0 && this->y == 0 && this->z == 0) return 0;
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}
float Vector3::norm2() {
    if(this->x == 0 && this->y == 0 && this->z == 0) return 0;
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

Vector3& Vector3::normalize() {
    if(this->norm() == 0)
        return *this;
    float norm = this->norm();
    *this /= norm;
    return *this;
}
Vector3 Vector3::normalized() const {
    Vector3 a = *this;
    return a.normalize();
}

Vector3 Vector3::abs()
{
    Vector3 a = *this;
    a.x = std::abs(x);
    a.y = std::abs(y);
    a.z = std::abs(z);
    return a;
}

Matrix Vector3::toMatrix()
{
    return Matrix(3, 1, (float*)(*this));
}

float Vector3::dot(Vector3 o) {
    return (this->x * o.x) + (this->y * o.y) + (this->z * o.z);
}
Vector3 Vector3::cross(Vector3 o) {
    Vector3 v(this->y * o.z - this->z * o.y,
              this->z * o.x - this->x * o.z,
              this->x * o.y - this->y * o.x);
    return v;
}
Vector3 Vector3::rounded(int precision)
{
    Vector3 v = *this;
    v.x = (int)(v.x * pow(10, precision)) / (float)(pow(10, precision));
    v.y = (int)(v.y * pow(10, precision)) / (float)(pow(10, precision));
    v.z = (int)(v.z * pow(10, precision)) / (float)(pow(10, precision));
    return v;
}

Vector3 Vector3::random() {
    Vector3 v(random_gen::generate(-1.0, 1.0), random_gen::generate(-1.0, 1.0), random_gen::generate(-1.0, 1.0));
    v.normalize();
    return v;
}

std::vector<float> Vector3::toArray(Vector3 v)
{
    std::vector<float> arr;
    arr.insert(arr.end(), {v.x, v.y, v.z});
    return arr;
}
std::vector<float> Vector3::toArray(std::vector<Vector3> vs)
{
    std::vector<float> arr;
    for (Vector3& v : vs)
        arr.insert(arr.end(), {v.x, v.y, v.z});
    return arr;
}


Vector3& Vector3::rotate(float angle_x, float angle_y, float angle_z) {
    return this->rotate(Vector3(angle_x, angle_y, angle_z));
}
Vector3& Vector3::rotate(Vector3 eulerAngles) {
    Matrix Rx (3, 3, std::vector<float>({
                   1, 0, 0,
                   0, cos(eulerAngles.x), -sin(eulerAngles.x),
                   0, sin(eulerAngles.x), cos(eulerAngles.x)
               }).data());
    Matrix Ry (3, 3, std::vector<float>({
                    cos(eulerAngles.y), 0, -sin(eulerAngles.y),
                    0, 1, 0,
                    sin(eulerAngles.y), 0, cos(eulerAngles.y)
                }).data());
    Matrix Rz (3, 3, std::vector<float>({
                    cos(eulerAngles.z), -sin(eulerAngles.z), 0,
                    sin(eulerAngles.z), cos(eulerAngles.z), 0,
                    0, 0, 1
                }).data());
    std::cout << "Rx " << Rx << "\nRy " << Ry << "\nRz " << Rz << std::endl;
    Matrix R = Rx.product(Ry).product(Rz);
    std::cout << "R " << R << std::endl;
    Matrix newCoords = R.product(this->toMatrix());
    std::cout << "New coords : " << newCoords << std::endl;
    this->x = newCoords[0][0];
    this->y = newCoords[1][0];
    this->z = newCoords[2][0];
    return *this;
}
Vector3 Vector3::rotated(float angle_x, float angle_y, float angle_z) {
    return this->rotated(Vector3(angle_x, angle_y, angle_z));
}
Vector3 Vector3::rotated(Vector3 eulerAngles) {
    Vector3 v = *this;
    return v.rotate(eulerAngles);
}
Vector3& Vector3::translate(float move_x, float move_y, float move_z) {
    return this->translate(Vector3(move_x, move_y, move_z));
}
Vector3& Vector3::translate(Vector3 move) {
    return (*this += move);
}
Vector3 Vector3::translated(float move_x, float move_y, float move_z) {
    return this->translated(Vector3(move_x, move_y, move_z));
}
Vector3 Vector3::translated(Vector3 move) {
    Vector3 v = *this;
    return v.translate(move);
}






//Vector3 operator+(Vector3 a, Vector3&b) {
//    a += b;
//    return a;
//}
Vector3 operator+(Vector3 a, Vector3 b) {
    a += b;
    return a;
}
Vector3& Vector3::operator+=(const Vector3& o) {
    this->x += o.x;
    this->y += o.y;
    this->z += o.z;
    return *this;
}
Vector3 operator-(Vector3 a, const Vector3& b) {
    a -= b;
    return a;
}
Vector3& Vector3::operator-=(const Vector3& o) {
    this->x -= o.x;
    this->y -= o.y;
    this->z -= o.z;
    return *this;
}
Vector3 operator*(Vector3 a, Vector3 b) {
    a *= b;
    return a;
}
Vector3& Vector3::operator*=(Vector3 o) {
    this->x *= o.x;
    this->y *= o.y;
    this->z *= o.z;
    return *this;
}
Vector3 Vector3::operator/(Vector3 o) {
    Vector3 v = *this;
    v /= o;
    return v;
}
Vector3& Vector3::operator/=(Vector3 o) {
    this->x /= o.x;
    this->y /= o.y;
    this->z /= o.z;
    return *this;
}
Vector3 Vector3::operator*(float o) {
    Vector3 v = *this;
    v *= o;
    return v;
}
Vector3& Vector3::operator*=(float o) {
    this->x *= o;
    this->y *= o;
    this->z *= o;
    return *this;
}
Vector3 Vector3::operator/(float o) {
    Vector3 v = *this;
    v /= o;
    return v;
}
Vector3& Vector3::operator/=(float o) {
    this->x /= o;
    this->y /= o;
    this->z /= o;
    return *this;
}
Vector3 Vector3::operator+(float o) {
    Vector3 v = *this;
    v += o;
    return v;
}
Vector3& Vector3::operator+=(float o) {
    this->x += o;
    this->y += o;
    this->z += o;
    return *this;
}
Vector3 Vector3::operator-(float o) {
    Vector3 v = *this;
    v -= o;
    return v;
}
Vector3& Vector3::operator-=(float o) {
    this->x -= o;
    this->y -= o;
    this->z -= o;
    return *this;
}
Vector3& Vector3::operator=(const Vector3& o) {
    this->x = o.x;
    this->y = o.y;
    this->z = o.z;
    return *this;
}
bool operator==(Vector3 a, Vector3 b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(Vector3 a, Vector3 b)
{
    return !(a == b);
}

bool operator<(Vector3 a, Vector3 b)
{
    return a.norm2() < b.norm2();
}

bool operator<=(Vector3 a, Vector3 b)
{
    return !(a > b);
}

bool operator>(Vector3 a, Vector3 b)
{
    return a.norm2() > b.norm2();
}

bool operator>=(Vector3 a, Vector3 b)
{
    return !(a < b);
}

std::ostream& operator<<(std::ostream& io, const Vector3& v) {
    io << v.toString();
    return io;
}

std::ostream& operator<<(std::ostream& io, std::shared_ptr<Vector3> v) {
    io << v->toString();
    return io;
}

