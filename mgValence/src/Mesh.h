#ifndef MESH_H
#define MESH_H

#include "Pointcloud.h"

#include <surface_mesh/surface_mesh.h>

class Mesh : public PointCloud
{
public:
    Mesh() {}
    ~Mesh();
    void load(const std::string& filename);
    void init(Shader *shader);
    void draw(Shader *shader, bool drawEdges = false);

    surface_mesh::Surface_mesh mHalfEdge;

private:

    std::vector<Eigen::Vector3i> mIndices;


    GLuint mIndicesBuffer;
};


#endif // MESH_H
