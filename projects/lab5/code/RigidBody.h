#pragma once
#define RIGIDBODY_TYPE_BASE 0
#define RIGIDBODY_TYPE_BOX 1
#include <vector>
#include "GraphicsNode.h"


class RigidBody
{
public:
    RigidBody();
    virtual ~RigidBody();

    virtual void update(float deltaTime) {}
    virtual void render(const Camera& cam, const Matrix4D& projection, const Vector4D& lightPos) {}
    virtual void applyForces() {}
    virtual void solveConstraints(const std::vector<GraphicsNode>& constraints) {}

    bool hasVolume();

    int type;
    
};

