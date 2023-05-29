#pragma once

#include <memory>

#include "colour.h"
#include "vector3.h"

#include "Ogre.h"

namespace bab
{

/**
 * A renderable object that can be modified after it's creation. Currently just renders a line list and supports adding
 * more lines.
 *
 * Before the object cna be rendered end() must be called, then begin() must be called before editing again.
 */
class ManualObject
{
  public:
    ~ManualObject() = default;
    ManualObject(const ManualObject &) = default;
    ManualObject &operator=(const ManualObject &) = default;
    ManualObject(ManualObject &&) = default;
    ManualObject &operator=(ManualObject &&) = default;

    /**
     * Add a new line to be rendered.
     *
     * @param start
     *   World position of the start of the line.
     *
     * @param end
     *   World position of the end of the line.
     *
     * @param colour
     *   The colour of the line.
     */
    void add_line(const Vector3 &start, const Vector3 &end, const Colour &colour);

    /**
     * Begin modification of the object. Clears all previous lines.
     */
    void begin();

    /**
     * Ends modification of the object so it can be rendered.
     */
    void end();

  private:
    // allow GraphicsManager to construct this object
    friend class GraphicsManager;

    /**
     * Construct a new ManualObject, private so only GraphicsManager can call.
     *
     * @param lines
     *   The ogre manual object used to store rendered lines.
     */
    ManualObject(std::unique_ptr<::Ogre::ManualObject> lines);

    /** The ogre manual object to store rendered lines. */
    std::unique_ptr<::Ogre::ManualObject> lines_;
};

}