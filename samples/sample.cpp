#include "entry.h"

#include <functional>
#include <iostream>
#include <numbers>

#include "audio_clip.h"
#include "audio_manager.h"
#include "colour.h"
#include "degree.h"
#include "graphics_manager.h"
#include "physics_manager.h"
#include "quaternion.h"
#include "radian.h"
#include "scene_manager.h"
#include "vector3.h"

int main()
{
    bab::GraphicsManager gm{};

    gm.set_sky_dome("Examples/CloudySky", 5.0f, 8.0f);
    gm.add_mesh("ninja.mesh", bab::Vector3::ZERO, {bab::Radian{std::numbers::pi_v<float>}, bab::Vector3::UNIT_Y}, true);
    gm.add_plane(1500.0f, 1500.0f, 20u, 20u, false, "Examples/Rockwall");
    gm.add_material("box_material", "box.png");
    gm.add_spot_light(
        {200.0f, 200.0f, 0.0f}, {-1.0f, -1.0f, 0.0f}, bab::Colour::Blue, bab::Degree{35.0f}, bab::Degree{50.0f});
    gm.add_directional_light({0.0f, -1.0f, 1.0f}, bab::Colour{0.4f, 0.0f, 0.0f});
    gm.add_point_light({0.0f, 150.0f, 250.0f}, bab::Colour::White * 0.3f);

    bab::PhysicsManager pm{};
    pm.add_static_rigid_body({750.0f, 0.0f, 750.0f}, bab::Vector3::ZERO);
    gm.register_frame_start_callback([&pm] { pm.update(); });

    bab::AudioManager am{};
    const auto *clip = am.load("assets/box-crash.wav");

    bab::SceneManager sm{gm, pm};
    sm.add_cube({10.0f, 200.0f, 10.0f}, 0.5f, "box_material", 10.0f, [clip] {
        clip->play();
        return true;
    });

    gm.start_rendering();

    return 0;
}