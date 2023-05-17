#pragma once
#include "LinearMath/btTransform.h"
#include "btBulletDynamicsCommon.h"

class GravityRigidBody {
 public:
    GravityRigidBody(btDiscreteDynamicsWorld* world) : dynamicsWorld(world) {}
	bool init(float mass, glm::vec3 _pos, glm::vec3 half_extension, glm::vec3 _velocity) {
        collisionShapes.clear();
        ///create a few basic rigid bodies

        //the ground is a cube of side 100 at position y = -56.
        //the sphere will hit it at y = -6, with center at -5
        // create_ground();
        pos = btVector3(_pos[0], _pos[1], _pos[2]);

		btCollisionShape* groundShape = new btBoxShape(btVector3(half_extension[0], half_extension[1], half_extension[2]));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(pos);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

        //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
        
        btVector3 lin_vel(_velocity[0], _velocity[1], _velocity[2]);
	    body->setLinearVelocity(lin_vel);
		
        world_id = dynamicsWorld->getNumCollisionObjects();
		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);

        return true;
	}

    ~GravityRigidBody() {
        //next line is optional: it will be cleared by the destructor when the array goes out of scope
        collisionShapes.clear();
    }

    int get_world_id() {
        return world_id;
    }

 private:
    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

    //keep track of the shapes, we release memory at exit.
    //make sure to re-use collision shapes among rigid bodies whenever possible!
    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    btVector3 pos;
	btScalar mass = 0.f;
    int world_id = -1;
};
