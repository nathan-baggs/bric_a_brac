#include "render_entity.h"

#include "quaternion.h"
#include "vector3.h"

#include "Ogre.h"

namespace bab
{

RenderEntity::RenderEntity(::Ogre::SceneNode *node)
    : node_(node)
{
}

void RenderEntity::set_position(const Vector3 &position)
{
    node_->setPosition(position);
}

void RenderEntity::set_orientation(const Quaternion &orientation)
{
    node_->setOrientation(orientation);
}

}
