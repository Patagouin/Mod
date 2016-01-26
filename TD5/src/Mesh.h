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
    void initEdges(Shader *shader);
    void detectHole();
    void displayHoles();
    void displayConnex();
    void holeTriangulation(std::vector<surface_mesh::Surface_mesh::Vertex> &hole);
    void earClimpy(std::vector<surface_mesh::Surface_mesh::Vertex> &hole);
    void nbConnexTest();
    void nbConnexTest2();
    void nbConnexTest3();
    void nbConnex();

    void fillHole(int choix);

    surface_mesh::Surface_mesh mHalfEdge;

    std::vector<Eigen::Vector3f> mPositionsHole;
    std::vector<Eigen::Vector3f> mNormalsHole;

    bool mReadyHole;
    std::vector<Eigen::Vector3i> mIndicesHole;

private:

    std::vector<Eigen::Vector3i> mIndices;
    std::vector<surface_mesh::Surface_mesh::Vertex> posVert;

    std::vector< std::vector<surface_mesh::Surface_mesh::Vertex> > mHoles;
    std::vector< std::vector<surface_mesh::Surface_mesh::Vertex> > mConnex;

    //surface_mesh::Surface_mesh mHalfEdge;

    GLuint mIndicesBuffer;
    GLuint mIndicesBufferHole;

    Eigen::Vector3f mDefaultColor;
    Shader *mShaderHole;
    Shader *mCurrentShader;
    GLuint mVaoHole;
    GLuint mBufsHole[2];//Positions,Normals




};


#endif // MESH_H

