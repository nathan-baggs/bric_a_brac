#include "scene_manager.h"

#include <string>

#include "graphics_manager.h"
#include "physics_manager.h"
#include "render_entity.h"
#include "rigid_body.h"
#include "vector3.h"

namespace bab
{

SceneManager::SceneManager(GraphicsManager &gm, PhysicsManager &pm)
    : gm_(gm)
    , pm_(pm)
    , physics_debug_object_(gm.add_manual_object())
    , debug_drawer_(physics_debug_object_)
    , entities_()
{
    pm_.set_debug_drawer(&debug_drawer_);

    // ensure the manual object for drawing physics debug information is correctly stopped/started at frame end/begin
    gm_.register_frame_start_callback([this] { physics_debug_object_.end(); });
    gm_.register_frame_end_callback([this] { physics_debug_object_.begin(); });

    // synchronise the rigid body position/orientation with its associated render entity
    gm_.register_frame_start_callback([this] {
        for (auto &[render_entity, rigid_body] : entities_)
        {
            render_entity.set_position(rigid_body.position());
            render_entity.set_orientation(rigid_body.orientation());
        }
    });
}

void SceneManager::add_cube(const Vector3 &position, float scale, const std::string &material_name, float mass)
{
    const auto bullet_ogre_scale_factor = 50.0f;

    entities_.emplace_back(
        gm_.add_cube(position, scale, "box_material"),
        pm_.add_dynamic_rigid_body(Vector3{scale, scale, scale} * bullet_ogre_scale_factor, position, mass));
}

}