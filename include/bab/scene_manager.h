#pragma once

#include "debug_drawer.h"
#include "manual_object.h"

namespace bab
{

class GraphicsManager;
class PhysicsManager;

/**
 * Class to handle the games scene.
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

  private:
    /** The GraphicsManager for the engine. */
    GraphicsManager &gm_;

    /** The PhysicsManager for the engine. */
    PhysicsManager &pm_;

    /** Manual object for drawing debug lines of the physics engine. */
    ManualObject physics_debug_object_;

    /** Object used for processing physics debug information. */
    DebugDrawer debug_drawer_;
};

}
