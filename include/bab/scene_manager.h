#pragma once

#include <functional>
#include <string>
#include <tuple>
#include <vector>

#include "debug_drawer.h"
#include "manual_object.h"
#include "render_entity.h"
#include "rigid_body.h"
#include "vector3.h"

namespace bab
{

class GraphicsManager;
class PhysicsManager;

/**
 * Class to handle the games scene. Can be used to add render entities with a physics component and automatically keep
 * them in synch.
 */
class SceneManager
{
  public:
    /**
     * Construct a new SceneManager.
     *
     * @param gm
     *   The GraphicsManager for the engine.
     *
     * @param pm
     *   The PhysicsManager for the engine.
     */
    SceneManager(GraphicsManager &gm, PhysicsManager &pm);

    /**
     * Add a renderable cube with a physics component, will automatically synch the position and orientation.Position
     *
     * @param position
     *   World position of the cube.
     *
     * @param scale
     *   The scale of the cube (x, y and z direction).
     *
     * @param material_name
     *   The name of the material to add.
     *
     * @param mass
     *   The mass of the rigid body.
     *
     * @param callback
     *   Optional callback to fire when the physics component collides.
     */
    void add_cube(
        const Vector3 &position,
        float scale,
        const std::string &material_name,
        float mass,
        std::function<bool()> callback = nullptr);

  private:
    /** The GraphicsManager for the engine. */
    GraphicsManager &gm_;

    /** The PhysicsManager for the engine. */
    PhysicsManager &pm_;

    /** Manual object for drawing debug lines of the physics engine. */
    ManualObject physics_debug_object_;

    /** Object used for processing physics debug information. */
    DebugDrawer debug_drawer_;

    /** Collection of added render entities and rigid bodies. */
    std::vector<std::tuple<RenderEntity, RigidBody>> entities_;
};

}
