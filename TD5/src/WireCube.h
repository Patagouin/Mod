#ifndef _CUBE_H
#define _CUBE_H

#include "Shape.h"

class WireCube : public Shape
{
public:
    WireCube();
    ~WireCube();

    void init(Shader *shader);
    void draw(Shader *shader, bool drawEdges = true);

protected:
    void specifyVertexData(Shader *shader);

private:
    int mVertexArrayObject;
    GLuint mPosBuffer;
    GLuint mEdges;
};

#endif
