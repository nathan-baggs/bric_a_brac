#include "manual_object.h"

#include <memory>

#include "colour.h"
#include "vector3.h"

#include "Ogre.h"

namespace bab
{

ManualObject::ManualObject(std::unique_ptr<::Ogre::ManualObject> lines)
    : lines_(std::move(lines))
{
}

void ManualObject::add_line(const Vector3 &start, const Vector3 &end, const Colour &colour)
{
    lines_->position(start);
    lines_->colour(colour);
    lines_->textureCoord(::Ogre::Vector2::ZERO);
    lines_->normal(::Ogre::Vector3::ZERO);

    lines_->position(end);
    lines_->colour(colour);
    lines_->textureCoord(::Ogre::Vector2::ZERO);
    lines_->normal(::Ogre::Vector3::ZERO);
}

void ManualObject::begin()
{
    lines_->beginUpdate(0);
}

void ManualObject::end()
{
    lines_->end();
}

}