#include "plane.h"

Plane::Plane() {

}

Plane::~Plane() {

}

Plane::Plane(std::vector<Vector4D> pointVectors) {
    this->pointVectors = pointVectors;
    Vector4D ab = this->pointVectors[1] - this->pointVectors[0];
    Vector4D ac = this->pointVectors[2] - this->pointVectors[0];
    this->normal = Vector4D::cross(ab, ac);
}

Vector4D& Plane::getPoint(int index) {
    return this->pointVectors[index];
}

Vector4D& Plane::getNormal() {
    return normal;
}
