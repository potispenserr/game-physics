#pragma once
#include "Vector4D.h"
#include "plane.h"
class Ray
{
public:
    Ray();
    Ray(Vector4D startPoint, Vector4D direction);
    ~Ray();

    Vector4D Intersect(Plane &plane);

private:
    Vector4D rayStart;
    Vector4D rayDirection;

};
