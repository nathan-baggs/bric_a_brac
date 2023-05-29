#include "physics_manager.h"

#include "vector3.h"

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "LinearMath/btDefaultMotionState.h"

namespace
{

/**
 * Helper function to convert an engine vector to a bullet vector.
 *
 * @param vector
 *   Engine vector to convert.
 *
 * @returns
 *   Supplied engine vector as a bullet vector.
 */
::btVector3 to_bullet(const bab::Vector3 &vector)
{
    return {vector.x, vector.y, vector.z};
}

}

namespace bab
{

PhysicsManager::PhysicsManager()
    : collision_config_()
    , collision_dispatcher_(&collision_config_)
    , broadphase_()
    , solver_()
    , ghost_pair_callback_()
    , world_(&collision_dispatcher_, &broadphase_, &solver_, &collision_config_)
    , shapes_()
    , motion_states_()
    , rigid_bodies_()
{
    broadphase_.getOverlappingPairCache()->setInternalGhostPairCallback(&ghost_pair_callback_);
    world_.setGravity({-0.0f, -10.0f, 0.0f});
}

PhysicsManager::~PhysicsManager()
{
    for (const auto &body : rigid_bodies_)
    {
        world_.removeRigidBody(body.get());
    }
}

void PhysicsManager::add_static_rigid_body(const Vector3 &half_extent, const Vector3 &position)
{
    ::btTransform start_transform{};
    start_transform.setIdentity();
    start_transform.setOrigin(to_bullet(position));

    shapes_.push_back(std::make_unique<::btBoxShape>(to_bullet(half_extent)));
    auto *shape = shapes_.back().get();

    motion_states_.push_back(std::make_unique<::btDefaultMotionState>(start_transform));
    auto *motion_state = motion_states_.back().get();

    ::btRigidBody::btRigidBodyConstructionInfo info{0.0f, motion_state, shape, {0.0f, 0.0f, 0.0f}};

    rigid_bodies_.emplace_back(std::make_unique<::btRigidBody>(info));
    auto *rigid_body = rigid_bodies_.back().get();

    rigid_body->setFriction(1.0f);
    world_.addRigidBody(rigid_body);
}

void PhysicsManager::add_dynamic_rigid_body(const Vector3 &half_extent, const Vector3 &position, float mass)
{
    ::btTransform start_transform{};
    start_transform.setIdentity();
    start_transform.setOrigin(to_bullet(position));

    ::btVector3 local_inertia{0.0f, 0.0f, 0.0f};

    shapes_.push_back(std::make_unique<::btBoxShape>(to_bullet(half_extent)));
    auto *shape = shapes_.back().get();
    shape->calculateLocalInertia(mass, local_inertia);

    motion_states_.push_back(std::make_unique<::btDefaultMotionState>(start_transform));
    auto *motion_state = motion_states_.back().get();

    ::btRigidBody::btRigidBodyConstructionInfo info{mass, motion_state, shape, local_inertia};

    rigid_bodies_.emplace_back(std::make_unique<::btRigidBody>(info));
    auto *rigid_body = rigid_bodies_.back().get();

    rigid_body->setFriction(1.0f);
    world_.addRigidBody(rigid_body);
}

void PhysicsManager::update()
{
    world_.stepSimulation(0.16f);
}

}