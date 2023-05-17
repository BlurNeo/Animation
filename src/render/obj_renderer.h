#pragma once

#include "common_macro.hpp"
#include "obj_extractor.h"
#include "stb_image.h"
#include <vector>
#include "input_control.h"
#include <iostream>
#include "LinearMath/btTransform.h"

class ObjRenderer {
 public:
    ObjRenderer();
    ~ObjRenderer();

    int init(std::string obj_path, std::string texture_path);
    int submit_vertice();
    int submit_texture(std::string texture_path);
    std::vector<float> get_vertices();
    std::vector<unsigned int> get_vertice_idxs();

    void set_vertices(std::vector<float> &vertices);

    void set_vertice_idxs(std::vector<unsigned int> &vertices_indices);
    void set_controler(InputControl *_controler);

    void set_transform(btTransform _trans);

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
    ObjExtractor obj_extractor;
    std::vector<float> vertices_;
    std::vector<float> textures_;
    std::vector<float> normals_;
    std::vector<unsigned int> vertices_indices_;
    GLuint shaderProgram;
    InputControl *controler = nullptr;
    
    GLuint texture;
    btTransform trans;
};
