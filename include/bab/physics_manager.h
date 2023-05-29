#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "collision_callback.h"
#include "rigid_body.h"
#include "vector3.h"

#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btIDebugDraw.h"

namespace bab
{

class DebugDrawer;

/**
 * Class to handle all things related to physics.
 */
class PhysicsManager
{
  public:
    /**
     * Construct a new PhysicsManager.
     */
    PhysicsManager();

    /**
     * PhysicsManager specific cleanup.
     */
    ~PhysicsManager();

    /**
     * Add a cube static rigid body to the simulation.
     *
     * @param half_extents
     *   The half extends of the cube representing the rigid body.
     *
     * @param position
     *   The position in world space of the rigid body.
     */
    void add_static_rigid_body(const Vector3 &half_extent, const Vector3 &position);

    /**
     * Add a cube dynamic rigid body to the simulation.
     *
     * @param half_extents
     *   The half extends of the cube representing the rigid body.
     *
     * @param position
     *   The position in world space of the rigid body.
     *
     * @param mass
     *   The mass of the rigid body.
     *
     * @returns
     *   Wrapper for the internal bullet rigid body.
     */
    RigidBody add_dynamic_rigid_body(const Vector3 &half_extent, const Vector3 &position, float mass);

    /**
     * Register a new collision callback.
     *
     * @param rigid_body.
     *   The rigid body to test for collisions to fire the callback.
     *
     * @param callback
     *   The callback to fire when the rigid body has a collision, the callback should return true if it is to be
     *   consumed and not fired again, or false if it should be fired on further collisions.
     */
    void register_collision_callback(const RigidBody &rigid_body, std::function<bool()> callback);

    /**
     * Set the DebugDraer object, until this is called no debug information will be rendered.
     *
     * @param debug_drawer
     *   The object for handling debug information.
     */
    void set_debug_drawer(DebugDrawer *debug_drawer);

    /**
     * Advance the physics simulation, should be called every frame.
     */
    void update();

  private:
    /** Default configuration for collision detection. */
    ::btDefaultCollisionConfiguration collision_config_;

    /** Calculations for handling collision pairs. */
    ::btCollisionDispatcher collision_dispatcher_;

    /** Broadphase implementation. */
    ::btDbvtBroadphase broadphase_;

    /** Constrain solver. */
    ::btSequentialImpulseConstraintSolver solver_;

    /** Callback for handling ghost collision pairs. */
    ::btGhostPairCallback ghost_pair_callback_;

    /** Simulated physics world. */
    ::btDiscreteDynamicsWorld world_;

    /** Collection of collision states. */
    std::vector<std::unique_ptr<::btBoxShape>> shapes_;

    /** Collection of motion states. */
    std::vector<std::unique_ptr<::btDefaultMotionState>> motion_states_;

    /** Collection of created rigid bodies. */
    std::vector<std::unique_ptr<::btRigidBody>> rigid_bodies_;

    /** Optional object for handling debug information. */
    DebugDrawer *debug_drawer_;

    /** Map of rigid bodies to their registered collision callbacks. */
    std::unordered_map<::btRigidBody *, std::function<bool()>> collision_callbacks_;
};

}