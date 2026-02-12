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

Vector4D Ray::Intersect(Plane &plane)
{
    // Vector4D ray = rayOrigin - rayDirection;
    // ray.norm();

    // if(Vector4D::dot(plane.getNormal(), ray) >= 0){
    //     std::cout << "early intersect exit" << "\n";
    //     return Vector4D(0, 0, 0);
    // }
    // Vector4D planePoint = plane.getPoint(0);
    // float p = Vector4D::dot((plane.getPoint(0) - rayOrigin), plane.getNormal());
    // if (p == 0) {
    //     std::cout << "line is not intersecting or is in the plane" << "\n";
    //     return Vector4D(0, 0, 0);
    // }
    
    // float d = p / Vector4D::dot(ray, plane.getNormal());

    // Vector4D intersectPoint = rayOrigin + ray * d;


    // std::cout << "Ray hits plane at " << intersectPoint.x() << " " << intersectPoint.y() << " " << intersectPoint.z() << "\n";

    Vector4D ray = rayOrigin - rayDirection;
    float d = Vector4D::dot(plane.getNormal(), plane.getPoint(0));
    if(Vector4D::dot(plane.getNormal(), ray) >= 0){
        std::cout << "early intersect exit" << "\n";
        return Vector4D(0, 0, 0);
    }
    float x = (d - Vector4D::dot(plane.getNormal(), rayOrigin)) / Vector4D::dot(plane.getNormal(), ray);

    Vector4D intersectPoint = rayOrigin + ray * x;
    std::cout << "Ray hits plane at " << intersectPoint.x() << " " << intersectPoint.y() << " " << intersectPoint.z() << "\n";

    return intersectPoint;
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

bool Ray::isInsideSquare()
{
    return false;
}
