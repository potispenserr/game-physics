#include "RigidBodyVolume.h"

RigidBodyVolume::RigidBodyVolume() : 
mass(1.0f) {
    type = RIGIDBODY_TYPE_BASE;
}

RigidBodyVolume::RigidBodyVolume(int bodyType) : 
mass(1.0f) {
    type = bodyType;
}

RigidBodyVolume::~RigidBodyVolume()
{
}

void RigidBodyVolume::render()
{

}

void RigidBodyVolume::update(float deltaTime)
{
    const float damping = 0.98f;
    Vector4D accel = forces * inverseMass();
    velocity = velocity + accel * deltaTime;
    velocity = velocity * damping;
    position = position + velocity * deltaTime;
    synchCollisionVolumes();
}

void RigidBodyVolume::applyForces()
{
    forces = GRAV_CONST * mass;
}

void RigidBodyVolume::synchCollisionVolumes()
{
    gn.AABBCenter = position;
}

void RigidBodyVolume::addLinearImpulse(const Vector4D &impulse)
{
    velocity = velocity + impulse;
}


float RigidBodyVolume::inverseMass()
{
    if(mass == 0.0f) {
        return 0.0f;
    }
    return 1.0f / mass;
}


void applyImpulse(RigidBodyVolume &r1, RigidBodyVolume &r2, const CollisionManifold &manifold, int constant)
{
    float invMass1 = r1.inverseMass();
    float invMass2 = r2.inverseMass();
    float invSum = invMass1 + invMass2;
    if (invSum == 0.0f) {
        return;
    }

    Vector4D relativeVelo = r2.velocity - r1.velocity;

    Vector4D relativeCollNorm = manifold.collisionNormal;
    relativeCollNorm = relativeCollNorm.norm();

    //if they're moving away from each other? do jack shit

    if(Vector4D::dot(relativeVelo, relativeCollNorm) > 0.0f){
        return;
    }

    float minCOR = std::min(r1.cor, r2.cor);
    float numerator (-(1.0f + minCOR) * Vector4D::dot(relativeVelo, relativeCollNorm));
    float impulse = numerator / invSum;

    if(manifold.contactPoints.size() > 0 && impulse != 0.0f) {
        impulse /= (float)manifold.contactPoints.size();
    }
    Vector4D normalImpulse = relativeCollNorm * impulse;
    r1.velocity = r1.velocity - normalImpulse * invMass1;
    r2.velocity = r2.velocity + normalImpulse * invMass2;


}

CollisionManifold findCollisions(RigidBodyVolume &r1, RigidBodyVolume &r2)
{
    CollisionManifold result;
    if(r1.type == RIGIDBODY_TYPE_BOX) {
        if(r2.type == RIGIDBODY_TYPE_BOX) {
            result.SATOnAABBs(r1.gn, r2.gn);
        }
    }
    return result;
}