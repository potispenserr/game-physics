#include "plane.h"
#include "core/app.h"
Plane::Plane() {

}

Plane::~Plane() {

}
Plane::Plane(Plane &newPlane) {
    this->normal = newPlane.getNormal();
    this->pointVectors = newPlane.getPoints();
}

Plane::Plane(std::vector<Vector4D> pointVector) {
    this->pointVectors = pointVector;
    Vector4D ab = this->pointVectors[1] - this->pointVectors[0];
    Vector4D ac = this->pointVectors[2] - this->pointVectors[0];
    this->normal = Vector4D::cross(ab, ac);
    this->distance = Vector4D::dot(this->normal, pointVectors[0]);

    float xMax = pointVectors[0].x();
    float xMin = pointVectors[0].x();

    float yMax = pointVectors[0].y();
    float yMin = pointVectors[0].y();

    float zMax = pointVectors[0].z();
    float zMin = pointVectors[0].z();
    for (int i = 1; i < pointVectors.size(); i++){
        if(xMax < pointVectors[i].x()){
            xMax = pointVectors[i].x();
        }
        if(xMin > pointVectors[i].x()){
            xMin = pointVectors[i].x();
        }

        if(yMax < pointVectors[i].y()){
            yMax = pointVectors[i].y();
        }
        if(yMin > pointVectors[i].y()){
            yMin = pointVectors[i].y();
        }

        if(zMax < pointVectors[i].z()){
            zMax = pointVectors[i].z();
        }
        if(zMin > pointVectors[i].z()){
            zMin = pointVectors[i].z();
        }

    }
    maxBounds = {xMax, yMax, zMax};
    minBounds = {xMin, yMin, zMin};

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    
    unsigned int indices[] = {
        0, 1, 3, 
        1, 2, 3 
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

Vector4D& Plane::getPoint(int index) {
    return this->pointVectors[index];
}

Vector4D& Plane::getNormal() {
    return normal;
}

float &Plane::getDistance()
{
    return distance;
}

Vector4D &Plane::getMaxBounds()
{
    return maxBounds;
}

Vector4D &Plane::getMinBounds()
{
    return minBounds;
}

void Plane::draw()
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

std::vector<Vector4D>& Plane::getPoints() {
    return pointVectors;
}
