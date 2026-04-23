#include "RigidBody.h"

RigidBody::RigidBody()
{
    type = RIGIDBODY_TYPE_BASE;
}

bool RigidBody::hasVolume()
{
    return type == RIGIDBODY_TYPE_BOX;
}
