#include "RigidBody.h"

RigidBody::RigidBody()
{
    type = RIGIDBODY_TYPE_BASE;
}

RigidBody::~RigidBody()
{
}

bool RigidBody::hasVolume()
{
    return type == RIGIDBODY_TYPE_BOX;
}
