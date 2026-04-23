#pragma once
#include "RigidBody.h"
#include "GraphicsNode.h"

class PhysicsSystem
{
protected:
std::vector<RigidBody*> rBodies;
std::vector<GraphicsNode> constraints;
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
