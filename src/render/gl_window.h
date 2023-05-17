#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

class GlWindow {
 public:
    GlWindow() {}
    ~GlWindow() {
        glfwTerminate();
    }
    
    int init(int width, int height, std::string title) {
        if (!glfwInit()){
            std::cout << "Failed to init GLFW" << std::endl;
            return -1;
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cout << "Failed to initialize GLEW" << std::endl;
            return -1;
        }

        // int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        std::cout << "Window FrameBufferSize: " << width << ", " << height << "\n";

        glViewport(0, 0, width, height);

        return GL_TRUE;
    }


    bool should_close() {
        return glfwWindowShouldClose(window);
    }
    
    void clear_color(float rgba[4]) {
        glClearColor(rgba[0], rgba[1], rgba[2], rgba[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    void swap_buffers() {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    GLFWwindow* get_handle() { return window; }

 private:
    GLFWwindow* window = nullptr;
};
