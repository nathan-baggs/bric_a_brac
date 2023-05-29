add_executable(bab_sample
    sample.cpp
)

target_link_libraries(bab_sample bab)

# list of ogre targets that create libraries we will need to copy to the sample build dir
set(BAB_OGRE_REQUIRED_LIBS
    $<$<NOT:$<PLATFORM_ID:Darwin>>:$<TARGET_FILE:RenderSystem_GL3Plus>>
    $<$<NOT:$<PLATFORM_ID:Darwin>>:$<TARGET_FILE:RenderSystem_GLES2>>
    $<$<PLATFORM_ID:Darwin>:$<TARGET_FILE:RenderSystem_Metal>>
    $<$<PLATFORM_ID:Windows>:$<TARGET_FILE:Codec_Assimp>>
    $<$<PLATFORM_ID:Windows>:$<TARGET_FILE:RenderSystem_Direct3D11>>
    $<TARGET_FILE:Codec_STBI>
    $<TARGET_FILE:OgreBites>
    $<TARGET_FILE:OgreMain>
    $<TARGET_FILE:OgreOverlay>
    $<TARGET_FILE:OgrePaging>
    $<TARGET_FILE:OgreRTShaderSystem>
    $<TARGET_FILE:OgreTerrain>
    $<TARGET_FILE:Plugin_BSPSceneManager>
    $<TARGET_FILE:Plugin_DotScene>
    $<TARGET_FILE:Plugin_OctreeSceneManager>
    $<TARGET_FILE:Plugin_OctreeZone>
    $<TARGET_FILE:Plugin_PCZSceneManager>
    $<TARGET_FILE:Plugin_ParticleFX>
    $<TARGET_FILE:RenderSystem_GL>
)

# list of ogre generated files we will need to copy to the sample build dir
# for some reason they are in a slightly different place on macOS
set(BAB_OGRE_REQUIRED_FILES
    $<IF:$<PLATFORM_ID:Darwin>,$<TARGET_FILE_DIR:SampleBrowser>/../Resources/plugins.cfg,$<TARGET_FILE_DIR:SampleBrowser>/plugins.cfg>
    $<IF:$<PLATFORM_ID:Darwin>,$<TARGET_FILE_DIR:SampleBrowser>/../Resources/resources.cfg,$<TARGET_FILE_DIR:SampleBrowser>/resources.cfg>
)

# create a custom target which copies everything we need into the sample build dir
add_custom_target(copy_ogre_deps
    ALL
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:OgreMain> ${BAB_OGRE_REQUIRED_LIBS} $<TARGET_FILE_DIR:bab_sample>
    COMMAND ${CMAKE_COMMAND} -E copy ${BAB_OGRE_REQUIRED_FILES} $<TARGET_FILE_DIR:bab_sample>
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:bab_sample>/assets
    DEPENDS OgreMain RenderSystem_GL
)

# windows specific setup
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # list of libraries ogre depends on we will need to copy to the sample build dir
    file(GLOB BAB_OGRE_DEPENDENCIES ${ogre_BINARY_DIR}/Dependencies/bin/*.dll)

    add_custom_target(copy_ogre_deps_windows
        ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${BAB_OGRE_DEPENDENCIES} $<TARGET_FILE_DIR:bab_sample>
        DEPENDS OgreMain RenderSystem_GL
    )
endif()
