#include "entry.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionDispatch/btCollisionWorld.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btMotionState.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreSimpleRenderable.h"
#include "SDL.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

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

/**
 * Class for registering bullet collisions.
 */
class CollisionCallback : public ::btCollisionWorld::ContactResultCallback
{
  public:
    /**
     * Callback to register a collision.

     * @returns
     *   Unused by bullet, we always return 0.
     */
    btScalar addSingleResult(
        btManifoldPoint &,
        const btCollisionObjectWrapper *,
        int,
        int,
        const btCollisionObjectWrapper *,
        int,
        int) override
    {
        has_collided_ = true;
        return 0;
    }

    /**
     * Check if a collision has been registered.
     *
     * @returns
     *   True if a collision has happened, false otherwise.
     */
    bool has_collided() const
    {
        return has_collided_;
    }

  private:
    /** Flag indicating if a collision has happened. */
    bool has_collided_ = false;
};

/**
 * A class which draws bullet debug information using ogre.
 *
 * Internally this class managed an ogre ManualObject in which we will add all the debug lines from bullet
 */
class DebugDraw : public ::btIDebugDraw
{
  public:
    /**
     * Construct a new DebugDraw class.
     *
     * @param scene_manager
     *   The scene manager to add debug graphics to.
     */
    DebugDraw(::Ogre::SceneManager *scene_manager)
        : lines_("physics_lines")
    {
        // tell ogre we will be updating the geometry of the object regularly
        lines_.setDynamic(true);

        // attach the manual object to the scene
        scene_manager->getRootSceneNode()->attachObject(&lines_);

        // setup a basic material which doesn't cast shadows and isn't effected by lights
        auto mtl = ::Ogre::MaterialManager::getSingleton().getDefaultSettings()->clone("debug_line_material");
        mtl->setReceiveShadows(false);
        mtl->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);
        mtl->setDepthBias(0.1, 0);

        auto *tu = mtl->getTechnique(0)->getPass(0)->createTextureUnitState();
        tu->setColourOperationEx(::Ogre::LBX_SOURCE1, ::Ogre::LBS_DIFFUSE);
        mtl->getTechnique(0)->setLightingEnabled(false);

        // tell ogre we will now start defining geometry
        lines_.begin(mtl->getName(), ::Ogre::RenderOperation::OT_LINE_LIST);
    }

    /**
     * Draw a debug line, the bullet engine calls this for each line it wants rendered.
     *
     * @param from
     *   The position in world space of the start of the line.
     *
     * @param to
     *   The position in world space of the end of the line.
     *
     * @param colour
     *   The colour of the line.
     */
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &colour) override
    {
        // convert bullet colour to ogre
        ::Ogre::ColourValue c{colour.getX(), colour.getY(), colour.getZ()};
        c.saturate();

        // add start and end points of line

        lines_.position({from.getX(), from.getY(), from.getZ()});
        lines_.colour(c);
        lines_.textureCoord(::Ogre::Vector2::ZERO);
        lines_.normal(::Ogre::Vector3::ZERO);

        lines_.position({to.getX(), to.getY(), to.getZ()});
        lines_.colour(c);
        lines_.textureCoord(::Ogre::Vector2::ZERO);
        lines_.normal(::Ogre::Vector3::ZERO);
    }

    // these must be defined but we do not use them

    void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) override
    {
        throw std::runtime_error("unimplemented");
    }

    void reportErrorWarning(const char *) override
    {
        throw std::runtime_error("unimplemented");
    }

    void draw3dText(const btVector3 &, const char *) override
    {
        throw std::runtime_error("unimplemented");
    }

    void setDebugMode(int) override
    {
    }

    /**
     * Get the bullet debug mode.
     *
     * @returns
     *   Always returns DBG_DrawWireFrame as this only requires us to implement drawLine.
     */
    int getDebugMode() const override
    {
        return ::btIDebugDraw::DBG_DrawWireframe;
    }

    /**
     * The engine should call this at the start of the frame.
     */
    void frame_start()
    {
        // start of the frame so end the manual object so it can be rendered
        lines_.end();
    }

    /**
     * The engine should call this at the end of the frame.
     */
    void frame_end()
    {
        // we are about to start a new frame so tell ogre we want to start defining geometry again
        lines_.beginUpdate(0);
    }

  private:
    /** The ogre manual object to store rendered lines. */
    Ogre::ManualObject lines_;
};

/**
 * Custom extension of ApplicationContext so we can perform actions on frame start and end.
 */
class ApplicationContext : public ::OgreBites::ApplicationContext
{
  public:
    /**
     * Construct a new ApplicationContext object.
     *
     * @param app_name
     *   Name of the app.
     */
    ApplicationContext(const ::Ogre::String &app_name)
        : ::OgreBites::ApplicationContext(app_name)
        , scene_manager_(nullptr)
        , debug_draw_(nullptr)
        , update_(nullptr)
    {
        initApp();
        scene_manager_ = getRoot()->createSceneManager();

        debug_draw_ = std::make_unique<DebugDraw>(scene_manager_);
    }

    ~ApplicationContext()
    {
        // note that the debug_draw object must be released *before* the app is closed, this is because the ManualObject
        // inside the debug draw can only be destroyed from a running app
        debug_draw_.reset();
        closeApp();
    }

    bool frameStarted(const ::Ogre::FrameEvent &evt) override
    {
        if (update_)
        {
            update_();
        }

        // call the base function
        const auto res = ::OgreBites::ApplicationContext::frameStarted(evt);

        debug_draw_->frame_start();
        return res;
    }

    bool frameEnded(const ::Ogre::FrameEvent &evt) override
    {
        // call the base function
        const auto res = ::OgreBites::ApplicationContext::frameEnded(evt);

        debug_draw_->frame_end();
        return res;
    }

    ::Ogre::SceneManager *scene_manager() const
    {
        return scene_manager_;
    }

    DebugDraw *debug_draw() const
    {
        return debug_draw_.get();
    }

    void set_update(std::function<void()> update)
    {
        update_ = update;
    }

  private:
    /** SceneManager to be used for the app. */
    ::Ogre::SceneManager *scene_manager_;

    /** DebugDraw object for use with physics. */
    std::unique_ptr<DebugDraw> debug_draw_;

    /** Optional update function. */
    std::function<void()> update_;
};

void entry()
{
    // use ogre bites to setup a simple application
    ApplicationContext ctx{"bab_sample"};
    auto *scene_manager = ctx.scene_manager();

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

    // add a box

    const auto material = ::Ogre::MaterialManager::getSingleton().create("MyMaterial", "bab");
    const auto texture = ::Ogre::TextureManager::getSingleton().load("box.png", "bab");
    auto *pass = material->getTechnique(0)->getPass(0);
    auto *tex_unit = pass->createTextureUnitState();
    tex_unit->setTextureName(texture->getName());

    auto *box_entity = scene_manager->createEntity("cube.mesh");
    box_entity->setMaterial(material);
    auto *box_node = scene_manager->getRootSceneNode()->createChildSceneNode();
    box_node->attachObject(box_entity);
    box_node->setScale(0.2f, 0.2f, 0.2f);

    // setup bullet physics

    auto collision_config = ::btDefaultCollisionConfiguration{};
    auto collision_dispatcher = ::btCollisionDispatcher{&collision_config};
    auto broadphase = ::btDbvtBroadphase{};
    auto solver = ::btSequentialImpulseConstraintSolver{};
    auto world = ::btDiscreteDynamicsWorld{&collision_dispatcher, &broadphase, &solver, &collision_config};
    world.setDebugDrawer(ctx.debug_draw());

    auto ghost_pair_callback = ::btGhostPairCallback{};
    broadphase.getOverlappingPairCache()->setInternalGhostPairCallback(&ghost_pair_callback);
    world.setGravity({-1.0f, -10.0f, 0.0f});

    // base starting transform object
    ::btTransform start_transform;
    start_transform.setIdentity();

    // add a static rigid body for the ground (mass of 0 means static in bullet)
    ::btBoxShape ground_shape{::btVector3{750.0f, 0.5f, 750.0f}};
    ::btDefaultMotionState ground_motion_state{start_transform};
    ::btRigidBody::btRigidBodyConstructionInfo ground_rb_info{
        0.0f, &ground_motion_state, &ground_shape, {0.0f, 0.0f, 0.0f}};

    ::btRigidBody ground_rigid_body{ground_rb_info};
    ground_rigid_body.setFriction(1.0f);
    world.addRigidBody(&ground_rigid_body);

    // add a dynamic rigid body of a box, starting in the air so it falls

    start_transform.setOrigin({100.0f, 100.0f, 0.0f});

    ::btVector3 box_local_inertia{0.0f, 0.0f, 0.0f};
    ::btBoxShape box_shape{{10.0f, 10.0f, 10.0f}};
    box_shape.calculateLocalInertia(10.0f, box_local_inertia);
    ::btDefaultMotionState box_motion_state{start_transform};
    ::btRigidBody::btRigidBodyConstructionInfo box_rb_info{10.0f, &box_motion_state, &box_shape, box_local_inertia};

    ::btRigidBody box_rigid_body{box_rb_info};
    box_rigid_body.setFriction(1.0f);
    world.addRigidBody(&box_rigid_body);

    // setup SDL for just audio
    ::SDL_Init(SDL_INIT_AUDIO);
    ::SDL_AudioSpec wavSpec;
    std::uint32_t wavLength;
    std::uint8_t *wavBuffer;

    // load a crash sound
    assert(::SDL_LoadWAV("assets/box-crash.wav", &wavSpec, &wavBuffer, &wavLength) != nullptr);
    const auto deviceId = ::SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    // add an update function which updates physics
    ctx.set_update([&] {
        world.stepSimulation(0.16);
        world.debugDrawWorld();

        const auto physics_transform = box_rigid_body.getWorldTransform();
        const auto origin = physics_transform.getOrigin();
        const auto rotation = physics_transform.getRotation();
        box_node->setPosition(origin.getX(), origin.getY(), origin.getZ());
        box_node->setOrientation(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());

        static auto single_collision = false;

        // only handle collision once
        if (!single_collision)
        {
            CollisionCallback callback{};
            world.contactPairTest(&ground_rigid_body, &box_rigid_body, callback);

            // if collision happens then play sound
            if (callback.has_collided())
            {
                int success = ::SDL_QueueAudio(deviceId, wavBuffer, wavLength);
                ::SDL_PauseAudioDevice(deviceId, 0);
                single_collision = true;
            }
        }
    });

    // create and register our custom key listener
    KeyHandler key_handler{};
    ctx.addInputListener(&key_handler);

    ctx.getRoot()->startRendering();

    // remove rigid bodies befor the physics engine is destroyed
    world.removeRigidBody(&box_rigid_body);
    world.removeRigidBody(&ground_rigid_body);
}

}
