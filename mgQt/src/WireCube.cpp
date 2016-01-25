#include "WireCube.h"

// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// vertex coords array for glDrawArrays() =====================================
// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
// of 36 vertices (6 sides * 2 tris * 3 vertices = 36 vertices). And, each
// vertex is 3 components (x,y,z) of floats, therefore, the size of vertex
// array is 108 floats (36 * 3 = 108).

GLfloat vertices[]  = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,
                        1,-1,-1,   1, 1,-1,  -1, 1,-1,  -1,-1,-1};

GLint edges[] = { 0 , 1 , 1 , 2 , 2 , 3 , 3 , 4 , 4 , 5 , 5 , 6 , 6 , 7 ,
                  0 , 5 , 1 , 6 , 2 , 7 ,
                  0 , 3 , 4 , 7};

WireCube::WireCube()
{
}

WireCube::~WireCube()
{
    glDeleteBuffers(1,&mPosBuffer);
    glDeleteBuffers(1,&mEdges);
}

void WireCube::init(Shader *shader)
{
    glGenVertexArrays(1, (GLuint *)&mVertexArrayObject);

    glBindVertexArray(mVertexArrayObject);

    glGenBuffers(1, &mEdges);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEdges);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*24, edges,  GL_STATIC_DRAW);

    glGenBuffers(1, &mPosBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*24, vertices, GL_STATIC_DRAW);

    specifyVertexData(shader);

    glBindVertexArray(0);

    GL_TEST_ERR;

    mReady = true;
}

void WireCube::specifyVertexData(Shader *shader)
{
    mShader = shader;

    glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);

    // Recuperation des emplacements des deux attributs dans le shader
    int vboIdx = shader->getAttribLocation("vtx_position");

    glVertexAttribPointer(vboIdx, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vboIdx);
}

void WireCube::draw(Shader *shader, bool drawEdges)
{
    if (!mReady) {
        std::cerr<<"Warning: Cube not ready for rendering" << std::endl;
        return;
    }

    glBindVertexArray(mVertexArrayObject);

    if(mShader->id() != shader->id()){
        specifyVertexData(shader);
    }

    glDrawElements(GL_LINES, 24*sizeof(int),  GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
