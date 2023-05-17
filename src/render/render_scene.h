#pragma once

#include "light.h"
#include "shadow_map.h"

class RenderScene {
 public:
    static RenderScene* get() {
        static RenderScene scene;
        return &scene;
    }
    bool init() {
        light.reset(new Light());
        shadow_map.reset(new ShadowMap());
        return true;
    }
    void set_light_pos(glm::vec3 vec) {
        light->set_pos(vec);
        shadow_map->set_camera(light->get_camera());
    }
    void set_light_dir(glm::vec3 vec) {
        light->set_dir(vec);;
        shadow_map->set_camera(light->get_camera());
    }
    glm::mat4 get_light_space_matrix() {
        return light->get_light_space_matrix();
    }
    std::shared_ptr<Light> get_light() {
        return light;
    }
    std::shared_ptr<ShadowMap> get_shadow_map() {
        return shadow_map;
    }
    GLuint get_shadow_map_texture() {
        return shadow_map->get_shadow_map();
    }
    void clear_shadow_map() {
        return shadow_map->clear();
    }
 
 private:
    std::shared_ptr<Light> light;
    std::shared_ptr<ShadowMap> shadow_map;
};