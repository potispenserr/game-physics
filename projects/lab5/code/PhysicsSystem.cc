#include "PhysicsSystem.h"

void PhysicsSystem::update(float deltaTime)
{
    for (int i = 0; i < rBodies.size(); i++) {
        rBodies[i]->applyForces();
    }
}

void PhysicsSystem::render()
{
}

void PhysicsSystem::addConstraint(const GraphicsNode &constraint)
{
    constraints.push_back(constraint);
}

void PhysicsSystem::addRigidBody(RigidBody *newBody)
{
    rBodies.push_back(newBody);
}

void PhysicsSystem::clearConstraints()
{
    constraints.clear();
}

void PhysicsSystem::clearRigidbodies()
{
    rBodies.clear();
}
