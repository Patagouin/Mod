project(mg)

cmake_minimum_required(VERSION 2.8)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 "3.0.3" REQUIRED)
add_subdirectory(glfw)
add_subdirectory(SOIL)

include_directories( ${PROJECT_SOURCE_DIR}/glfw/include )
include_directories( ${PROJECT_SOURCE_DIR}/glfw/deps )
include_directories( ${PROJECT_SOURCE_DIR}/SOIL/src )
include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/src )

# To change later
add_definitions(-DPGHP_DIR="${PROJECT_SOURCE_DIR}")

SET(QT_USE_QTOPENGL TRUE)

# Moc
set(moc_cpp
    src/DisplayGL.cpp
    src/Windows.cpp
    )
set(moc_h
    src/DisplayGL.h
    src/Windows.h
    )
set(forms ) # Fichiers.ui
set(ressources ) # Data.qrc


# the list of source files
set(srcs
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
    src/DisplayGL.cpp
    glfw/deps/glad.c

    )

QT4_WRAP_CPP(headersMoced ${moc_h})
QT4_WRAP_UI(formed ${forms})
QT4_ADD_RESOURCES(ressourced ${ressources})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


ADD_EXECUTABLE(mg ${srcs}
    ${headersMoced}
    ${forms}
    ${ressources})

#add_executable(mg ${srcs} ${headersMoced} )

target_link_libraries(mg glfw SOIL ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${QT_LIBRARIES} dl)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # Pour que l'en-tête du fichier généré par uic soit trouvé par le compilateur
