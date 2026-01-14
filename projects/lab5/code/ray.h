#pragma once
#include "Vector4D.h"
#include "plane.h"
class Ray
{
public:
    Ray();
    Ray(Vector4D startPoint, Vector4D endPoint);
    ~Ray();
    Ray(const Ray &ray);

    void initializeRay();
    void draw();
    Vector4D Intersect(Plane &plane);
    Vector4D rayColor;


private:
    Vector4D rayOrigin;
    Vector4D rayDirection;
    std::vector<float> rayCoords;
    unsigned int rayVAO;
    unsigned int rayVBO;

};
