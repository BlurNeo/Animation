

add_executable(opengl_example ${CMAKE_CURRENT_SOURCE_DIR}/opengl_example.cpp ${env_srcs} ${util_srcs} ${stb_srcs} ${render_srcs} ${render_body_srcs} ${physics_srcs})
target_link_libraries(opengl_example BulletSoftBody_xcode4_x64_debug BulletDynamics_xcode4_x64_debug BulletCollision_xcode4_x64_debug LinearMath_xcode4_x64_debug GLEW::GLEW glfw)
