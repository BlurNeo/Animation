#pragma once

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

class OrthogonalCamera {
 public:
    OrthogonalCamera() :
        init_camera_world_pos(0,0,500),
        init_camera_look_at_pos(0,0,-100),
        init_model_world_pos(0.f, 0.f, 0.f) {
            cur_camera_world_pos = init_camera_world_pos;
            cur_camera_look_at_pos = init_camera_look_at_pos;
        }
    
    OrthogonalCamera(glm::vec3 pos, glm::vec3 look_at) :
        init_camera_world_pos(pos),
        init_camera_look_at_pos(look_at),
        init_model_world_pos(0.f, 0.f, 0.f) {
            cur_camera_world_pos = init_camera_world_pos;
            cur_camera_look_at_pos = init_camera_look_at_pos;
        }

    void reset_trans() {
        cur_camera_world_pos = init_camera_world_pos;
        cur_camera_look_at_pos = init_camera_look_at_pos;
    }

    void rotate(float x, float y, float z) {
        // rotate around the camera coordinate axis
        glm::vec3 look_dir(cur_camera_world_pos - cur_camera_look_at_pos);
        look_dir =  glm::normalize(look_dir);
        glm::vec3 camera_z_dir = look_dir;
        glm::vec3 camera_x_dir = glm::rotate(glm::mat4(1.0), glm::radians(90.f), glm::vec3(0,1,0)) * glm::vec4(camera_z_dir, 0);
        glm::vec3 camera_y_dir = glm::vec3(0,1,0);//glm::rotate(glm::mat4(1.0), glm::radians(90.f), camera_x_dir) * glm::vec4(camera_z_dir, 0);
        auto rotmat1 = glm::rotate(glm::mat4(1.0), glm::radians(x/5), camera_y_dir);
        auto rotmat2 = glm::rotate(glm::mat4(1.0), glm::radians(y/5), camera_x_dir);
        cur_camera_look_at_pos = cur_camera_world_pos - glm::vec3(rotmat1 * rotmat2 * glm::vec4(look_dir, 0));

        // rotate around the world coordinate axis, can't precisely show the mouse movement
        // auto rotmat_x = glm::rotate(glm::mat4(1.0), glm::radians(x), glm::vec3(0,1,0));
        // auto rotmat_y = glm::rotate(glm::mat4(1.0), glm::radians(y), glm::vec3(1,0,0));
        // init_camera_look_at_pos = rotmat_x * rotmat_y * glm::vec4(init_camera_look_at_pos, 1);
    }
    void translate(float x, float y, float z) {
        glm::vec3 look_dir(cur_camera_world_pos - cur_camera_look_at_pos);
        look_dir[1] = 0;
        look_dir =  glm::normalize(look_dir);
        // convert to camera coordinate and translate x
        auto rotmat_x = glm::rotate(glm::mat4(1.0), glm::radians(90.f), glm::vec3(0,1,0));
        glm::vec3 delta_pos = x * rotmat_x * glm::vec4(look_dir, 1);
        glm::vec3 delta_look_pos = x * rotmat_x * glm::vec4(look_dir, 1);
        cur_camera_world_pos += delta_pos;
        cur_camera_look_at_pos += delta_look_pos;
        // convert to camera coordinate and translate y
        cur_camera_world_pos[1] += y;
        cur_camera_look_at_pos[1] += y;
        // convert to camera coordinate and translate z
        cur_camera_world_pos = cur_camera_world_pos + z * look_dir;
        cur_camera_look_at_pos = cur_camera_look_at_pos + z * look_dir;

    }
    glm::mat4 get_mvp() {
        glm::mat4 Projection, View, Model;
        return get_mvp(Projection, View, Model);
    }

    glm::mat4 get_mvp(glm::mat4 &Projection, glm::mat4 &View, glm::mat4 &Model) {
        GLfloat near_plane = 1.0f, far_plane = 200.f;
        Projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
        View = glm::lookAt(cur_camera_world_pos, // Camera is at (4,3,3), in World Space
            cur_camera_look_at_pos,              // and looks at the origin
            glm::vec3(0,1,0)                      // Head is up (set to 0,-1,0 to look upside-down)
        );
        Model = glm::translate(glm::mat4(1.0f), init_model_world_pos);
        return Projection * View * Model;
    }

    glm::vec3 get_camera_pos() {
        return cur_camera_world_pos;
    }

    glm::vec3 get_camera_ray() {
        return glm::normalize(cur_camera_look_at_pos - cur_camera_world_pos);
    }

 private:
    glm::vec3 cur_camera_world_pos;
    glm::vec3 cur_camera_look_at_pos;
    glm::vec3 init_camera_world_pos;
    glm::vec3 init_camera_look_at_pos;
    glm::vec3 init_model_world_pos;
};