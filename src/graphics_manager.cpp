#include "graphics_manager.h"

#include "Ogre.h"
#include "OgreApplicationContext.h"

namespace bab
{

GraphicsManager::GraphicsManager()
    : scene_manager_(nullptr)
{
    initApp();
    scene_manager_ = getRoot()->createSceneManager();

    auto *shader_gen = ::Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shader_gen->addSceneManager(scene_manager_);

    // add our resource location and ensure it's loaded
    ::Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./assets", "FileSystem", "bab");
    ::Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("bab");

    // simple skydome setup using the builtin ogre assets
    scene_manager_->setSkyDome(true, "Examples/CloudySky", 5.0, 8.0);

    // set scene properties
    scene_manager_->setAmbientLight(::Ogre::ColourValue{0.2, 0.2, 0.2});
    scene_manager_->setShadowTechnique(::Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

    // create a camera
    auto *camera = scene_manager_->createCamera("main_camera");
    camera->setNearClipDistance(5.0);

    auto *camera_node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    camera_node->setPosition(0.0, 200.0, 500.0);
    camera_node->lookAt(::Ogre::Vector3{0, 100.0, 0}, ::Ogre::Node::TransformSpace::TS_WORLD);
    camera_node->attachObject(camera);

    // setup viewport
    auto *vp = getRenderWindow()->addViewport(camera);
    vp->setBackgroundColour(::Ogre::ColourValue{0.0, 0.0, 0.0});
    camera->setAspectRatio(::Ogre::Real(vp->getActualWidth()) / ::Ogre::Real(vp->getActualHeight()));

    // create some lights

    auto *spot_light = scene_manager_->createLight("spot_light");
    spot_light->setDiffuseColour(0, 0, 1.0);
    spot_light->setSpecularColour(0, 0, 1.0);
    spot_light->setType(::Ogre::Light::LT_SPOTLIGHT);
    spot_light->setSpotlightRange(::Ogre::Degree(35), ::Ogre::Degree(50));

    auto *spot_light_node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    spot_light_node->attachObject(spot_light);
    spot_light_node->setDirection(-1, -1, 0);
    spot_light_node->setPosition(::Ogre::Vector3(200, 200, 0));

    auto *directional_light = scene_manager_->createLight("directional_light");
    directional_light->setType(::Ogre::Light::LT_DIRECTIONAL);
    directional_light->setDiffuseColour(::Ogre::ColourValue(0.4, 0, 0));
    directional_light->setSpecularColour(::Ogre::ColourValue(0.4, 0, 0));

    auto *directional_light_node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    directional_light_node->attachObject(directional_light);
    directional_light_node->setDirection(::Ogre::Vector3(0, -1, 1));

    auto *point_light = scene_manager_->createLight("point_light");
    point_light->setType(::Ogre::Light::LT_POINT);
    point_light->setDiffuseColour(0.3, 0.3, 0.3);
    point_light->setSpecularColour(0.3, 0.3, 0.3);

    auto *point_light_node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    point_light_node->attachObject(point_light);
    point_light_node->setPosition(::Ogre::Vector3(0, 150, 250));

    addInputListener(this);
}

GraphicsManager::~GraphicsManager()
{
    closeApp();
}

void GraphicsManager::add_mesh(
    const std::string &mesh_name,
    const Vector3 &position,
    const Quaternion &orientation,
    bool casts_shadows)
{
    auto *entity = scene_manager_->createEntity(mesh_name);
    entity->setCastShadows(casts_shadows);

    auto *node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    node->attachObject(entity);
    node->setPosition(position);
    node->setOrientation(orientation);
}

void GraphicsManager::add_plane(
    float width,
    float height,
    std::uint32_t x_segments,
    std::uint32_t z_segments,
    bool casts_shadows,
    const std::string &material_name)
{
    static auto plane_counter = 0u;
    std::string name = "plane" + std::to_string(plane_counter++);

    ::Ogre::Plane plane{::Ogre::Vector3::UNIT_Y, 0.0};
    ::Ogre::MeshManager::getSingleton().createPlane(
        name,
        ::Ogre::RGN_DEFAULT,
        plane,
        width,
        height,
        static_cast<int>(x_segments),
        static_cast<int>(z_segments),
        true,
        1,
        5,
        5.0,
        ::Ogre::Vector3::UNIT_Z);

    auto *entity = scene_manager_->createEntity(name);
    entity->setCastShadows(casts_shadows);
    entity->setMaterialName(material_name);

    scene_manager_->getRootSceneNode()->createChildSceneNode()->attachObject(entity);
}

void GraphicsManager::add_material(const std::string &name, const std::string &texture_name)
{
    const auto material = ::Ogre::MaterialManager::getSingleton().create(name, "bab");
    const auto texture = ::Ogre::TextureManager::getSingleton().load(texture_name, "bab");
    auto *pass = material->getTechnique(0)->getPass(0);
    auto *tex_unit = pass->createTextureUnitState();
    tex_unit->setTextureName(texture->getName());
}

void GraphicsManager::add_cube(const Vector3 &position, float scale, const std::string &material_name)
{
    auto *entity = scene_manager_->createEntity("cube.mesh");
    entity->setMaterialName(material_name);

    auto *node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    node->attachObject(entity);
    node->setScale(scale, scale, scale);
    node->setPosition(position);
}

void GraphicsManager::start_rendering()
{
    getRoot()->startRendering();
}

bool GraphicsManager::frameStarted(const ::Ogre::FrameEvent &evt)
{
    return ::OgreBites::ApplicationContext::frameStarted(evt);
}

bool GraphicsManager::frameEnded(const ::Ogre::FrameEvent &evt)
{
    return ::OgreBites::ApplicationContext::frameEnded(evt);
}

bool GraphicsManager::keyPressed(const ::OgreBites::KeyboardEvent &evt)
{
    if (evt.keysym.sym == ::OgreBites::SDLK_ESCAPE)
    {
        ::Ogre::Root::getSingleton().queueEndRendering();
    }

    return true;
}

}