#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "OpenGL.h"
#include <GLFW/glfw3.h>
#include <SOIL2.h>

#include "common.h"
#include <eigen3/Eigen/Geometry>
using namespace Eigen;


#include "Shader.h"
#include "Trackball.h"
#include "Mesh.h"
#include "Pointcloud.h"
#include "Octree.h"
#include "WireCube.h"
#include "Mesh.h"
#include "Meshloader.h"
#include <surface_mesh/surface_mesh.h>

using namespace surface_mesh;
// initial window size
int WIDTH = 640;
int HEIGHT = 480;

// the default shader program
Shader mBlinn, mSimple, mHole, mMesh;

// geometrical represnetation of a pointlight
Vector3f mLightPos(1,1,1);
Vector3f mLightPos2(-1,-1,-1);

// Camera parameters
Trackball mCamera;

// Mouse parameters
Vector2f mLastMousePos;
int mButton = -1;

//Point Cloud Object
PointCloud* pc;

//Mesh
Mesh* mesh;

//Octree Debug
Octree* octree;
WireCube* wirecube;
int octreeVisu = -1;

/** This method needs to be called once the GL context has been created by GLFW.
  * It is called only once per execution */
void initGL()
{ 
    // set the background color, i.e., the color used
    // to fill the screen when calling glClear(GL_COLOR_BUFFER_BIT)
    glClearColor(0.3f,0.3f,0.3f,0.);

    glEnable(GL_DEPTH_TEST);

    // load the default shaders 
    mBlinn.loadFromFiles(PGHP_DIR"/shaders/blinn.vert", PGHP_DIR"/shaders/blinn.frag");
    mSimple.loadFromFiles(PGHP_DIR"/shaders/simple.vert", PGHP_DIR"/shaders/simple.frag");
    mHole.loadFromFiles(PGHP_DIR"/shaders/hole.vert", PGHP_DIR"/shaders/hole.frag");
    mMesh.loadFromFiles(PGHP_DIR"/shaders/mesh.vert", PGHP_DIR"/shaders/mesh.frag");

    //PointCloud
    pc = new PointCloud();
    pc->load(PGHP_DIR"/data/decimate.asc");
    pc->makeUnitary();
    pc->init(&mBlinn);

    //Mesh
    mesh = new Mesh();
    //mesh->load(PGHP_DIR"/data/PhantomUgly.obj");
    //mesh->load(PGHP_DIR"/data/Kate.obj");
    //mesh->load(PGHP_DIR"/data/PhantomLite.obj");
    //mesh->load(PGHP_DIR"/data/bunny.obj");
    //mesh->load(PGHP_DIR"/data/sphere.obj");
    //mesh->load(PGHP_DIR"/data/sphere2.obj");
    //mesh->load(PGHP_DIR"/data/trouComplexe5.obj");
    //mesh->load(PGHP_DIR"/data/HoleMoreComplex.obj");
    mesh->load(PGHP_DIR"/data/HoleMoreComplex1.obj");
    mesh->makeUnitary();
    mesh->init(&mBlinn);

    //Octree
    octree = new Octree(pc,15,10);
    wirecube = new WireCube();
    wirecube->init(&mSimple);

    mCamera.setSceneCenter(Vector3f(0.0,0.0,0.0));
    mCamera.setSceneDistance(4);
    mCamera.setMinNear(0.1f);
    mCamera.setNearFarOffsets(-200.f,200.0f);
    mCamera.setScreenViewport(AlignedBox2f(Vector2f(0.0,0.0), Vector2f(WIDTH,HEIGHT)));

    GL_TEST_ERR;
}

/** This method is called in the main rendering loop everytime the OpenGL windows has to be refreshed. */
void render(GLFWwindow* window)
{
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear all buffers

    //Draw PointCloud
    mBlinn.activate();

    glUniformMatrix4fv(mBlinn.getUniformLocation("projection_matrix"),1,false,mCamera.computeProjectionMatrix().data());
    glUniformMatrix4fv(mBlinn.getUniformLocation("modelview_matrix"),1,false,mCamera.computeViewMatrix().data());
    Vector4f light_pos;
    light_pos << mLightPos , 1.0f;
    Vector4f light_pos2;
    light_pos2 << mLightPos2 , 1.0f;
    glUniform4fv(mBlinn.getUniformLocation("light_pos"),1,light_pos.data());
    glUniform4fv(mBlinn.getUniformLocation("light_pos2"),1,light_pos2.data());

    glUniformMatrix4fv(mBlinn.getUniformLocation("object_matrix"),1,false,/*pc*/mesh->getTransformationMatrix().data());
    Matrix3f normal_matrix = (mCamera.computeViewMatrix()*/*pc*/mesh->getTransformationMatrix()).linear().inverse().transpose();
    glUniformMatrix3fv(mBlinn.getUniformLocation("normal_matrix"),1,false,normal_matrix.data());

    mesh->draw(&mBlinn/*,true*/);
    //mesh->drawEdges(&mHole);
    //pc->draw(&mBlinn);


//    Vector4f light_pos_mesh;

//    mMesh.activate();
//    glUniformMatrix4fv(mMesh.getUniformLocation("projection_matrix"),1,false,mCamera.computeProjectionMatrix().data());
//    glUniformMatrix4fv(mMesh.getUniformLocation("modelview_matrix"),1,false,mCamera.computeViewMatrix().data());

//    light_pos_mesh << mLightPos , 1.0f;
//    glUniform4fv(mMesh.getUniformLocation("light_pos"),1,light_pos_mesh.data());

//    glUniformMatrix4fv(mMesh.getUniformLocation("object_matrix"),1,false,mesh->getTransformationMatrix().data());
//    Matrix3f normal_matrix = (mCamera.computeViewMatrix()*mesh->getTransformationMatrix()).linear().inverse().transpose();
//    glUniformMatrix3fv(mMesh.getUniformLocation("normal_matrix"),1,false,normal_matrix.data());



//    mesh->draw(&mMesh);

    //Draw Octree
    if(octreeVisu >= 0)
    {
        mSimple.activate();
        glUniformMatrix4fv(mSimple.getUniformLocation("projection_matrix"),1,false,mCamera.computeProjectionMatrix().data());
        glUniformMatrix4fv(mSimple.getUniformLocation("modelview_matrix"),1,false,mCamera.computeViewMatrix().data());
        std::vector<AlignedBox3f> aabbs = octree->getAABBs(octreeVisu);
        for(unsigned i=0; i<aabbs.size(); ++i)
        {
            Affine3f object_matrix;
            object_matrix = Translation3f(aabbs[i].center()) * Scaling((aabbs[i].max() - aabbs[i].min())/2.0);
            glUniformMatrix4fv(mSimple.getUniformLocation("object_matrix"),1,false, object_matrix.data());
            wirecube->draw(&mSimple);
        }
    }

    mHole.activate();
    glUniformMatrix4fv(mHole.getUniformLocation("object_matrix"),1,false,mesh->getTransformationMatrix().data());
    glUniformMatrix3fv(mHole.getUniformLocation("normal_matrix"),1,false,normal_matrix.data());
    //mesh->drawEdges(&mHole);

    // check OpenGL errors
    GL_TEST_ERR;

    glfwSwapBuffers(window);
}

/** This method is automatically called by GLFW everytime a mouse button is pressed */
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
    if(action == GLFW_PRESS) {
        if(button == GLFW_MOUSE_BUTTON_LEFT)
        {
            mCamera.startRotation(mLastMousePos);
        }
        else if(button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            mCamera.startTranslation(mLastMousePos);
        }
        mButton = button;       
    }else if(action == GLFW_RELEASE) {
        if(mButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            mCamera.endRotation();
        }
        else if(mButton == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            mCamera.endTranslation();
        }
        mButton = -1;
    }
}

/** This method is automatically called by GLFW everytime the scrollwheel is used */
static void scroll_callback(GLFWwindow* window, double x, double y)
{
    mCamera.zoom((y>0)? 1.1: 1./1.1);
}

/** This method is automatically called by GLFW everytime the mouse moves */
static void cursorPos_callback(GLFWwindow* window, double x, double y)
{
    if(mButton == GLFW_MOUSE_BUTTON_LEFT) 
    {
        mCamera.dragRotate(Vector2f(x,y));
    }
    else if(mButton == GLFW_MOUSE_BUTTON_MIDDLE) 
    {
        mCamera.dragTranslate(Vector2f(x,y));
    }
    mLastMousePos = Vector2f(x,y);
}

/** This method is automatically called by GLFW everytime a key is pressed */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
            glfwSetWindowShouldClose(window, GL_TRUE);
        else if(key == GLFW_KEY_R)
            mBlinn.loadFromFiles(PGHP_DIR"/shaders/blinn.vert", PGHP_DIR"/shaders/blinn.frag");
        else if(key == GLFW_KEY_RIGHT)
        {
            if(octreeVisu < 10)
                octreeVisu++;
        }
        else if(key == GLFW_KEY_LEFT)
        {
            if(octreeVisu > -1)
                octreeVisu--;
        }
        else if(key == GLFW_KEY_D)
        {
            octree->decimateOneDepth();

            pc = new PointCloud(octree->getPositions(), octree->getNormals());
            pc->init(&mBlinn);
        }
        else if(key == GLFW_KEY_F)
        {
            std::cout << "starting number of points " << pc->numPoints() << std::endl;

            octree->decimate(pc->numPoints()/10.0f);
            pc = new PointCloud(octree->getPositions(), octree->getNormals());
            pc->init(&mBlinn);

            std::cout << "ending number of points " << pc->numPoints() << std::endl;
        }
        else if(key == GLFW_KEY_X)
        {


            mesh->detectHole(&mHole);
            mesh->init(&mBlinn);
            //mesh->displayHoles();





        }

        else if(key == GLFW_KEY_B)
        {

            //mesh->holeTriangulation();
            mesh->fillHole(0);
            mesh->init(&mBlinn);



        }


        else if(key == GLFW_KEY_N)
        {
            mesh->detectHole(&mHole);
            mesh->init(&mBlinn);
//            mesh->earClimpy();
            mesh->fillHole(1);
            mesh->init(&mBlinn);

        }

        else if(key == GLFW_KEY_P)
        {
//            Vector2f A(2,-3);
//            Vector2f B(3,1);
//            Vector2f C(-1,4);

//            Vector2f AB = B-A;
//            Vector2f AC = C-A;

//            double cosBAC=( AB.dot(AC) ) / (AB.norm() * AC.norm());

//            double BAC=acos(cosBAC);

//            double angle = (BAC/M_PI) * 180;

//            std::cout << BAC << std::endl;

//            Vector3f A(0,0,0);
//            Vector3f B(3,0,0);
//            Vector3f C(1.5,3,0);

//            Vector3f M(1.5,1.5,0);
//            Vector3f N(5,5,5);

//            Vector3f AB = B-A;
//            Vector3f AM = M-A;
//            Vector3f AN = N-A;
//            Vector3f AC = C-A;

//            Vector3f BA = A-B;
//            Vector3f BM = M-B;
//            Vector3f BN = N-B;
//            Vector3f BC = C-B;

//            Vector3f CA = A-C;
//            Vector3f CM = M-C;
//            Vector3f CN = N-C;
//            Vector3f CB = B-C;

//            float v0m = (AB.cross(AM)).dot(AM.cross(AC));
//            float v1m = (BA.cross(BM)).dot(BM.cross(BC));
//            float v2m = (CA.cross(CM)).dot(CM.cross(CB));

//            float v0n = (AB.cross(AN)).dot(AN.cross(AC));
//            float v1n = (BA.cross(BN)).dot(BN.cross(BC));
//            float v2n = (CA.cross(CN)).dot(CN.cross(CB));

//            if(v0m>=0 && v1m>=0 && v2m>=0)
//                std::cout<< "M appartient à ABC" << std::endl;
//            else
//                std::cout<< "M n'appartient pas à ABC" << std::endl;

//            if(v0n>=0 && v1n>=0 && v2n>=0)
//                std::cout<< "N appartient à ABC" << std::endl;
//            else
//                std::cout<< "N n'appartient pas à ABC" << std::endl;

              Vector3f A(1,0);
              Vector3f B(0,1);

              Vector3f C = A.cross(B);
              Vector3f D = B.cross(A);

              std::cout<< "C="<< C << std::endl<<std::endl;

              std::cout<< "D="<< D << std::endl;




        }






    }
}

/** This method is automatically called by GLFW everytime the OpenGL windows is resized.
      * \param width the new width of the windows (in pixels)
      * \param height the new height of the windows (in pixels)
      */
static void reshape_callback(GLFWwindow* window, int width, int height)
{
    // configure the rendering target size (viewport)
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, width, height);
    mCamera.setScreenViewport(AlignedBox2f(Vector2f(0.0,0.0), Vector2f(width,height)));
    render(window);
}

/** This method is automatically called by GLFW when an error occurs */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

/** This method uses GLFW to create a window and the OpenGL context */
GLFWwindow* initGLFW()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Modelisation Geométrique", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Print some useful information about the OpengGL version in use
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
    glfwSetCursorPosCallback(window, cursorPos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    return window;
}

int main(int argc, char *argv[])
{
	GLFWwindow* window = initGLFW();

    initGL();

    while (!glfwWindowShouldClose(window))
    {
        render(window);

        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
