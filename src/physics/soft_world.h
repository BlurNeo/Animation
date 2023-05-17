#pragma once
#include "glm/glm.hpp"


class SoftWorld {
 public:
    SoftWorld() {
        collisionConfiguration = new btDefaultCollisionConfiguration();

        ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        dispatcher = new btCollisionDispatcher(collisionConfiguration);

        ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
        btVector3 worldAabbMin(-1000, -1000, -1000);
	    btVector3 worldAabbMax(1000, 1000, 1000);
        overlappingPairCache = new btAxisSweep3(worldAabbMin, worldAabbMax, maxProxies);

        ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        solver = new btSequentialImpulseConstraintSolver;

        // Create Soft Body Dynamics World
        dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
        
        m_softBodyWorldInfo = new btSoftBodyWorldInfo();
        m_softBodyWorldInfo->m_dispatcher = dispatcher;
        
    	m_softBodyWorldInfo->m_broadphase = overlappingPairCache;

	    m_softBodyWorldInfo->m_gravity.setValue(0, -10, 0);

	    m_softBodyWorldInfo->m_sparsesdf.Initialize();
        m_softBodyWorldInfo->m_sparsesdf.Reset();

        m_softBodyWorldInfo->air_density = (btScalar)1.2;
        m_softBodyWorldInfo->water_density = 0;
        m_softBodyWorldInfo->water_offset = 0;
        m_softBodyWorldInfo->water_normal = btVector3(0, 0, 0);
        m_softBodyWorldInfo->m_gravity.setValue(0, -10, 0);

        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }
    
    static SoftWorld* get() {
        static SoftWorld soft_world;
        return &soft_world;
    }

    ~SoftWorld() {
        //cleanup in the reverse order of creation/initialization

        ///-----cleanup_start-----

        //remove the rigidbodies from the dynamics world and delete them
        for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
        {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
            btRigidBody* body = btRigidBody::upcast(obj);
            if (body && body->getMotionState())
            {
                delete body->getMotionState();
            }
            dynamicsWorld->removeCollisionObject(obj);
            delete obj;
        }

        //delete collision shapes
        // for (int j = 0; j < collisionShapes.size(); j++)
        // {
        //     btCollisionShape* shape = collisionShapes[j];
        //     collisionShapes[j] = 0;
        //     delete shape;
        // }

        //delete dynamics world
        if (dynamicsWorld)
            delete dynamicsWorld;

        //delete solver
        if (solver)
            delete solver;

        //delete broadphase
        if (overlappingPairCache)
            delete overlappingPairCache;

        //delete dispatcher
        if (dispatcher)
            delete dispatcher;

        if (collisionConfiguration)
            delete collisionConfiguration;
    }

    void update(float delta) {
        dynamicsWorld->stepSimulation(1.f / 60.f, 10);
    }

    btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
    btCollisionDispatcher* dispatcher = nullptr;
    btBroadphaseInterface* overlappingPairCache = nullptr;
    btSequentialImpulseConstraintSolver* solver = nullptr;
    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    btSoftBodyWorldInfo *m_softBodyWorldInfo;
    const int maxProxies = 32766;
};