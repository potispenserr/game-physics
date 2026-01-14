#include "ray.h"
#include <iostream>
#include "render/window.h"
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
    Vector4D lolRayEndpoint = Vector4D(rayOrigin + rayDirection  * -50);
    glBegin(GL_LINES);
	glVertex3f(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());
	glVertex3f(rayDirection.x(), rayDirection.y(), rayDirection.z());
	glEnd();
}

Vector4D Ray::Intersect(Plane &plane)
{
    Vector4D ray = rayDirection - rayOrigin;

    float d = Vector4D::dot(plane.getNormal(), plane.getPoint(0));
    if(Vector4D::dot(plane.getNormal(), ray) == 0){
        std::cout << "early intersect exit" << "\n";
        return Vector4D(0, 0, 0);
    }
    
    float t = (d - Vector4D::dot(plane.getNormal(), rayOrigin) / Vector4D::dot(plane.getNormal(), ray ));

    Vector4D intersectPoint = rayOrigin + ray * t;

    return intersectPoint;
}