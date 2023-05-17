#pragma once
#include "LinearMath/btTransform.h"
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "glm/glm.hpp"

class GravitySoftBody {
 public:
    GravitySoftBody(btDiscreteDynamicsWorld* world, btSoftBodyWorldInfo *softBodyWorldInfo) :
            dynamicsWorld(world), m_softBodyWorldInfo(softBodyWorldInfo) {}
	bool init(float mass, std::vector<btScalar> &vertices, std::vector<unsigned int> &triangles) {

        collisionShapes.clear();
        
        // Create Soft Cube
        btScalar s = 1; // Side length of the cube
        int ntriangles = triangles.size() / 3;
        btSoftBody* softCube = btSoftBodyHelpers::CreateFromTriMesh(*m_softBodyWorldInfo,
            vertices.data(), (int*)triangles.data(), ntriangles, false);
        softCube->generateBendingConstraints(2);
        softCube->m_cfg.piterations = 50;
        softCube->m_cfg.kDP = 0.01;
        softCube->m_cfg.kDF = 0.8;
        softCube->m_cfg.kMT = 0.02;
        softCube->m_cfg.kCHR = 1.0;
        softCube->m_cfg.kKHR = 0.1;
        softCube->m_cfg.kSHR = 1.0;
        softCube->m_cfg.kAHR = 0.7;
        softCube->setTotalMass(mass, false);
        // softCube->translate(btVector3(pos[0], pos[1], pos[2]));

        world_id = dynamicsWorld->getNumCollisionObjects();
        // Add Soft Cube to Dynamics World
        ((btSoftRigidDynamicsWorld*)dynamicsWorld)->addSoftBody(softCube);

        return true;
    }

    ~GravitySoftBody() {
        //next line is optional: it will be cleared by the destructor when the array goes out of scope
        collisionShapes.clear();
    }

    int get_world_id() {
        return world_id;
    }
 private:
    // btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

    //keep track of the shapes, we release memory at exit.
    //make sure to re-use collision shapes among rigid bodies whenever possible!
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    btVector3 pos;
	btScalar mass = 0.f;
    int world_id = -1;

    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    btSoftBodyWorldInfo *m_softBodyWorldInfo = nullptr;
};
