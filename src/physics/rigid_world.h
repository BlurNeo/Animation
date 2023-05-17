#pragma once

#include "btBulletDynamicsCommon.h"

class RigidWorld {
 public:
    RigidWorld() {
                ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
        collisionConfiguration = new btDefaultCollisionConfiguration();

        ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
        dispatcher = new btCollisionDispatcher(collisionConfiguration);

        ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
        overlappingPairCache = new btDbvtBroadphase();

        ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
        solver = new btSequentialImpulseConstraintSolver;

        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

        dynamicsWorld->setGravity(btVector3(0, -10, 0));
    }
    static RigidWorld* get() {
        static RigidWorld rigid_world;
        return &rigid_world;
    }

    ~RigidWorld() {
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
};