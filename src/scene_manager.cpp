#include "scene_manager.h"

#include "graphics_manager.h"
#include "physics_manager.h"

namespace bab
{

SceneManager::SceneManager(GraphicsManager &gm, PhysicsManager &pm)
    : gm_(gm)
    , pm_(pm)
    , physics_debug_object_(gm.add_manual_object())
    , debug_drawer_(physics_debug_object_)
{
    pm_.set_debug_drawer(&debug_drawer_);

    // ensure the manual object for drawing physics debug information is correctly stopped/started at frame end/begin
    gm_.register_frame_start_callback([this] { physics_debug_object_.end(); });
    gm_.register_frame_end_callback([this] { physics_debug_object_.begin(); });
}

}