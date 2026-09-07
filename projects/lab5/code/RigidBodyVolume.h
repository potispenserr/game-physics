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

    void render(const Camera& cam, const Matrix4D& projection, const Vector4D& lightPos);
    void update(const float deltaTime);
    void applyForces();
    void synchCollisionVolumes();
    void addLinearImpulse(const Vector4D& impulse);
    void updateAABB(const float r, const float g, const float b);
    void updateAABB(const bool AABBRender);
    
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
