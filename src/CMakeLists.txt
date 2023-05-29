add_library(bab STATIC
    debug_drawer.cpp
    entry.cpp
    graphics_manager.cpp
    manual_object.cpp
    physics_manager.cpp
)

add_library(bab::bab ALIAS bab)

target_compile_features(bab PUBLIC cxx_std_23)

target_link_libraries(bab PUBLIC OgreMain OgreBites BulletDynamics BulletCollision LinearMath SDL2-static)

# bullet has an old-style cmake file so we need to manually add the includes
target_include_directories(
    bab SYSTEM
    PUBLIC ${bullet_SOURCE_DIR}/src)

target_include_directories(
    bab
    PUBLIC ${PROJECT_SOURCE_DIR}/include/bab
)
