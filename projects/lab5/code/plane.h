#pragma once
#include "Vector4D.h"
#include <vector>
class Plane {
    public: 
        Plane();
        Plane(std::vector<Vector4D> pointVectors);

        Plane(Plane &plane);

        ~Plane();

        Vector4D& getPoint(int index);
        std::vector<Vector4D>& getPoints();

        Vector4D& getNormal();
        float& getDistance();

    private:
        Vector4D normal;
        float distance; 
        std::vector<Vector4D> pointVectors;
        


};