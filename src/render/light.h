#pragma once

#include "glm/glm.hpp"
#include "orthogonal_camera.h"

class Light {
 public:
    explicit Light() : pos(glm::vec3(1.0,1.0,1.0)), dir(glm::vec3(-1.0,-1.0,-1.0)) {
        orth_camera.reset(new OrthogonalCamera(pos, pos+dir));
    }
    explicit Light(glm::vec3 _pos, glm::vec3 _dir) : pos(_pos), dir(_dir) {
        orth_camera.reset(new OrthogonalCamera(pos, pos+dir));
    }
    void set_pos(glm::vec3 vec) {
        pos = vec;
        orth_camera.reset(new OrthogonalCamera(pos, pos+dir));
    }
    void set_dir(glm::vec3 vec) {
        dir = vec;
        orth_camera.reset(new OrthogonalCamera(pos, pos+dir));
    }
    glm::vec3 get_pos() {
        return pos;
    }
    glm::vec3 get_dir() {
        return dir;
    }
    glm::mat4 get_light_space_matrix() {
        glm::mat4 Projection, View, Model;
        orth_camera->get_mvp(Projection, View, Model);
        return Projection * View;
    }
    std::shared_ptr<OrthogonalCamera> get_camera() {
        return orth_camera;
    }
    
 private:
    glm::vec3 pos, dir;
    std::shared_ptr<OrthogonalCamera> orth_camera;
};