#include "Pointcloud.h"
#include <fstream>

using namespace Eigen;

PointCloud::~PointCloud()
{
    if(mReady)
    {
        //glDeleteBuffers(2, mBufs);
        glDeleteBuffers(3, mBufs);
    }
}

void PointCloud::load(const std::string& filename)
{
    std::cout << "Loading: " << filename << std::endl;

    std::ifstream infile(filename.c_str());

    char buf[256];
    infile.getline(buf, 256);
    infile.getline(buf, 256);

    Vector3f p,n;

    int oneOnTwo = 0;
    while (!infile.eof())
    {
        infile >> p.x() >> p.y() >> p.z() >> n.x() >> n.y() >> n.z();
        mPositions.push_back(p);
        mNormals.push_back(n);
        oneOnTwo = ++oneOnTwo%2;
        Vector3f color = Vector3f((float) oneOnTwo, (float) oneOnTwo, (float) oneOnTwo);
        mColors.push_back(color);
    }
    infile.close();
}

void PointCloud::init(Shader *shader)
{
    std::cout << "Step init of mesh 1" << std::endl;
    glGenVertexArrays(1, &mVao);
    //glGenBuffers(2, mBufs);
    glGenBuffers(3, mBufs);

    glBindVertexArray(mVao);

    glBindBuffer(GL_ARRAY_BUFFER, mBufs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mPositions.size(), mPositions.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mBufs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mNormals.size(), mNormals.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mBufs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mColors.size(), mColors.data(), GL_STATIC_DRAW);

    std::cout << "Step init of mesh 2" << std::endl;
    specifyVertexData(shader);

    std::cout << "Step init of mesh 3" << std::endl;
    glBindVertexArray(0);

    mReady = true;
}

void PointCloud::draw(Shader *shader, bool drawEdges)
{
    if (!mReady) {
        std::cerr<<"Warning: PointCloud not ready for rendering" << std::endl;
        return;
    }

    glBindVertexArray(mVao);
    if(mShader->id() != shader->id()){
        specifyVertexData(shader);
    }

    glDrawArrays(GL_POINTS, 0, mPositions.size());
    GL_TEST_ERR;
    glBindVertexArray(0);
}

void PointCloud::specifyVertexData(Shader *shader)
{
    mShader = shader;

    glBindBuffer(GL_ARRAY_BUFFER, mBufs[0]);
    int pos_loc = shader->getAttribLocation("vtx_position");
    if(pos_loc>=0){
        glEnableVertexAttribArray(pos_loc);
        glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, mBufs[1]);
    int normal_loc = shader->getAttribLocation("vtx_normal");
    if(normal_loc>=0){
        glEnableVertexAttribArray(normal_loc);
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
    }

    glBindBuffer(GL_ARRAY_BUFFER,   mBufs[2] );
    int color_loc = shader->getAttribLocation("vtx_color");
    if(color_loc>=0){
        glEnableVertexAttribArray(color_loc);
        glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0 );
    }
}


void PointCloud::makeUnitary()
{
    // computes the lowest and highest coordinates of the axis aligned bounding box,
    // which are equal to the lowest and highest coordinates of the vertex positions.
    Vector3f lowest, highest;
    lowest.fill(std::numeric_limits<float>::max());   // "fill" sets all the coefficients of the vector to the same value
    highest.fill(-std::numeric_limits<float>::max());

    for(std::vector<Eigen::Vector3f>::iterator v_iter = mPositions.begin() ; v_iter!=mPositions.end() ; ++v_iter)
    {
      // - v_iter is an iterator over the elements of mVertices,
      //   an iterator behaves likes a pointer, it has to be dereferenced (*v_iter, or v_iter->) to access the referenced element.
      // - Here the .aray().min(_) and .array().max(_) operators work per component.
      //
      lowest  = lowest.array().min(v_iter->array());
      highest = highest.array().max(v_iter->array());
    }

    // TODO: appliquer une transformation à tous les sommets de mVertices de telle sorte
    // que la boite englobante de l'objet soit centrée en (0,0,0)  et que sa plus grande dimension soit de 1
    Vector3f center = (lowest+highest)/2.0;
    float m = (highest-lowest).maxCoeff();
    for(unsigned i=0; i<mPositions.size(); ++i)
    {
        mPositions[i] = (mPositions[i] - center) / m;
    }
}

const std::vector<Eigen::Vector3f>& PointCloud::getPositions() const
{
    return mPositions;
}

const std::vector<Eigen::Vector3f>& PointCloud::getNormals() const
{
    return mNormals;
}

const std::vector<Eigen::Vector3f> &PointCloud::getColors() const
{
    return mColors;
}
