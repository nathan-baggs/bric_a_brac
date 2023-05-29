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

    // load ninja model and ensure it casts shadows
    auto *ninja_entity = scene_manager_->createEntity("ninja.mesh");
    ninja_entity->setCastShadows(true);

    // create a scene node for the ninja
    auto *ninja_node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    ninja_node->attachObject(ninja_entity);
    ninja_node->setOrientation(::Ogre::Quaternion{::Ogre::Radian{::Ogre::Math::PI}, ::Ogre::Vector3{0.0, 1.0, 0.0}});

    // create a plane for the ground
    ::Ogre::Plane plane{::Ogre::Vector3::UNIT_Y, 0.0};
    ::Ogre::MeshManager::getSingleton().createPlane(
        "ground", ::Ogre::RGN_DEFAULT, plane, 1500.0, 1500.0, 20, 20, true, 1, 5, 5.0, ::Ogre::Vector3::UNIT_Z);

    // create an entity for the ground
    auto *ground_entity = scene_manager_->createEntity("ground");
    scene_manager_->getRootSceneNode()->createChildSceneNode()->attachObject(ground_entity);
    ground_entity->setCastShadows(false);
    ground_entity->setMaterialName("Examples/Rockwall");

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

    // add a box

    const auto material = ::Ogre::MaterialManager::getSingleton().create("MyMaterial", "bab");
    const auto texture = ::Ogre::TextureManager::getSingleton().load("box.png", "bab");
    auto *pass = material->getTechnique(0)->getPass(0);
    auto *tex_unit = pass->createTextureUnitState();
    tex_unit->setTextureName(texture->getName());

    auto *box_entity = scene_manager_->createEntity("cube.mesh");
    box_entity->setMaterial(material);
    auto *box_node = scene_manager_->getRootSceneNode()->createChildSceneNode();
    box_node->attachObject(box_entity);
    box_node->setScale(0.2f, 0.2f, 0.2f);

    addInputListener(this);
}

GraphicsManager::~GraphicsManager()
{
    closeApp();
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