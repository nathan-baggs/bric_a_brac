#pragma once

#include <string>

#include "quaternion.h"
#include "vector3.h"

#include "Ogre.h"
#include "OgreApplicationContext.h"

namespace bab
{

/**
 * Class to handle all things related to rendering (and by extension windowing).
 */
class GraphicsManager : private ::OgreBites::ApplicationContext, ::OgreBites::InputListener
{
  public:
    /**
     * Construct a new GraphicsManager.
     */
    GraphicsManager();

    /**
     * GraphicsManager specific cleanup.
     */
    ~GraphicsManager() override;

    /**
     * Add a mesh to the scene.
     *
     * @param mesh_name
     *   Name of mesh to load, must exist in a resource location.
     *
     * @param position
     *   The world position of the mesh.
     *
     * @param orientation
     *   The world orientation of the mesh.
     *
     * @param casts_shadows
     *   Whether the model can cast a shadow.
     */
    void add_mesh(
        const std::string &mesh_name,
        const Vector3 &position,
        const Quaternion &orientation,
        bool casts_shadows);

    /**
     * Add a plane (XZ) to the scene.
     *
     * @param width
     *   The width of the mesh (x axis).
     *
     * @param height
     *   The height of the mesh (z axis).
     *
     * @param x_segments
     *   How many segments make up the plane in the x direction.
     *
     * @param z_segments
     *   How many segments make up the plane in the z direction.
     *
     * @param casts_shadows
     *   Whether the model can cast a shadow.
     *
     * @param material_name
     *   The name of the material to add to the plane.
     */
    void add_plane(
        float width,
        float height,
        std::uint32_t x_segments,
        std::uint32_t z_segments,
        bool casts_shadows,
        const std::string &material_name);

    /**
     * Add a new named material to the engine which simply applies a texture.
     *
     * @param name
     *   The name for the new material.
     *
     * @param texture_name
     *   Name of texture to load, must exist in a resource location.
     */
    void add_material(const std::string &name, const std::string &texture_name);

    /**
     * Add a new cube to the scene.
     *
     * @param position
     *   World position of the cube.
     *
     * @param scale
     *   The scale of the cube (x, y and z direction).
     *
     * @param material_name
     *   The name of the material to add.
     */
    void add_cube(const Vector3 &position, float scale, const std::string &material_name);

    /**
     * Block and start the render loop.
     */
    void start_rendering();

  private:
    /**
     * Called by Ogre when the frame starts.
     *
     * @param evt
     *   Ogre frame event object.
     *
     * @returns
     *   True if rendering should continue, false if it should abort.
     */
    bool frameStarted(const ::Ogre::FrameEvent &evt) override;

    /**
     * Called by Ogre when the frame ends.
     *
     * @param evt
     *   Ogre frame event object.
     *
     * @returns
     *   True if rendering should continue, false if it should abort.
     */
    bool frameEnded(const ::Ogre::FrameEvent &evt) override;

    /**
     * Handle key pressed event.
     *
     * @param evt
     *   Ogre keyboard event object.
     *
     * @returns
     *   True if event should be consumed and not propagated further, otherwise false.
     */
    bool keyPressed(const ::OgreBites::KeyboardEvent &evt) override;

    /** Ogre scene manager object. */
    ::Ogre::SceneManager *scene_manager_;
};

}
