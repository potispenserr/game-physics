#pragma once
#include "RigidBody.h"
#include "GraphicsNode.h"
#include "CollisionManifold.h"

class PhysicsSystem
{
protected:
std::vector<RigidBody*> rBodies;
std::vector<GraphicsNode> constraints;
std::vector<RigidBody*> colliderPair1;
std::vector<RigidBody*> colliderPair2;
std::vector<CollisionManifold> collisionResults;
float linearProjectionPercent; // the range should be between 0.2 - 0.8
float penetrationSlack; // range 0.01 - 0.1
int impulseIterations; // range 1 - 20 with less jitter the higher you go but start with 6

public:
    PhysicsSystem();
    ~PhysicsSystem();

    void update(float deltaTime);
    void render();

    void addConstraint(const GraphicsNode& constraint);
    void addRigidBody(RigidBody* newBody);

    void clearConstraints();
    void clearRigidbodies();


};
