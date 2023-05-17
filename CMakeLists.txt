cmake_minimum_required(VERSION 2.8...3.22)

project(Animation)

include(cmake/utils.cmake)

GetFilesFromDir(${PROJECT_SOURCE_DIR}/src/stb_image stb_srcs)
GetFilesFromDir(${PROJECT_SOURCE_DIR}/src/utils util_srcs)
GetFilesFromDir(${PROJECT_SOURCE_DIR}/src/render render_srcs)
GetFilesFromDir(${PROJECT_SOURCE_DIR}/src/render_body render_body_srcs)
GetFilesFromDir(${PROJECT_SOURCE_DIR}/src/physics physics_srcs)
GetFilesFromDir(${PROJECT_SOURCE_DIR}/src/env env_srcs)
include_directories(${PROJECT_SOURCE_DIR}/src/stb_image)
include_directories(${PROJECT_SOURCE_DIR}/src/utils)
include_directories(${PROJECT_SOURCE_DIR}/src/physics)
include_directories(${PROJECT_SOURCE_DIR}/src/env)
include_directories(${PROJECT_SOURCE_DIR}/src/render)
include_directories(${PROJECT_SOURCE_DIR}/src/render_body)
include_directories(${PROJECT_SOURCE_DIR}/third_party/glm)
include_directories(${PROJECT_SOURCE_DIR}/third_party/bullet3/include)
link_directories(${PROJECT_SOURCE_DIR}/third_party/bullet3/lib)

set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS} -fPIC -frtti -fexceptions -flax-vector-conversions -Werror=return-type")
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -fPIC -frtti -fexceptions -flax-vector-conversions -Werror=return-type")
set(CMAKE_CXX_FLAGS "-O0 -g")
add_compile_options("-std=c++11")

find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})

find_package(GLEW REQUIRED)
# # include_directories(${GLEW_INCLUDE_DIRS})

# message(STATUS "-------------${GLEW_INCLUDE_DIRS}")

add_subdirectory(examples)