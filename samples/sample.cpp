#include "entry.h"

#include <numbers>

#include "graphics_manager.h"
#include "quaternion.h"
#include "radian.h"
#include "vector3.h"

int main()
{
    bab::GraphicsManager gm{};

    gm.add_mesh("ninja.mesh", bab::Vector3::ZERO, {bab::Radian{std::numbers::pi_v<float>}, bab::Vector3::UNIT_Y}, true);
    gm.add_plane(1500.0f, 1500.0f, 20u, 20u, false, "Examples/Rockwall");

    gm.add_material("box_material", "box.png");
    gm.add_cube(bab::Vector3::ZERO, 0.2f, "box_material");

    gm.start_rendering();

    return 0;
}