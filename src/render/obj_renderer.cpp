#pragma once

#include "common_macro.hpp"
#include "obj_renderer.h"
#include "stb_image.h"
#include <vector>
#include <iostream>
#include "helpers.h"

static const char* obj_vertex_shader_source = " \
#version 330 core \
\
layout (location = 0) in vec3 aPosition; \
layout (location = 1) in vec2 aTextureCoord; \
layout (location = 2) in vec3 aNormal; \
uniform mat4 Model; \
uniform mat4 View; \
uniform mat4 Projection; \
out vec3 Normal; \
out vec2 TextureCoord; \
\
void main() \
{ \
    gl_Position = Projection*View*Model*vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);\
    Normal = aNormal; \
    TextureCoord = aTextureCoord; \
}\
";

// image pixel coord should be flipped to match texture coord
// pixel coord is left top as origin, texture coord is left bottom as origin
static const char* obj_fragment_shader_source = " \
#version 330 core \
\
in vec3 Normal; \
in vec2 TextureCoord; \
out vec4 color; \
uniform sampler2D texture1; \
\
void main() \
{ \
    vec4 obj_color = texture(texture1, vec2(TextureCoord.s, 1-TextureCoord.t)); \
    vec4 light_dir = normalize(vec4(0, 1, 1, 0)); \
    vec4 light_color = vec4(1, 1, 1, 0); \
    vec4 diffuse = dot(light_dir, vec4(Normal, 0)) * light_color; \
    float ambient_strength = 0.1f; \
    vec4 ambient = ambient_strength * light_color; \
    color = (ambient + diffuse) * obj_color; \
} \
";

ObjRenderer::ObjRenderer() {}

ObjRenderer::~ObjRenderer() {}

int ObjRenderer::init(std::string obj_path, std::string texture_path) {
    // Vertex shader loading
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &(obj_vertex_shader_source), nullptr);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment shader loading
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &obj_fragment_shader_source, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Link the loaded shader
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Extract vertices from obj
    int ret = obj_extractor.init(obj_path);
    CHECK_EQ_RET(ret, true, -1)
    obj_extractor.get_infos(vertices_indices_, vertices_, textures_, normals_);
    
    glGenVertexArrays(1, &VAO);

    GL_CHECK_RET(-1);
    
    obj_extractor.get_vertices(vertices_);
    obj_extractor.get_vertice_idxs(vertices_indices_);
    
    submit_vertice();
    submit_texture(texture_path);

    return GL_TRUE;
}

int ObjRenderer::submit_vertice() {
    // Bind the VAO
    glBindVertexArray(VAO);
    // VAO can only bind one EBO, therefore all vertice,normals,texcoord should be in the same order
    // VBO won't be bound to VAO, therefore we can create serveral VBO, each for one attribute
    {
        // Generate VBO and EBO for vertices
        glGenBuffers(1, &V_EBO);
        glGenBuffers(1, &V_VBO);
        // Copy the vertices into VBO
        glBindBuffer(GL_ARRAY_BUFFER, V_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_.size(), vertices_.data(), GL_DYNAMIC_DRAW);
        // Copy the vertex indices into EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, V_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * vertices_indices_.size(), vertices_indices_.data(), GL_DYNAMIC_DRAW);
        // Enable the vertex attribute #0 as the input vertices
        int position_loc = glGetAttribLocation(shaderProgram, "aPosition");
        std::cout << "position_loc: " << position_loc << "\n";
        glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(position_loc);
    }
    {
        // std::vector<float> normals;
        // std::vector<unsigned int> normal_idxs;
        // obj_extractor.get_normals(normals);
        // obj_extractor.get_normal_idxs(normal_idxs);
        // Generate VBO and EBO for normals
        // glGenBuffers(1, &N_EBO);
        glGenBuffers(1, &N_VBO);
        // Copy the vertices into VBO
        glBindBuffer(GL_ARRAY_BUFFER, N_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals_.size(), normals_.data(), GL_DYNAMIC_DRAW);
        // // Copy the vertex indices into EBO
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, N_EBO);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * normal_idxs.size(), normal_idxs.data(), GL_DYNAMIC_DRAW);
        // Enable the vertex attribute #1 as the input normals
        int normal_loc = glGetAttribLocation(shaderProgram, "aNormal");
        std::cout << "normal_loc: " << normal_loc << "\n";
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(normal_loc);
    }
    glBindVertexArray(0);

    GL_CHECK_RET(-1);
    
    return GL_TRUE;
}

int ObjRenderer::submit_texture(std::string texture_path) {
    glBindVertexArray(VAO);
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return GL_FALSE;
    }
    stbi_image_free(data);
    GL_CHECK_RET(-1);
    
    // int tmp = glGetUniformLocation(shaderProgram, "atexture1");
    // GL_CHECK_RET(-1);
    // glUniform1i(tmp, 0);
    // GL_CHECK_RET(-1);
    

    // std::vector<float> textures;
    // std::vector<unsigned int> texture_idxs;
    // obj_extractor.get_textures(textures);
    // obj_extractor.get_texture_idxs(texture_idxs);
    // Generate VBO and EBO for normals
    // glGenBuffers(1, &T_EBO);
    glGenBuffers(1, &T_VBO);
    // Copy the vertices into VBO
    glBindBuffer(GL_ARRAY_BUFFER, T_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * textures_.size(), textures_.data(), GL_DYNAMIC_DRAW);
    // Copy the vertex indices into EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, T_EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * texture_idxs.size(), texture_idxs.data(), GL_DYNAMIC_DRAW);
    // Enable the vertex attribute #1 as the input normals
    int texture_loc = glGetAttribLocation(shaderProgram, "aTextureCoord");
    std::cout << "texture_loc: " << texture_loc << "\n";
    glVertexAttribPointer(texture_loc, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
    glEnableVertexAttribArray(texture_loc);
    glBindVertexArray(0);

    GL_CHECK_RET(-1);
    return GL_TRUE;
}

std::vector<float> ObjRenderer::get_vertices() {
    return this->vertices_;
}

std::vector<unsigned int> ObjRenderer::get_vertice_idxs() {
    return this->vertices_indices_;
}

void ObjRenderer::set_vertices(std::vector<float> &vertices) {
    this->vertices_ = vertices;
}

void ObjRenderer::set_vertice_idxs(std::vector<unsigned int> &vertices_indices) {
    this->vertices_indices_ = vertices_indices;
}

void ObjRenderer::set_controler(InputControl *_controler) {
    controler = _controler;
}

void ObjRenderer::set_transform(btTransform _trans) {
    trans = _trans;
}

int ObjRenderer::draw() {
    glUseProgram(shaderProgram);

    // float Translate = 0;
    // glm::vec2 Rotate(5, 5);
    glm::mat4 Model, View, Projection;
    controler->get_mvp(Projection, View, Model);
    Model = glm::translate(Model, glm::vec3(trans.getOrigin()[0], trans.getOrigin()[1], trans.getOrigin()[2]));
    // camera(Translate, Rotate, Projection, View, Model);
    std::vector<float> Model_vec = glm_mat4_to_std_vec(Model, false);
    std::vector<float> View_vec = glm_mat4_to_std_vec(View, false);
    std::vector<float> Projection_vec = glm_mat4_to_std_vec(Projection, false);
    // std::cout << MVP;
    GLint triangle_model_location = glGetUniformLocation(shaderProgram, "Model");
    glUniformMatrix4fv(triangle_model_location, 1, GL_FALSE, (const GLfloat*)Model_vec.data());
    GLint triangle_view_location = glGetUniformLocation(shaderProgram, "View");
    glUniformMatrix4fv(triangle_view_location, 1, GL_FALSE, (const GLfloat*)View_vec.data());
    GLint triangle_projection_location = glGetUniformLocation(shaderProgram, "Projection");
    glUniformMatrix4fv(triangle_projection_location, 1, GL_FALSE, (const GLfloat*)Projection_vec.data());

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, vertices_indices_.size(), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    GL_CHECK_RET(-1);
    
    return GL_TRUE;
}