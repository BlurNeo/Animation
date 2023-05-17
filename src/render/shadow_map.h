#pragma once

#include "glm/glm.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "stb_image_write.h"

static const char* box_shadow_vertex_shader_source = " \
#version 330 core \
layout (location = 0) in vec3 aPosition; \
layout (location = 2) in vec3 aNormal; \
uniform mat4 Model; \
uniform mat4 View; \
uniform mat4 Projection; \
\
void main() \
{\
    gl_Position = Projection * View * Model * vec4(aPosition, 1.0f);\
}\
";

static const char* box_shadow_fragment_shader_source = "\
#version 330 core \
\
void main() \
{\
    gl_FragDepth = gl_FragCoord.z;\
}\
";

class ShadowMap {
 public:
    ShadowMap() {
        this->init();
    }
    void set_camera(std::shared_ptr<OrthogonalCamera> camera) {
        orth_camera = camera;
    }
    ~ShadowMap() {}
    int init() {
        // depth frame buffer for shadow
        
        // Vertex shader loading
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &(box_shadow_vertex_shader_source), nullptr);
        glCompileShader(vertexShader);
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADOW SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Fragment shader loading
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &box_shadow_fragment_shader_source, nullptr);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADOW SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        // Link the loaded shader
        shadow_shaderProgram = glCreateProgram();
        glAttachShader(shadow_shaderProgram, vertexShader);
        glAttachShader(shadow_shaderProgram, fragmentShader);
        glLinkProgram(shadow_shaderProgram);

        glGenFramebuffers(1, &depthMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                    SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glBindTexture(GL_TEXTURE_2D, 0);

        GL_CHECK_RET(-1);
        return GL_TRUE;
    }

    int draw_shadow_map(GLuint VAO, int num_vertice, glm::mat4 model_mat) {
        glUseProgram(shadow_shaderProgram);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        
        GLint viewport_size[4];
        glGetIntegerv(GL_VIEWPORT, viewport_size);
        int cache_width = viewport_size[2];
        int cache_height = viewport_size[3];

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glm::mat4 Projection, View, Model;
        orth_camera->get_mvp(Projection, View, Model);
        light_mat = Projection * View;
        // std::cout << "light mat[0]:" << light_mat[0] << "\n";
        std::vector<float> Model_vec = glm_mat4_to_std_vec(model_mat, false);
        std::vector<float> View_vec = glm_mat4_to_std_vec(View, false);
        std::vector<float> Projection_vec = glm_mat4_to_std_vec(Projection, false);
        // std::cout << MVP;
        GLint triangle_model_location = glGetUniformLocation(shadow_shaderProgram, "Model");
        glUniformMatrix4fv(triangle_model_location, 1, GL_FALSE, (const GLfloat*)Model_vec.data());
        GLint triangle_view_location = glGetUniformLocation(shadow_shaderProgram, "View");
        glUniformMatrix4fv(triangle_view_location, 1, GL_FALSE, (const GLfloat*)View_vec.data());
        GLint triangle_projection_location = glGetUniformLocation(shadow_shaderProgram, "Projection");
        glUniformMatrix4fv(triangle_projection_location, 1, GL_FALSE, (const GLfloat*)Projection_vec.data());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, num_vertice, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        GL_CHECK_RET(-1);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(0);
        glViewport(0, 0, cache_width, cache_height);
        
        return GL_TRUE;
    }

    GLuint get_shadow_map() {
        return depthMap;
    }

    glm::mat4 get_light_mat() {
        return light_mat;
    }

    void clear() {
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        if (0) {
            int width_ = SHADOW_WIDTH, height_ = SHADOW_HEIGHT;
            std::string image_path = "/Users/ssc/Desktop/workspace/git_repos/Animation/shadow_map.png";
            // Read pixels from color attachment of the bound framebuffer
            std::vector<unsigned int> f_bytes(width_ * height_);
            // GL_CHECK_RET(-1);
            glReadPixels(0, 0, width_, height_, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, f_bytes.data());
            // GL_CHECK_RET(-1);
            stbi_write_png(image_path.c_str(), width_, height_, 1, f_bytes.data(), width_ * 1);
        }
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
 private:
    // shadow mapping
    GLuint depthMapFBO;
    GLuint depthMap;
    glm::vec3 light_pos;
    glm::vec3 light_dir;
    std::shared_ptr<OrthogonalCamera> orth_camera;
    GLuint shadow_shaderProgram;

    glm::mat4 light_mat;
    
    const GLuint SHADOW_WIDTH = 1200, SHADOW_HEIGHT = 1200;
};
