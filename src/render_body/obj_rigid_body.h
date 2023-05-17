#pragma once

#include "glm/glm.hpp"
#include "gravity_rigid_body.h"
#include "obj_renderer.h"
#include "rigid_world.h"

// Physical box with rendering
class ObjRigidBody {
 public:
    ObjRigidBody(float mass, glm::vec3 _pos, glm::vec3 _half_extension, glm::vec3 _velocity = glm::vec3(0,0,0)) :
            pos(_pos), half_extension(_half_extension),
            gravity_rigid_body(RigidWorld::get()->dynamicsWorld) {
	    gravity_rigid_body.init(mass, _pos, _half_extension, _velocity);
    }
    ~ObjRigidBody() {}
    void set_controler(InputControl *_controler) {
        obj_renderer.set_controler(_controler);
    }
    bool init(std::string obj_path, std::string texture_path) {
        obj_renderer.init(obj_path, texture_path);
        return true;
    }
    void draw() {
        btTransform trans = this->get_trans_from_world();
        obj_renderer.set_transform(trans);
        obj_renderer.draw();
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
        trans.setOrigin(trans.getOrigin());
        return trans;
    }

 private:
    glm::vec3 pos, half_extension;
    GravityRigidBody gravity_rigid_body;
    ObjRenderer obj_renderer;
};