#pragma once
#include "RigidBody.h"
#include "Vector4D.h"
#include "GraphicsNode.h"
#include "CollisionManifold.h"
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
    // coefficient of restitution
    float cor;
    GraphicsNode gn;
    
};

CollisionManifold findCollisions(RigidBodyVolume& r1, RigidBodyVolume& r2);
void applyImpulse(RigidBodyVolume& r1, RigidBodyVolume& r2, const CollisionManifold& manifold, int constant);
