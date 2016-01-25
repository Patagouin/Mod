#ifndef OCTREE_H
#define OCTREE_H

#include "Pointcloud.h"
#include <eigen3/Eigen/Geometry>

class Node
{
    Node(Eigen::AlignedBox3f aabb)
        : mAABB(aabb), idx0(0), idx1(0), depth(0),isLeaf(false){}

protected:
    Eigen::AlignedBox3f mAABB;
    int childs[8];
    int idx0, idx1;
    int depth;
    bool isLeaf;

friend class Octree;
};

class Octree
{
public:
    Octree(PointCloud const * const pc, int maxDepth = 15, int cellSize = 10);

    void build();
    std::vector<Eigen::AlignedBox3f> getAABBs(int maxDepth = 8);

    void decimate(int maxNumOfPoints);
    void decimateOneDepth();
    const std::vector<Eigen::Vector3f>& getPositions(){return mPositions;}
    const std::vector<Eigen::Vector3f>& getNormals(){return mNormals;}

private:
    static Eigen::AlignedBox3f computeAABB(const std::vector<Eigen::Vector3f>& positions);
    static Eigen::AlignedBox3f computeSubAABB(const Eigen::AlignedBox3f& aabb, short x, short y, short z);

    std::vector<Eigen::Vector3f> mPositions;
    std::vector<Eigen::Vector3f> mNormals;
    std::vector<Node*> mNodes;

    int mMaxDepth;
    int mCellSize;
};

#endif // OCTREE_H
