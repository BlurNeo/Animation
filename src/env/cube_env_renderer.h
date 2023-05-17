#pragma once

#include <string>
#include "glm/glm.hpp"
#include "common_macro.hpp"
#include <vector>
#include <iostream>
#include "stb_image.h"
#include "input_control.h"

static const char* env_vertex_shader_source = "\
#version 330 core \
layout (location = 0) in vec3 aPosition;\
\
out vec3 TexCoords;\
\
uniform mat4 View; \
uniform mat4 Projection; \
\
void main()\
{\
    TexCoords = aPosition;\
    gl_Position = Projection * View * vec4(aPosition, 1.0);\
}\
";

static const char* env_fragment_shader_source = "\
#version 330 core \
in vec3 TexCoords;\
out vec4 FragColor;\
uniform samplerCube skybox;\
\
void main()\
{\
    FragColor = texture(skybox, TexCoords);\
}\
";

class CubeEnvRenderer {
 public:
    CubeEnvRenderer() {}
    ~CubeEnvRenderer() {}

    int init(std::vector<std::string> textures_faces) {
    {
        // Vertex shader loading
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &(env_vertex_shader_source), nullptr);
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
        glShaderSource(fragmentShader, 1, &env_fragment_shader_source, nullptr);
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
        // {
        //     // std::vector<float> normals;
        //     // std::vector<unsigned int> normal_idxs;
        //     // obj_extractor.get_normals(normals);
        //     // obj_extractor.get_normal_idxs(normal_idxs);
        //     // Generate VBO and EBO for normals
        //     // glGenBuffers(1, &N_EBO);
        //     glGenBuffers(1, &N_VBO);
        //     // Copy the vertices into VBO
        //     glBindBuffer(GL_ARRAY_BUFFER, N_VBO);
        //     glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals_.size(), normals_.data(), GL_DYNAMIC_DRAW);
        //     // // Copy the vertex indices into EBO
        //     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, N_EBO);
        //     // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * normal_idxs.size(), normal_idxs.data(), GL_DYNAMIC_DRAW);
        //     // Enable the vertex attribute #1 as the input normals
        //     int normal_loc = glGetAttribLocation(shaderProgram, "aNormal");
        //     // std::cout << "normal_loc: " << normal_loc << "\n";
        //     glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
        //     glEnableVertexAttribArray(normal_loc);
        // }

        this->init_cube_map(textures_faces);
        // Unbind the VAO
        glBindVertexArray(0);    
        GL_CHECK_RET(-1);
        return GL_TRUE;
    }
    bool init_cube_map(std::vector<std::string> textures_faces) {
        glGenTextures(1, &texture_cube_map_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cube_map_id);
        int width, height, nrChannels;
        unsigned char *data = nullptr;  
        // right, left, up, down, back, front
        for(unsigned int i = 0; i < textures_faces.size(); i++)
        {
            unsigned char *data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap tex failed to load at path: " << textures_faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return true;
    }
    void init_vertices() {
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
        // this->normals_ = {
        //     0.125, 0.125, 0.125,
        //     0.125, -0.125, 0.125,
        //     0.125, -0.125, -0.125,
        //     0.125, 0.125, -0.125,
        //     -0.125, 0.125, 0.125,
        //     -0.125, -0.125, 0.125,
        //     -0.125, 0.125, -0.125,
        //     -0.125, -0.125, -0.125,
        // };
    }
    void set_controler(InputControl *_controler) {
        controler = _controler;
    }
    int draw() {
        glUseProgram(shaderProgram);

        // float Translate = 0;
        // glm::vec2 Rotate(5, 5);
        glm::mat4 Model, View, Projection;
        controler->get_mvp(Projection, View, Model);
        
        std::vector<float> View_vec = glm_mat4_to_std_vec(View, false);
        std::vector<float> Projection_vec = glm_mat4_to_std_vec(Projection, false);
        // glm::mat4 light_mat = RenderScene::get()->get_light_space_matrix();
        // std::vector<float> light_mat_vec = glm_mat4_to_std_vec(light_mat, false);
        // std::cout << MVP;
        // GLint triangle_model_location = glGetUniformLocation(shaderProgram, "Model");
        // glUniformMatrix4fv(triangle_model_location, 1, GL_FALSE, (const GLfloat*)Model_vec.data());
        GLint triangle_view_location = glGetUniformLocation(shaderProgram, "View");
        glUniformMatrix4fv(triangle_view_location, 1, GL_FALSE, (const GLfloat*)View_vec.data());
        GLint triangle_projection_location = glGetUniformLocation(shaderProgram, "Projection");
        glUniformMatrix4fv(triangle_projection_location, 1, GL_FALSE, (const GLfloat*)Projection_vec.data());
        // GLint light_mat_location = glGetUniformLocation(shaderProgram, "lightMat");
        // glUniformMatrix4fv(light_mat_location, 1, GL_FALSE, (const GLfloat*)light_mat_vec.data());
        // GLint light_dir_location = glGetUniformLocation(shaderProgram, "aLightDir");
        // glm::vec3 light_dir = RenderScene::get()->get_light()->get_dir();
        // glUniform3f(light_dir_location, light_dir[0], light_dir[1], light_dir[2]);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cube_map_id);
        glDrawElements(GL_TRIANGLES, vertices_indices_.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        GL_CHECK_RET(-1);
        
        return GL_TRUE;
    }

 private:
    unsigned int texture_cube_map_id;

    // VAO
    GLuint VAO;
    // VBO for vertices
    GLuint V_VBO;
    GLuint V_EBO;
    // // VBO for normals
    // GLuint N_VBO;
    // GLuint N_EBO;
    // // VBO for textures
    // GLuint T_VBO;
    // GLuint T_EBO;
    std::vector<float> vertices_;
    // std::vector<float> textures_;
    // std::vector<float> normals_;
    std::vector<unsigned int> vertices_indices_;
    GLuint shaderProgram;

    InputControl *controler = nullptr;
    glm::vec3 pos = glm::vec3(0,0,0);
    glm::vec3 half_extension = glm::vec3(1000, 1000, 1000);
    btTransform trans;
};