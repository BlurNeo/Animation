#pragma once

#include "glm/glm.hpp"
#include "common_macro.hpp"
#include "input_control.h"
#include <vector>
#include <iostream>
#include "LinearMath/btTransform.h"
#include "orthogonal_camera.h"

class BoxRenderer {
 public:
    BoxRenderer(glm::vec3 _pos, glm::vec3 _half_extension);
    ~BoxRenderer();

    int init();

    void init_vertices();
    void set_controler(InputControl *_controler);
    void set_transform(btTransform _trans);

    int draw_shadow_map();
    int draw();

 private:
    // VAO
    GLuint VAO;
    // VBO for vertices
    GLuint V_VBO;
    GLuint V_EBO;
    // VBO for normals
    GLuint N_VBO;
    GLuint N_EBO;
    // VBO for textures
    GLuint T_VBO;
    GLuint T_EBO;
    std::vector<float> vertices_;
    std::vector<float> textures_;
    std::vector<float> normals_;
    std::vector<unsigned int> vertices_indices_;
    GLuint shaderProgram;

    InputControl *controler = nullptr;
    glm::vec3 pos, half_extension;
    btTransform trans;
};
