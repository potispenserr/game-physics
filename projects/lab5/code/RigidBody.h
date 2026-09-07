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

    virtual void update(const float deltaTime) {}
    virtual void render(const Camera& cam, const Matrix4D& projection, const Vector4D& lightPos) {}
    virtual void applyForces() {}
    virtual void updateAABB(const float r, const float g, const float b) {}

    bool hasVolume();

    int type;
    
};

