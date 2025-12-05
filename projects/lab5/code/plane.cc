#include "plane.h"

Plane::Plane() {

}

Plane::Plane(std::vector<Vector4D> &pointvectors) {
    this->pointVectors = pointVectors;
    Vector4D ab = pointVectors[1] - pointVectors[0];
    Vector4D ac = pointVectors[2] - pointVectors[0];
    this->normal = Vector4D::cross(ab, ac);
}

Vector4D& Plane::getPoint(int index) {
    return this->pointVectors[index];
}

Vector4D& Plane::getNormal() {
    return normal;
}