#include "entry.h"

#include "Ogre.h"
#include "OgreApplicationContext.h"

namespace bab
{

/**
 * Class handling key events from ogre.
 */
class KeyHandler : public ::OgreBites::InputListener
{
  public:
    /**
     * Handle key pressed event.
     *
     * @evt
     *   Key event.
     */
    bool keyPressed(const ::OgreBites::KeyboardEvent &evt) override
    {
        if (evt.keysym.sym == ::OgreBites::SDLK_ESCAPE)
        {
            ::Ogre::Root::getSingleton().queueEndRendering();
        }

        return true;
    }
};

void entry()
{
    // use ogre bites to setup a simple application
    ::OgreBites::ApplicationContext ctx{"bab_sample"};
    ctx.initApp();

    auto *root = ctx.getRoot();
    auto *scene_manager = root->createSceneManager();

    auto *shader_gen = ::Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    shader_gen->addSceneManager(scene_manager);

    // add our resource location and ensure it's loaded
    ::Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./assets", "FileSystem", "bab");
    ::Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("bab");

    // simple skydome setup using the builtin ogre assets
    scene_manager->setSkyDome(true, "Examples/CloudySky", 5.0, 8.0);

    // set scene properties
    scene_manager->setAmbientLight(::Ogre::ColourValue{0.2, 0.2, 0.2});
    scene_manager->setShadowTechnique(::Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_ADDITIVE);

    // create a camera
    auto *camera = scene_manager->createCamera("main_camera");
    camera->setNearClipDistance(5.0);

    auto *camera_node = scene_manager->getRootSceneNode()->createChildSceneNode();
    camera_node->setPosition(0.0, 200.0, 500.0);
    camera_node->lookAt(::Ogre::Vector3{0, 100.0, 0}, ::Ogre::Node::TransformSpace::TS_WORLD);
    camera_node->attachObject(camera);

    // setup viewport
    auto *vp = ctx.getRenderWindow()->addViewport(camera);
    vp->setBackgroundColour(::Ogre::ColourValue{0.0, 0.0, 0.0});
    camera->setAspectRatio(::Ogre::Real(vp->getActualWidth()) / ::Ogre::Real(vp->getActualHeight()));

    // load ninja model and ensure it casts shadows
    auto *ninja_entity = scene_manager->createEntity("ninja.mesh");
    ninja_entity->setCastShadows(true);

    // create a scene node for the ninja
    auto *ninja_node = scene_manager->getRootSceneNode()->createChildSceneNode();
    ninja_node->attachObject(ninja_entity);
    ninja_node->setOrientation(::Ogre::Quaternion{::Ogre::Radian{::Ogre::Math::PI}, ::Ogre::Vector3{0.0, 1.0, 0.0}});

    // create a plane for the ground
    ::Ogre::Plane plane{::Ogre::Vector3::UNIT_Y, 0.0};
    ::Ogre::MeshManager::getSingleton().createPlane(
        "ground", ::Ogre::RGN_DEFAULT, plane, 1500.0, 1500.0, 20, 20, true, 1, 5, 5.0, ::Ogre::Vector3::UNIT_Z);

    // create an entity for the ground
    auto *ground_entity = scene_manager->createEntity("ground");
    scene_manager->getRootSceneNode()->createChildSceneNode()->attachObject(ground_entity);
    ground_entity->setCastShadows(false);
    ground_entity->setMaterialName("Examples/Rockwall");

    // create some lights

    auto *spot_light = scene_manager->createLight("spot_light");
    spot_light->setDiffuseColour(0, 0, 1.0);
    spot_light->setSpecularColour(0, 0, 1.0);
    spot_light->setType(::Ogre::Light::LT_SPOTLIGHT);
    spot_light->setSpotlightRange(::Ogre::Degree(35), ::Ogre::Degree(50));

    auto *spot_light_node = scene_manager->getRootSceneNode()->createChildSceneNode();
    spot_light_node->attachObject(spot_light);
    spot_light_node->setDirection(-1, -1, 0);
    spot_light_node->setPosition(::Ogre::Vector3(200, 200, 0));

    auto *directional_light = scene_manager->createLight("directional_light");
    directional_light->setType(::Ogre::Light::LT_DIRECTIONAL);
    directional_light->setDiffuseColour(::Ogre::ColourValue(0.4, 0, 0));
    directional_light->setSpecularColour(::Ogre::ColourValue(0.4, 0, 0));

    auto *directional_light_node = scene_manager->getRootSceneNode()->createChildSceneNode();
    directional_light_node->attachObject(directional_light);
    directional_light_node->setDirection(::Ogre::Vector3(0, -1, 1));

    auto *point_light = scene_manager->createLight("point_light");
    point_light->setType(::Ogre::Light::LT_POINT);
    point_light->setDiffuseColour(0.3, 0.3, 0.3);
    point_light->setSpecularColour(0.3, 0.3, 0.3);

    auto *point_light_node = scene_manager->getRootSceneNode()->createChildSceneNode();
    point_light_node->attachObject(point_light);
    point_light_node->setPosition(::Ogre::Vector3(0, 150, 250));

    // create and register our custom key listener
    KeyHandler key_handler{};
    ctx.addInputListener(&key_handler);

    ctx.getRoot()->startRendering();

    ctx.closeApp();
}

}
