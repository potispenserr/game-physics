#pragma once
#define RIGIDBODY_TYPE_BASE 0
#define RIGIDBODY_TYPE_BOX 1


class RigidBody
{
public:
    RigidBody();
    virtual ~RigidBody();

    virtual void update(float deltaTime) {}
    virtual void render() {}
    virtual void applyForces() {}
    virtual void solveConstraints() {}

    bool hasVolume();

    int type;
    
};

