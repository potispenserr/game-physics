#pragma once
#include "Vector4D.h"
#include <vector>
class Plane {
    public: 
        Plane();
        Plane(const std::vector<Vector4D>& pointVector);
        Plane(const Vector4D& p1, const Vector4D& p2, const Vector4D& p3);

        Plane(const Plane &plane);

        ~Plane();

        Vector4D getPoint(int index) const;
        std::vector<Vector4D> getPoints() const;

        Vector4D getNormal() const;
        float getDistance() const;
        Vector4D& getMaxBounds();
        Vector4D& getMinBounds();

        void setNormal(const Vector4D& normal);
        void setDistance(const float& distance);

        void draw();

    private:
        Vector4D normal;
        float distance; 
        std::vector<Vector4D> pointVectors;
        Vector4D maxBounds;
        Vector4D minBounds;
        unsigned int VBO, VAO, EBO;
        


};