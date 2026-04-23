#pragma once
#include "RigidBody.h"
#include "Vector4D.h"
#include "GraphicsNode.h"
#define GRAV_CONST Vector4D(0.0f, -9.82f, 0.0f)

class RigidBodyVolume : public RigidBody
{
public:
    RigidBodyVolume();
    RigidBodyVolume(int bodyType);

    ~RigidBodyVolume();

    void render();
    void update(float deltaTime);
    void applyForces();
    void synchCollisionVolumes();
    void addLinearImpulse(const Vector4D& impulse);

    float inverseMass();

    Vector4D velocity;
    Vector4D position;
    Vector4D forces;
    float mass;
    GraphicsNode gn;

};

