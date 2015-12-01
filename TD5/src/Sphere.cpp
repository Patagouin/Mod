#include "Sphere.h"

//using namespace glm;
#include <Eigen/Core>

Sphere::Sphere(float radius, int nU, int nV) :
    mRadius(radius)
{
    int nVertices  = (nU + 1) * (nV + 1);
    int nTriangles =  nU * nV * 2;

    mVertices.resize(nVertices);
    mNormals.resize(nVertices);
    mTangents.resize(nVertices);
    mTexCoords.resize(nVertices);
    mColors.resize(nVertices);
    mIndices.resize(3*nTriangles);

    for(int v=0;v<=nV;++v)
    {
        for(int u=0;u<=nU;++u)
        {

            float theta = u / float(nU) * M_PI;
            float phi 	= v / float(nV) * M_PI * 2;

            int index 	= u +(nU+1)*v;

            vec3 vertex, tangent, normal;
            vec2 texCoord;

            // normal
            normal[0] = sin(theta) * cos(phi);
            normal[1] = sin(theta) * sin(phi);
            normal[2] = cos(theta);
            normal = normalize(normal);

            // position
            vertex = normal * mRadius;

            // tangent
            theta += M_PI_2;
            tangent[0] = sin(theta) * cos(phi);
            tangent[1] = sin(theta) * sin(phi);
            tangent[2] = cos(theta);
            tangent = normalize(tangent);

            // texture coordinates
            texCoord[1] = u / float(nU);
            texCoord[0] = v / float(nV);

            mVertices[index] = vertex;
            mNormals[index] = normal;
            mTangents[index] = tangent;
            mTexCoords [index] = texCoord;
            mColors[index] = vec3(0.6f,0.2f,0.8f);
        }
    }

    int index = 0;
    for(int v=0;v<nV;++v)
    {
        for(int u=0;u<nU;++u)
        {
            int vindex 	= u + (nU+1)*v;

            mIndices[index+0] = vindex;
            mIndices[index+1] = vindex+1 ;
            mIndices[index+2] = vindex+1 + (nU+1);

            mIndices[index+3] = vindex;
            mIndices[index+4] = vindex+1 + (nU+1);
            mIndices[index+5] = vindex   + (nU+1);

            index += 6;
        }
    }
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(6, _bufs);
}

void Sphere::init(const Shader *shader)
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(6, _bufs);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _bufs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * mIndices.size(), mIndices.data(),  GL_STATIC_DRAW);

    int vertex_loc = shader->getAttribLocation("vtx_position");
    glEnableVertexAttribArray(vertex_loc);
    glBindBuffer(GL_ARRAY_BUFFER, _bufs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mVertices.size(), mVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    int color_loc = shader->getAttribLocation("vtx_color");
    if(color_loc>=0){
        glEnableVertexAttribArray(color_loc);
        glBindBuffer(GL_ARRAY_BUFFER, _bufs[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mColors.size(), mColors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    int normal_loc = shader->getAttribLocation("vtx_normal");
    if(normal_loc>=0){
        glEnableVertexAttribArray(normal_loc);
        glBindBuffer(GL_ARRAY_BUFFER, _bufs[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mNormals.size(), mNormals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    int tangent_loc = shader->getAttribLocation("vtx_tangent");
    if(tangent_loc>=0){
        glEnableVertexAttribArray(tangent_loc);
        glBindBuffer(GL_ARRAY_BUFFER, _bufs[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mTangents.size(), mTangents.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(tangent_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    int texCoord_loc = shader->getAttribLocation("vtx_texCoord");
    if(texCoord_loc>=0){
        glEnableVertexAttribArray(texCoord_loc);
        glBindBuffer(GL_ARRAY_BUFFER, _bufs[5]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * mTexCoords.size(), mTexCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(texCoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }
    glBindVertexArray(0);

    mReady = true;
}

void Sphere::draw() const
{
    if (!mReady) {
        std::cerr<<"Warning: Sphere not ready for rendering" << std::endl;
        return;
    }

    glBindVertexArray(_vao);

    glDrawElements(GL_TRIANGLES, mIndices.size(),  GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
