#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "Shape.h"

#include <string>
#include <vector>

class PointCloud : public Shape
{
public:
    PointCloud(){}
    PointCloud(std::vector<Eigen::Vector3f> positions,
               std::vector<Eigen::Vector3f> normals)
               : mPositions(positions), mNormals(normals){}

    ~PointCloud();
    void load(const std::string& filename);
    void init(Shader *shader);
    void draw(Shader *shader, bool drawEdges = false);

    void makeUnitary();

    const std::vector<Eigen::Vector3f>& getPositions() const;
    const std::vector<Eigen::Vector3f>& getNormals() const;
    const std::vector<Eigen::Vector3f>& getColors() const;


    int numPoints() const {return mPositions.size();}

protected:
    void specifyVertexData(Shader *shader);

    std::vector<Eigen::Vector3f> mPositions;
    std::vector<Eigen::Vector3f> mNormals;
    std::vector<Eigen::Vector3f> mColors;

    GLuint mVao;
    GLuint mBufs[3];//Positions,Normals, Colors
};

#endif // POINTCLOUD_H
