#include "PhysicsSystem.h"
#include "RigidBodyVolume.h"
#include <iostream>

PhysicsSystem::PhysicsSystem()
{
    impulseIterations = 6;
    penetrationSlack = 0.01f;
    linearProjectionPercent = 0.45;
    colliderPair1.reserve(100);
    colliderPair2.reserve(100);
    collisionResults.reserve(100);
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::update(float deltaTime)
{
    colliderPair1.clear();
    colliderPair2.clear();
    collisionResults.clear();
    for (int i = 0; i < rBodies.size(); ++i){
        for (int j = i; j < rBodies.size(); ++j){
            if (i == j){
                continue;
            }
            CollisionManifold result;

            if (rBodies[i]->hasVolume() == true && rBodies[i]->hasVolume() == true) {
                RigidBodyVolume* rbv1 = (RigidBodyVolume*)rBodies[i];
                RigidBodyVolume* rbv2 = (RigidBodyVolume*)rBodies[j];
                result = findCollisions(*rbv1, *rbv2);
            }
            if(result.isColliding == true){
                colliderPair1.push_back(rBodies[i]);
                colliderPair2.push_back(rBodies[j]);
                collisionResults.push_back(result);
            }
        }

    }

    for (int i = 0; i < rBodies.size(); i++) {
        rBodies[i]->applyForces();
    }

    for (int i = 0; i < impulseIterations; ++i){
        for (int j = 0; j < collisionResults.size(); ++j) {
            for (int k = 0; k < collisionResults[j].contactPoints.size(); ++k) {
                if(colliderPair1[j]->hasVolume() == true && 
                    colliderPair2[j]->hasVolume() == true){
                        RigidBodyVolume* rbv1 = (RigidBodyVolume*)colliderPair1[j];
                        RigidBodyVolume* rbv2 = (RigidBodyVolume*)colliderPair2[j];
                        applyImpulse(*rbv1, *rbv2, collisionResults[j], k);

                }
            }
        }

    }

    for (int i = 0; i < rBodies.size(); ++i){
        rBodies[i]->update(deltaTime);
    }

    for (int i = 0; i < collisionResults.size(); ++i){
        if(!colliderPair1[i]->hasVolume() == true && 
            !colliderPair2[i]->hasVolume() == true){
                continue;      
        }
        RigidBodyVolume* rbv1 = (RigidBodyVolume*)colliderPair1[i];
        RigidBodyVolume* rbv2 = (RigidBodyVolume*)colliderPair2[i];
        float totalMass = rbv1->inverseMass() + rbv2->inverseMass();

        if(totalMass == 0.0f) {
            continue;
        }
        float depth = std::max(collisionResults[i].depth - penetrationSlack, 0.0f);
        float scalar = totalMass == 0.0f ? 0.0f : depth / totalMass;
        Vector4D correction = collisionResults[i].collisionNormal * scalar * linearProjectionPercent;

        rbv1->position = rbv1->position - correction * rbv1->inverseMass();
        rbv2->position = rbv2->position + correction * rbv2->inverseMass();



        rbv1->synchCollisionVolumes();
        rbv2->synchCollisionVolumes();

    }
    

}

void PhysicsSystem::render(const Camera& cam, const Matrix4D& projection, const Vector4D& lightPos)
{
    rBodies[rBodies.size() - 1]->render(cam, projection, lightPos);
    for (int i = 0; i < rBodies.size() - 1; i++){
        rBodies[i]->render(cam, projection, lightPos);
    }
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
