#pragma once

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
