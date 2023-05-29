#include "debug_drawer.h"

#include <stdexcept>

#include "colour.h"
#include "manual_object.h"
#include "vector3.h"

#include "LinearMath/btIDebugDraw.h"

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
bab::Colour to_engine(const ::btVector3 &vector, float alpha)
{
    return bab::Colour{vector.getX(), vector.getY(), vector.getZ(), alpha};
}

}

namespace bab
{

DebugDrawer::DebugDrawer(ManualObject &lines)
    : lines_(lines)
{
}

void DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &colour)
{
    lines_.add_line(to_engine(from), to_engine(to), to_engine(colour, 1.0f));
}

void DebugDrawer::drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &)
{
    throw std::runtime_error("unimplemented");
}

void DebugDrawer::reportErrorWarning(const char *)
{
    throw std::runtime_error("unimplemented");
}

void DebugDrawer::draw3dText(const btVector3 &, const char *)
{
    throw std::runtime_error("unimplemented");
}

void DebugDrawer::setDebugMode(int)
{
}

int DebugDrawer::getDebugMode() const
{
    return ::btIDebugDraw::DBG_DrawWireframe;
}

}
