#ifndef _SHAPE_H
#define _SHAPE_H

#include "Shader.h"

#include <eigen3/Eigen/Geometry>

class Shape {
public:
    Shape() : mReady(false), mTransformation(Eigen::Matrix4f::Identity()) {}

    virtual void init(Shader *shader) = 0;
    virtual void draw(Shader *shader, bool drawEdges = false) = 0;

    const Eigen::Affine3f& getTransformationMatrix() const { return mTransformation; }
    void setTransformationMatrix(const Eigen::Affine3f& transfo) { mTransformation = transfo; }

protected:
	virtual void specifyVertexData(Shader *shader) = 0;

    bool mReady;

    Eigen::Affine3f mTransformation;

    Shader *mShader;
};

#endif // _SHAPE_H
