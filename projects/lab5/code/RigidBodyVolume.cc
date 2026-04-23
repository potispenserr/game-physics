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
