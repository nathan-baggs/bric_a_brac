#pragma once

#include "manual_object.h"

#include "LinearMath/btIDebugDraw.h"

namespace bab
{

/**
 * A class which draws bullet debug information using ogre.
 */
class DebugDrawer : public ::btIDebugDraw
{
  public:
    /**
     * Construct a new DebugDrawer class.
     *
     * @param lines
     *   The manual object to render lines with.
     */
    DebugDrawer(ManualObject &lines);

    /**
     * Draw a debug line, the bullet engine calls this for each line it wants rendered.
     *
     * @param from
     *   The position in world space of the start of the line.
     *
     * @param to
     *   The position in world space of the end of the line.
     *
     * @param colour
     *   The colour of the line.
     */
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &colour) override;

    // these must be defined but we do not use them

    void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) override;

    void reportErrorWarning(const char *) override;

    void draw3dText(const btVector3 &, const char *) override;

    void setDebugMode(int) override;

    /**
     * Get the bullet debug mode.
     *
     * @returns
     *   Always returns DBG_DrawWireFrame as this only requires us to implement drawLine.
     */
    int getDebugMode() const override;

  private:
    /** ManualObject used for rendering lines. */
    ManualObject &lines_;
};

}