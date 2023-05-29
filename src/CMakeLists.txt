add_library(bab STATIC
    entry.cpp
)

add_library(bab::bab ALIAS bab)

target_compile_features(bab PUBLIC cxx_std_23)

target_link_libraries(bab PUBLIC OgreMain OgreBites)

target_include_directories(
    bab
    PUBLIC ${PROJECT_SOURCE_DIR}/include/bab
)
