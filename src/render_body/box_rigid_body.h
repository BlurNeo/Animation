#pragma once

#include "glm/glm.hpp"
#include "gravity_rigid_body.h"
#include "box_renderer.h"
#include "rigid_world.h"

// Physical box with rendering
class BoxRigidBody {
 public:
    BoxRigidBody(float mass, glm::vec3 _pos, glm::vec3 _half_extension, glm::vec3 _velocity = glm::vec3(0,0,0)) :
            pos(_pos), half_extension(_half_extension),
            gravity_rigid_body(RigidWorld::get()->dynamicsWorld), box_renderer(_pos, _half_extension) {
	    gravity_rigid_body.init(mass, _pos, _half_extension, _velocity);
        box_renderer.init();
    }
    ~BoxRigidBody() {}
    void set_controler(InputControl *_controler) {
        box_renderer.set_controler(_controler);
    }
    bool init() {
        return true;
    }
    void draw_shadow_map() {
        box_renderer.draw_shadow_map();
    }
    void draw() {
        btTransform trans = this->get_trans_from_world();
        box_renderer.set_transform(trans);
        box_renderer.draw();
    }
    void reset_motion(glm::vec3 _pos, glm::vec3 _velocity) {
        //print positions of all objects
        int world_id = gravity_rigid_body.get_world_id();
        btTransform trans;
        btCollisionObject* obj = RigidWorld::get()->dynamicsWorld->getCollisionObjectArray()[world_id];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body) {
            btTransform groundTransform;
            groundTransform.setIdentity();
            groundTransform.setOrigin(btVector3(_pos[0], _pos[1], _pos[2]));
		    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
            body->setMotionState(myMotionState);
            btVector3 lin_vel(_velocity[0], _velocity[1], _velocity[2]);
	        body->setLinearVelocity(lin_vel);
        }
    }
 
 private:
    btTransform get_trans_from_world() {
        //print positions of all objects
        int world_id = gravity_rigid_body.get_world_id();
        btTransform trans;
        btCollisionObject* obj = RigidWorld::get()->dynamicsWorld->getCollisionObjectArray()[world_id];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            body->getMotionState()->getWorldTransform(trans);
        }
        else
        {
            trans = obj->getWorldTransform();
        }
        // printf("world pos object %d = %f,%f,%f\n", world_id, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
        trans.setOrigin(trans.getOrigin() - btVector3(pos[0], pos[1], pos[2]));
        return trans;
    }

 private:
    glm::vec3 pos, half_extension;
    GravityRigidBody gravity_rigid_body;
    BoxRenderer box_renderer;
};