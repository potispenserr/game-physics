#include "ray.h"
#include <iostream>
#include "render/window.h"
#define CMP(x, y) \
	(fabsf(x - y) <= __FLT_EPSILON__ * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))
Ray::Ray() {

}

Ray::~Ray() {

}

Ray::Ray(const Ray &ray)
{
    this->rayOrigin = ray.rayOrigin;
    this->rayDirection = ray.rayDirection;
    this->rayColor = ray.rayColor;
    this->rayVAO = ray.rayVAO;
    this->rayVBO = ray.rayVBO;
    this->rayCoords = ray.rayCoords;
}

Ray::Ray(Vector4D startPoint, Vector4D endPoint) {
    this->rayOrigin = startPoint;
    this->rayDirection = endPoint;
    Vector4D lolRayEndpoint = Vector4D(startPoint + endPoint * 50);
    rayCoords = {startPoint.x(), startPoint.y(), startPoint.z(), endPoint.x(), endPoint.y(), endPoint.z()};

    initializeRay();
}

void Ray::initializeRay()
{
    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);
    glBindVertexArray(rayVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rayCoords) * rayCoords.size(), rayCoords.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

void Ray::draw()
{
    // glBindVertexArray(rayVAO);
    // glDrawArrays(GL_LINES, 0, 2);
    glBegin(GL_LINES);
	glVertex3f(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());
	glVertex3f(rayDirection.x(), rayDirection.y(), rayDirection.z());
	glEnd();
}

Vector4D Ray::getRayOrigin()
{
    return rayOrigin;
}

Vector4D Ray::getRayDir()
{
    return rayDirection;
}

bool Ray::Intersect(Plane& plane, Vector4D& hitPoint)
{
    Vector4D ray = rayOrigin - rayDirection;
    float d = Vector4D::dot(plane.getNormal(), plane.getPoint(0));
    if(Vector4D::dot(plane.getNormal(), ray) >= 0){
        std::cout << "early intersect exit" << "\n";
        return false;
    }
    Vector4D max = plane.getMaxBounds();
    Vector4D min = plane.getMinBounds();

    float x = (d - Vector4D::dot(plane.getNormal(), rayOrigin)) / Vector4D::dot(plane.getNormal(), ray);

    Vector4D intersectPoint = rayOrigin + ray * x;

    //check if intersect point is outside the bounds
    if(intersectPoint.x() <= min.x() || intersectPoint.x() >= max.x() || intersectPoint.y() <= min.y() || intersectPoint.y() >= max.y()){
        std::cout << "ray is outside the plane bounds" << "\n";
        return false;
    }

    std::cout << "Ray hits plane at " << intersectPoint.x() << " " << intersectPoint.y() << " " << intersectPoint.z() << "\n";
    hitPoint = intersectPoint;

    return true;
}

bool Ray::Intersect(GraphicsNode &gn, Vector4D& hitPoint)
{
    Vector4D max = gn.getMesh().get()->maxCoords;
    Vector4D min = gn.getMesh().get()->minCoords;

    float xDivisor = (rayDirection.x() == 0.0f) ? 0.00001f : rayDirection.x();
    float yDivisor = (rayDirection.y() == 0.0f) ? 0.00001f : rayDirection.y();
    float zDivisor = (rayDirection.z() == 0.0f) ? 0.00001f : rayDirection.z();

    float xMax = (max.x() - rayOrigin.x()) / xDivisor;
    float xMin = (min.x() - rayOrigin.x()) / xDivisor;

    float yMax = (max.y() - rayOrigin.y()) / yDivisor;
    float yMin = (min.y() - rayOrigin.y()) / yDivisor;

    float zMax = (max.z() - rayOrigin.z()) / zDivisor;
    float zMin = (min.z() - rayOrigin.z()) / zDivisor;

    float totalMax = fminf(fminf(fmaxf(xMax, xMin), fmaxf(yMax, yMin)), fmaxf(zMax, zMin));
    float totalMin = fmaxf(fmaxf(fminf(xMax, xMin), fminf(yMax, yMin)), fminf(zMax, zMin));

    //ray is intersecting AABB but the AABB is behind the ray origin
    if(totalMax < 0){
        std::cout << "ray does not intersect GN totalMax < 0" << "\n";
        return false;
    }

    if(totalMin > totalMax){
        std::cout << "ray does not intersect GN totalMin > totalMax" << "\n";
        return false;
    }

    float totalResult = totalMin;

    if(totalMin < 0.0f){
        totalResult = totalMax;
    }
    
    hitPoint = rayOrigin + rayDirection * totalResult;

    return true;
}

void Ray::rayCast(double xPos, double yPos, Matrix4D view, Matrix4D projection, unsigned int screenW, unsigned int screenH) 
{
	
	Vector4D near(xPos, yPos, 0.0f);
	Vector4D far(xPos, yPos, 1.0f);
	Vector4D viewportOrigin;
	Vector4D viewportSize(screenW, screenH);
	Vector4D pNear = Matrix4D::unproject(near, viewportOrigin, viewportSize, view, projection);
	Vector4D pFar = Matrix4D::unproject(far, viewportOrigin, viewportSize, view, projection);

	Vector4D origin = pNear;
	Vector4D direction = pFar - pNear;


    this->rayOrigin = origin;
    this->rayDirection = direction;

}

