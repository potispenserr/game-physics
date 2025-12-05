#pragma once
#include "Vector4D.h"
#include <vector>
class Plane {
    public: 
        Plane();
        Plane(std::vector<Vector4D> &pointvectors);

        Plane(Plane &plane);

        ~Plane();

        Vector4D& getPoint(int index);

        Vector4D& getNormal();

    private:
        Vector4D normal;
        std::vector<Vector4D> pointVectors;
        


};