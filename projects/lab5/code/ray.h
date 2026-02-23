#pragma once
#include "Vector4D.h"
#include "Matrix4D.h"
#include "plane.h"
#include "GraphicsNode.h"
class Ray
{
public:
    Ray();
    Ray(Vector4D startPoint, Vector4D endPoint);
    ~Ray();
    Ray(const Ray &ray);

    void initializeRay();
    void draw();
    void rayCast(double xPos, double yPos, Matrix4D view, Matrix4D projection, unsigned int screenW, unsigned int screenH);

    Vector4D getRayOrigin();
    Vector4D getRayDir();
    bool Intersect(Plane& plane, Vector4D& hitPoint);
    bool Intersect(GraphicsNode &gn, Vector4D& hitPoint);
    Vector4D rayColor;



private:
    Vector4D rayOrigin;
    Vector4D rayDirection;
    std::vector<float> rayCoords;
    unsigned int rayVAO;
    unsigned int rayVBO;

};
