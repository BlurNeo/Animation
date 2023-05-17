#pragma once

#include "glm/glm.hpp"
#include "common_macro.hpp"
#include "helpers.h"
#include "box_renderer.h"
#include "render_scene.h"
#include <glm/gtc/type_ptr.hpp>

static const char* box_vertex_shader_source = " \
#version 330 core \
\
layout (location = 0) in vec3 aPosition; \
layout (location = 1) in vec3 aNormal; \
uniform mat4 Model; \
uniform mat4 View; \
uniform mat4 Projection; \
uniform mat4 lightMat; \
uniform vec3 aLightDir; \
out vec3 Normal; \
out vec4 lightMapPos; \
out vec3 lightDir; \
\
void main() \
{ \
    gl_Position = Projection*View*Model*vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);\
    Normal = aNormal; \
    lightMapPos = lightMat * Model * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0);\
    lightDir = aLightDir; \
}\
";

// image pixel coord should be flipped to match texture coord
// pixel coord is left top as origin, texture coord is left bottom as origin
static const char* box_fragment_shader_source = " \
#version 330 core \
\
in vec3 Normal; \
in vec4 lightMapPos; \
in vec3 lightDir; \
\
uniform sampler2D shadowMap;\
\
out vec4 color; \
\
float ShadowCalculation(vec4 fragPosLightSpace) \
{\
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; \
    projCoords = projCoords * 0.5 + 0.5; \
    float closestDepth = texture(shadowMap, projCoords.xy).r; \
    float currentDepth = projCoords.z; \
    float shadow = currentDepth > closestDepth + 0.005  ? 1.0 : 0.0; \
\
    return shadow; \
} \
\
void main() \
{ \
    vec4 obj_color = vec4(0.3, 0.7, 0.7, 1); \
    vec4 light_dir = normalize(vec4(-lightDir.x, -lightDir.y, -lightDir.z, 0)); \
    vec4 light_color = vec4(1, 1, 1, 0); \
    vec4 diffuse = max(dot(light_dir, vec4(Normal, 0)), 0) * light_color; \
    float ambient_strength = 0.3f; \
    vec4 ambient = ambient_strength * light_color; \
    float shadow = ShadowCalculation(lightMapPos); \
    color = (ambient + (1.0-shadow) * diffuse) * obj_color; \
} \
";

BoxRenderer::BoxRenderer(glm::vec3 _pos, glm::vec3 _half_extension) : pos(_pos), half_extension(_half_extension) {}
BoxRenderer::~BoxRenderer() {}

int BoxRenderer::init() {
    {
        // Vertex shader loading
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &(box_vertex_shader_source), nullptr);
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
        glShaderSource(fragmentShader, 1, &box_fragment_shader_source, nullptr);
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
    }
    
    // init vertices
    this->init_vertices();

    glGenVertexArrays(1, &VAO);
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
        // std::cout << "position_loc: " << position_loc << "\n";
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
        // std::cout << "normal_loc: " << normal_loc << "\n";
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        glEnableVertexAttribArray(normal_loc);
    }
    // Unbind the VAO
    glBindVertexArray(0);    
    GL_CHECK_RET(-1);
    return GL_TRUE;
}

void BoxRenderer::init_vertices() {
    int vertice_num = 8*3;
    this->vertices_.resize(vertice_num);
    this->vertices_ = {
        pos[0] + half_extension[0], pos[1] + half_extension[1], pos[2] + half_extension[2],
        pos[0] + half_extension[0], pos[1] - half_extension[1], pos[2] + half_extension[2],
        pos[0] + half_extension[0], pos[1] - half_extension[1], pos[2] - half_extension[2],
        pos[0] + half_extension[0], pos[1] + half_extension[1], pos[2] - half_extension[2],
        pos[0] - half_extension[0], pos[1] + half_extension[1], pos[2] + half_extension[2],
        pos[0] - half_extension[0], pos[1] - half_extension[1], pos[2] + half_extension[2],
        pos[0] - half_extension[0], pos[1] + half_extension[1], pos[2] - half_extension[2],
        pos[0] - half_extension[0], pos[1] - half_extension[1], pos[2] - half_extension[2]
    };
    this->vertices_indices_ = {
        //right face
        0,1,2, 0,2,3,
        // front face
        0,1,5, 0,4,5,
        // top face
        0,4,6, 0,3,6,
        // bottom face
        1,5,7, 1,2,7,
        // back face
        3,6,7, 3,2,7,
        // left face
        4,5,7, 4,6,7
    };
    this->normals_ = {
        0.125, 0.125, 0.125,
        0.125, -0.125, 0.125,
        0.125, -0.125, -0.125,
        0.125, 0.125, -0.125,
        -0.125, 0.125, 0.125,
        -0.125, -0.125, 0.125,
        -0.125, 0.125, -0.125,
        -0.125, -0.125, -0.125,
    };
}

void BoxRenderer::set_controler(InputControl *_controler) {
    controler = _controler;
}

void BoxRenderer::set_transform(btTransform _trans) {
    trans = _trans;
}

int BoxRenderer::draw_shadow_map() {
    glm::mat4 Model, View, Projection;
    controler->get_mvp(Projection, View, Model);
    Model = glm::translate(Model, glm::vec3(trans.getOrigin()[0], trans.getOrigin()[1], trans.getOrigin()[2]));
    
    std::shared_ptr<ShadowMap> shadow_map = RenderScene::get()->get_shadow_map();
    shadow_map->draw_shadow_map(VAO, vertices_indices_.size(), Model);
    return GL_TRUE;
}

int BoxRenderer::draw() {
    glUseProgram(shaderProgram);

    // float Translate = 0;
    // glm::vec2 Rotate(5, 5);
    glm::mat4 Model, View, Projection;
    controler->get_mvp(Projection, View, Model);
    Model = glm::translate(Model, glm::vec3(trans.getOrigin()[0], trans.getOrigin()[1], trans.getOrigin()[2]));

    std::vector<float> Model_vec = glm_mat4_to_std_vec(Model, false);
    std::vector<float> View_vec = glm_mat4_to_std_vec(View, false);
    std::vector<float> Projection_vec = glm_mat4_to_std_vec(Projection, false);
    glm::mat4 light_mat = RenderScene::get()->get_light_space_matrix();
    std::vector<float> light_mat_vec = glm_mat4_to_std_vec(light_mat, false);
    // std::cout << MVP;
    GLint triangle_model_location = glGetUniformLocation(shaderProgram, "Model");
    glUniformMatrix4fv(triangle_model_location, 1, GL_FALSE, (const GLfloat*)Model_vec.data());
    GLint triangle_view_location = glGetUniformLocation(shaderProgram, "View");
    glUniformMatrix4fv(triangle_view_location, 1, GL_FALSE, (const GLfloat*)View_vec.data());
    GLint triangle_projection_location = glGetUniformLocation(shaderProgram, "Projection");
    glUniformMatrix4fv(triangle_projection_location, 1, GL_FALSE, (const GLfloat*)Projection_vec.data());
    GLint light_mat_location = glGetUniformLocation(shaderProgram, "lightMat");
    glUniformMatrix4fv(light_mat_location, 1, GL_FALSE, (const GLfloat*)light_mat_vec.data());
    GLint light_dir_location = glGetUniformLocation(shaderProgram, "aLightDir");
    glm::vec3 light_dir = RenderScene::get()->get_light()->get_dir();
    glUniform3f(light_dir_location, light_dir[0], light_dir[1], light_dir[2]);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderScene::get()->get_shadow_map_texture());
    glDrawElements(GL_TRIANGLES, vertices_indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    GL_CHECK_RET(-1);
    

    return GL_TRUE;
}