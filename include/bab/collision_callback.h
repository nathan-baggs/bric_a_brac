#pragma once

#include <functional>
#include <unordered_map>

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

namespace bab
{

class RigidBody;

/**
 * Class for registering bullet collisions.
 */
class CollisionCallback : public ::btCollisionWorld::ContactResultCallback
{
  public:
    /**
     * Construct a new CollisionCallback.
     */
    CollisionCallback();

    /**
     * Callback to register a collision.

     * @returns
     *   Unused by bullet, we always return 0.
     */
    btScalar addSingleResult(
        btManifoldPoint &,
        const btCollisionObjectWrapper *,
        int,
        int,
        const btCollisionObjectWrapper *,
        int,
        int);

    /**
     * Check if a collision has been registered.
     *
     * @returns
     *   True if a collision has happened, false otherwise.
     */
    explicit operator bool() const;

  private:
    /** Flag indicating if a collision has happened. */
    bool has_collided_;
};

}
