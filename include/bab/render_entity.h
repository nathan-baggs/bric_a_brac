#pragma once

#include "quaternion.h"
#include "vector3.h"

namespace Ogre
{
class SceneNode;
}

namespace bab
{

/**
 * Class which wraps a renderable entity created by the GraphicsManager and allows it to be manipulated.
 */
class RenderEntity
{
  public:
    /**
     * Set the world position.
     *
     * @param position
     *   New world position.
     */
    void set_position(const Vector3 &position);

    /**
     * Set the world orientation.
     *
     * @param orientation
     *   New world orientation.
     */
    void set_orientation(const Quaternion &set_orientation);

  private:
    // allow GraphicsManager to construct this object
    friend class GraphicsManager;

    /**
     * Construct a new RenderEntity, private so only GraphicsManager can call.
     *
     * @param node
     *   The ogre scene node that holds this entity.
     */
    RenderEntity(::Ogre::SceneNode *node);

    /** Ogre scene node for this entity. */
    ::Ogre::SceneNode *node_;
};

}
