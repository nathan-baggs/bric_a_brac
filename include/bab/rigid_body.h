#pragma once

#include "quaternion.h"
#include "vector3.h"

class btRigidBody;

namespace bab
{

/**
 * Class which wraps a rigid body created by the PhysicsManager.
 */
class RigidBody
{
  public:
    /**
     * Get the world space position.
     *
     * @returns
     *   Position in world space.
     */
    Vector3 position() const;

    /**
     * Get the world space orientation.
     *
     * @returns
     *   Orientation in world space.
     */
    Quaternion orientation() const;

  private:
    // allow PhysicsManager to construct this object
    friend class PhysicsManager;

    /**
     * Construct a new RigidBody, private so only PhysicsManager can call.
     *
     * @param rigid_body
     *   The bullet rigid body to wrap.
     */
    RigidBody(::btRigidBody *rigid_body);

    /** Bullet rigid body object. */
    ::btRigidBody *rigid_body_;
};

}
