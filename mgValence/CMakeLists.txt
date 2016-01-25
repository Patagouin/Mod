project(glViewer)

cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)
find_package(Eigen3 "3.0.3" REQUIRED)
add_subdirectory(glfw)
add_subdirectory(SOIL)

include_directories( ${PROJECT_SOURCE_DIR}/glfw/include )
include_directories( ${PROJECT_SOURCE_DIR}/glfw/deps )
include_directories( ${PROJECT_SOURCE_DIR}/SOIL/src )
include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/src )

add_definitions(-DPGHP_DIR="${PROJECT_SOURCE_DIR}")

# the list of source files
set(pghp_SRCS
    src/Shader.cpp
    src/WireCube.cpp
    src/Pointcloud.cpp
    src/Mesh.cpp
    src/Meshloader.cpp
    src/Trackball.cpp
    src/Octree.cpp
    src/main.cpp
    src/Analysis.cpp
    src/Reconstruction.cpp
    glfw/deps/glad.c

)

add_executable(glViewer ${pghp_SRCS})

target_link_libraries(glViewer glfw SOIL ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} dl)
