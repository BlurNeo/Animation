
#include <iostream>
#include <vector>
#include "stb_image_write.h"
#include "gl_window.h"
#include "render_to_image.h"
// #include "obj_renderer.h"
// #include "box_renderer.h"
#include "common_macro.hpp"
#include "input_control.h"
// #include "gravity_rigid_body.h"
// #include "gravity_soft_body.h"
#include "box_rigid_body.h"
#include "obj_rigid_body.h"
#include "obj_soft_body.h"
#include "fps_shoot.h"
#include "helpers.h"
#include "render_scene.h"
#include "cube_env_renderer.h"
#include "sphere_env_renderer.h"

int main() {
    int width = 800, height = 600;
    float background_color[4] = {0.2f, 0.3f, 0.3f, 1.f};
    std::string title = "OpenglExample";
    std::string image_path = "/Users/ssc/Desktop/workspace/git_repos/Animation/a.png";
    std::string obj_path = "/Users/ssc/Desktop/workspace/git_repos/Animation/data/1005_bone2mesh.obj";
    std::string texture_path = "/Users/ssc/Desktop/workspace/git_repos/Animation/data/1005_sm.jpg";
    std::string env_obj_path = "/Users/ssc/Desktop/workspace/git_repos/Animation/data/ocean_sky_sphere/maya_sphere.obj";
    std::string env_texture_path = "/Users/ssc/Desktop/workspace/git_repos/Animation/data/ocean_sky_sphere/texturify_pano-1-13.jpg";
    std::vector<std::string> env_tex_paths = {
        "/Users/ssc/Desktop/workspace/git_repos/Animation/data/skybox/right.jpg",
        "/Users/ssc/Desktop/workspace/git_repos/Animation/data/skybox/left.jpg",
        "/Users/ssc/Desktop/workspace/git_repos/Animation/data/skybox/top.jpg",
        "/Users/ssc/Desktop/workspace/git_repos/Animation/data/skybox/bottom.jpg",
        "/Users/ssc/Desktop/workspace/git_repos/Animation/data/skybox/front.jpg",
        "/Users/ssc/Desktop/workspace/git_repos/Animation/data/skybox/back.jpg"
    };
    GlWindow gl_window;
    int ret = gl_window.init(width, height, title);
    CHECK_EQ_RET(ret, GL_TRUE, -1)

    // Setup the render scene
    glm::vec3 light_pos(50,50,50);
    glm::vec3 light_dir(-50,-50,-50);
    RenderScene *scene = RenderScene::get();
    scene->init();
    scene->set_light_pos(light_pos);
    scene->set_light_dir(light_dir);

    // CubeEnvRenderer env_renderer;
    // env_renderer.init(env_tex_paths);
    SphereEnvRenderer env_renderer;
    env_renderer.init(env_obj_path, env_texture_path);
    RenderToImage render_to_image;
    ObjRenderer obj_renderer;
    ret = obj_renderer.init(obj_path, texture_path);
    BoxRigidBody ground_body(0.f, glm::vec3(0,-50,0), glm::vec3(50, 1, 50));
    ground_body.init();
    BoxRigidBody box_body(500.f, glm::vec3(0,10,0), glm::vec3(10, 10, 10));
    box_body.init();
    FpsShoot fps_shoot;
    fps_shoot.init();
    // ObjRigidBody obj_body(50.f, glm::vec3(0,160,0), glm::vec3(10, 10, 10));
    // obj_body.init(obj_path, texture_path);
    // ObjSoftBody soft_body(5.f, obj_path, texture_path);
    // soft_body.init();
    InputControl input_control(gl_window.get_handle());
    CHECK_EQ_RET(ret, GL_TRUE, -1)
    env_renderer.set_controler(&input_control);
    ground_body.set_controler(&input_control);
    box_body.set_controler(&input_control);
    fps_shoot.set_controler(&input_control);
    obj_renderer.set_controler(&input_control);
    // obj_body.set_controler(&input_control);
    // soft_body.set_controler(&input_control);
    RigidWorld* rigid_world = RigidWorld::get();
    while (!gl_window.should_close()) {
        gl_window.clear_color(background_color);
        rigid_world->update(0.f);
        input_control.update();
        env_renderer.draw();
        ground_body.draw_shadow_map();
        box_body.draw_shadow_map();
        fps_shoot.draw_shadow_map();
        ground_body.draw();
        box_body.draw();
        fps_shoot.draw();
        obj_renderer.draw();
        // obj_body.draw();
        // soft_body.draw();
        scene->clear_shadow_map();
        gl_window.swap_buffers();
    }

    return 0;
}
