#include "Octree.h"

using namespace Eigen;

Octree::Octree(const PointCloud * const pc, int maxDepth, int cellSize)
    : mMaxDepth(maxDepth), mCellSize(cellSize)
{
    mPositions = pc->getPositions();
    mNormals = pc->getNormals();
    build();
}

AlignedBox3f Octree::computeAABB(const std::vector<Vector3f>& positions)
{
    Eigen::AlignedBox3f aabb;
    aabb.setNull();
    for(unsigned i=0; i<positions.size(); ++i)
          aabb.extend(positions[i]);
    return aabb;
}

AlignedBox3f Octree::computeSubAABB(const AlignedBox3f& aabb, short x, short y, short z)
{
    float coords[9];
    Vector3f m = aabb.min();
    Vector3f M = aabb.max();
    Vector3f c = aabb.center();
    coords[0] = m.x();
    coords[1] = c.x();
    coords[2] = M.x();
    coords[3] = m.y();
    coords[4] = c.y();
    coords[5] = M.y();
    coords[6] = m.z();
    coords[7] = c.z();
    coords[8] = M.z();

    return AlignedBox3f(Vector3f(coords[x], coords[y+3], coords[z+6]),
                        Vector3f(coords[x+1], coords[y+4], coords[z+7]));

}

std::vector<Eigen::AlignedBox3f> Octree::getAABBs(int maxDepth)
{
    std::vector<Eigen::AlignedBox3f> aabbs;
    for(unsigned i=0; i<mNodes.size(); ++i)
    {
        Node* n = mNodes[i];
        if(n->depth <= maxDepth)
        {
            aabbs.push_back(n->mAABB);
        }
    }
    return aabbs;
}

void Octree::decimate(int maxNumOfPoints)
{
    while(mPositions.size() > maxNumOfPoints)
    {
        decimateOneDepth();
    }
}


void Octree::decimateOneDepth()
{
    int maxDepth = 0;
    for(unsigned i=0; i<mNodes.size(); ++i)
    {
        if(mNodes[i]->depth > maxDepth) maxDepth = mNodes[i]->depth;
    }

    std::vector<Vector3f> positions;
    std::vector<Vector3f> normals;

    for(unsigned i=0; i<mNodes.size(); ++i)
    {
        Node* n = mNodes[i];
        if(n->isLeaf)
        {
            if(n->depth < maxDepth)
            {
                for(unsigned i=n->idx0; i<n->idx1; ++i)
                {
                    positions.push_back(mPositions[i]);
                    normals.push_back(mNormals[i]);
                }
            }
            else
            {
                Vector3f meanPos(0.0,0.0,0.0);
                Vector3f meanNormal(0.0,0.0,0.0);
                for(unsigned i=n->idx0; i<n->idx1; ++i)
                {
                    meanPos += mPositions[i];
                    meanNormal += mNormals[i];
                }
                meanPos /= n->idx1 - n->idx0;
                meanNormal.normalize();
                positions.push_back(meanPos);
                normals.push_back(meanNormal);
            }
        }
    }

    mPositions = positions;
    mNormals = normals;
    mNodes.clear();

    build();

}

void Octree::build()
{
    Node* root = new Node(computeAABB(mPositions));
    root->depth = 0;
    root->idx0 = 0;
    root->idx1 = mPositions.size();
    mNodes.push_back(root);

    int k=0;
    while(k<mNodes.size())
    {
        Node* node = mNodes[k];
        if(node->idx1 - node->idx0 < mCellSize || node->depth > mMaxDepth)
        {
            node->isLeaf = true;
            k++;
            continue;
        }

        int cursor = node->idx0;
        for(short z=0 ; z < 2 ; z++)
            for(short y=0 ; y < 2 ; y++)
                for(short x=0 ; x < 2 ; x++)
                {
                    Node* child = new Node(computeSubAABB(node->mAABB,x,y,z));
                    child->depth = node->depth + 1;
                    child->idx0 = cursor;
                    for(unsigned i=cursor; i<node->idx1; ++i)
                    {
                        if(child->mAABB.contains(mPositions[i]))
                        {
                            std::swap(mPositions[i], mPositions[cursor]);
                            std::swap(mNormals[i], mNormals[cursor]);
                            cursor++;
                        }
                    }
                    child->idx1 = cursor;
                    if(child->idx1 - child->idx0 == 0)
                    {
                        node->childs[x + 2*y + 4*z] = -1;
                        delete child;
                    }
                    else
                    {
                        node->childs[x + 2*y + 4*z] = mNodes.size();
                        mNodes.push_back(child);
                    }
                }
        k++;
    }
}
