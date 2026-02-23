#pragma once
#include "Vector4D.h"
#include <vector>
class Plane {
    public: 
        Plane();
        Plane(std::vector<Vector4D> pointVector);

        Plane(Plane &plane);

        ~Plane();

        Vector4D& getPoint(int index);
        std::vector<Vector4D>& getPoints();

        Vector4D& getNormal();
        float& getDistance();
        Vector4D& getMaxBounds();
        Vector4D& getMinBounds();

        void draw();

    private:
        Vector4D normal;
        float distance; 
        std::vector<Vector4D> pointVectors;
        Vector4D maxBounds;
        Vector4D minBounds;
        unsigned int VBO, VAO, EBO;
        


};