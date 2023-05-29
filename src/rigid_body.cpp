#include "rigid_body.h"

#include "quaternion.h"
#include "vector3.h"

#include "BulletDynamics/Dynamics/btRigidBody.h"

namespace
{

/**
 * Helper function to convert a bullet vector to an engine vector.
 *
 * @param vector
 *   Bullet vector to convert.
 *
 * @returns
 *   Supplied bullet vector as an engine vector.
 */
bab::Vector3 to_engine(const ::btVector3 &vector)
{
    return {vector.getX(), vector.getY(), vector.getZ()};
}

/**
 * Helper function to convert a bullet vector to an engine colour.
 *
 * @param vector
 *   Bullet vector to convert, xyz will become rgb.
 *
 * @param alpha
 *   The alpha value for the colour.
 *
 * @returns
 *   Supplied bullet vector as an engine colour.
 */
bab::Quaternion to_engine(const ::btQuaternion &quaternion)
{
    return {quaternion.getW(), quaternion.getX(), quaternion.getY(), quaternion.getZ()};
}

}
namespace bab
{

RigidBody::RigidBody(::btRigidBody *rigid_body)
    : rigid_body_(rigid_body)
{
}

Vector3 RigidBody::position() const
{
    const auto transform = rigid_body_->getWorldTransform();
    return to_engine(transform.getOrigin());
}

Quaternion RigidBody::orientation() const
{
    const auto transform = rigid_body_->getWorldTransform();
    return to_engine(transform.getRotation());
}

}
