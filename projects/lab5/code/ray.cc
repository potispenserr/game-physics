#include "ray.h"

Ray::Ray() {

}

Ray::Ray(Vector4D startPoint, Vector4D direction) {
    this->rayStart = startPoint;
    this->rayDirection = direction;
}

Vector4D Ray::Intersect(Plane &plane) {
    Vector4D ray = rayDirection - rayStart;

    float d = Vector4D::dot(plane.getNormal(), plane.getPoint(0));
    if(Vector4D::dot(plane.getNormal(), ray) == 0){
        return;
    }
    
    float t = (d - Vector4D::dot(plane.getNormal(), rayStart) / Vector4D::dot(plane.getNormal(), ray ));

    Vector4D intersectPoint = rayStart + ray * t;

    return intersectPoint;

}