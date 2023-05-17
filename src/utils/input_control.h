#pragma once
#include "glm/glm.hpp"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "perspective_camera.h"
#include <iostream>

class InputControl {
 public:
    explicit InputControl (GLFWwindow* _window) : window(_window) {
    }
    void update() {
        // Move forward
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
            camera.translate(0, 0, -2);
        }
        // Move left
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
            camera.translate(-2, 0, 0);
        }
        // Move backward
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
            camera.translate(0, 0, 2);
        }
        // Move right
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
            camera.translate(2, 0, 0);
        }
        // Move up
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Q)) {
            camera.translate(0, 2, 0);
        }
        // Move down
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_E)) {
            camera.translate(0, -2, 0);
        }
        // Retrieve cursor movement when mouse pressed
        if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
            if (cursor_pos_inited) {
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                camera.rotate(pre_xpos - xpos, pre_ypos - ypos, 0);
                pre_xpos = xpos;
                pre_ypos = ypos;
            } else {
                glfwGetCursorPos(window, &pre_xpos, &pre_ypos);
                cursor_pos_inited = true;
            }
        } else {
            cursor_pos_inited = false;
        }
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F)) {
            camera.reset_trans();
        }
    }
    void get_mvp(glm::mat4 &Projection, glm::mat4& View, glm::mat4 &Model) {
        camera.get_mvp(Projection, View, Model);
    }

    bool shooting() {        
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) {
            if (pre_shoot) {
                return false;
            } else {
                pre_shoot = true;
                return true;
            }
        } else {
            pre_shoot = false;
            return false;
        }
    }

    glm::vec3 get_camera_pos() {
        return camera.get_camera_pos();
    }

    glm::vec3 get_camera_ray() {
        return camera.get_camera_ray();
    }
 
 private:
    bool cursor_pos_inited = false;
    double pre_xpos, pre_ypos;
    GLFWwindow* window;
    PerspectiveCamera camera;

    bool pre_shoot = false;
};