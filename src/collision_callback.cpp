#include "collision_callback.h"

#include <functional>
#include <unordered_map>

#include "rigid_body.h"

#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"

namespace bab
{

CollisionCallback::CollisionCallback()
    : has_collided_(false)
{
}

::btScalar CollisionCallback::addSingleResult(
    ::btManifoldPoint &,
    const ::btCollisionObjectWrapper *colObj0Wrap,
    int,
    int,
    const ::btCollisionObjectWrapper *colObj1Wrap,
    int,
    int)
{
    has_collided_ = true;
    return 0;
}

CollisionCallback::operator bool() const
{
    return has_collided_;
}

}
