#include "physics_manager.h"

#include <functional>
#include <unordered_map>
#include <vector>

#include "debug_drawer.h"
#include "rigid_body.h"
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
    , debug_drawer_(nullptr)
    , collision_callbacks_()
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

RigidBody PhysicsManager::add_dynamic_rigid_body(const Vector3 &half_extent, const Vector3 &position, float mass)
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

    return {rigid_body};
}

void PhysicsManager::register_collision_callback(const RigidBody &rigid_body, std::function<bool()> callback)
{
    auto *bullet_rigid_body = rigid_body.rigid_body_;
    collision_callbacks_.try_emplace(bullet_rigid_body, std::move(callback));
}

void PhysicsManager::set_debug_drawer(DebugDrawer *debug_drawer)
{
    debug_drawer_ = debug_drawer;
    world_.setDebugDrawer(debug_drawer);
}

void PhysicsManager::update()
{
    world_.stepSimulation(0.16f);

    // test for collisions on all rigid bodies with a registered callback
    for (auto &[rigid_body, callback] : collision_callbacks_)
    {
        CollisionCallback collision_check{};
        world_.contactTest(rigid_body, collision_check);

        if (collision_check)
        {
            // if the callback returns true then we can clear it
            if (callback())
            {
                collision_callbacks_[rigid_body] = nullptr;
            }
        }
    }

    // remove all entries with cleared callbacks
    std::erase_if(collision_callbacks_, [](const auto &element) { return !std::get<1>(element); });

    if (debug_drawer_ != nullptr)
    {
        world_.debugDrawWorld();
    }
}

}