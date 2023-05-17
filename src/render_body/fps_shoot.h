#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "gravity_rigid_body.h"
#include "box_renderer.h"
#include "rigid_world.h"

// Physical box with rendering
class FpsShoot {
 public:
    FpsShoot() {}
    ~FpsShoot() {
        for (auto box_rigid_body : shoot_queue) {
            if (box_rigid_body) {
                delete box_rigid_body;
                box_rigid_body = nullptr;
            }
        }
        shoot_queue.clear();
    }
    void set_controler(InputControl *_controler) {
        controler = _controler;
    }
    bool init() {
        return true;
    }
    void shoot() {
    }
    void draw_shadow_map() {
        for (auto box_rigid_body : shoot_queue)
            box_rigid_body->draw_shadow_map();
    }
    void draw() {
        if (controler->shooting()) {
            glm::vec3 pos = controler->get_camera_pos();
            pos[1] += 20;
            glm::vec3 half_extension(1,1,1);
            glm::vec3 ray = controler->get_camera_ray();
            // printf("ray: %f %f %f\n", ray[0], ray[1], ray[2]);
            glm::vec3 velocity = 100.f * ray;
            if (shoot_queue.size() < max_shoot_queue_size) {
                float mass = 10;
                auto box_rigid_body = new BoxRigidBody(mass, pos, half_extension, velocity);
                box_rigid_body->set_controler(controler);
                shoot_queue.push_back(box_rigid_body);
                last_box_idx = shoot_queue.size() - 1;
            } else {
                last_box_idx++;
                last_box_idx %= shoot_queue.size();
                shoot_queue[last_box_idx]->reset_motion(pos, velocity);
            }
        }
        for (auto box_rigid_body : shoot_queue)
            box_rigid_body->draw();
    }
 
 private:

 private:
    glm::vec3 pos, half_extension;
    // BoxRigidBody *box_rigid_body = nullptr;
    // GravityRigidBody *gravity_rigid_body;
    // BoxRenderer* box_renderer;
    InputControl *controler = nullptr;
    int max_shoot_queue_size = 10;
    std::vector<BoxRigidBody*> shoot_queue;
    int last_box_idx = -1;
};
