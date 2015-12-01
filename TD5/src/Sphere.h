#ifndef _SPHERE_H
#define _SPHERE_H

#include <Eigen/Core>
#include "Shape.h"
#include <vector>

class Sphere : public Shape {

public:
    Sphere(float radius=1.f, int nU=40, int nV=40);
    ~Sphere();
    void init(const Shader *shader);
    void draw() const;
    float radius() const { return mRadius; }

private :
    GLuint _vao;
    GLuint _bufs[6];

    std::vector<int>        mIndices;   /** vertex indices */
    std::vector<eigen::vec3f>	mVertices;  /** 3D positions */
    std::vector<eigen::vec3f>	mColors;    /** colors */
    std::vector<eigen::vec3f>	mNormals;   /** 3D normals */
    std::vector<eigen::vec3f>	mTangents;  /** 3D tangent to surface */
    std::vector<eigen::vec3f>	mTexCoords; /** 2D texture coordinates */

    float mRadius;
};

#endif
