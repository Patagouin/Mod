#include "Mesh.h"
#include "Meshloader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

using namespace std;
using namespace Eigen;
using namespace surface_mesh;

Mesh::~Mesh()
{
    if(mReady){
        glDeleteBuffers(1, &mIndicesBuffer);
        glDeleteVertexArrays(1,&mVao);
    }
}

void Mesh::load(const string& filename)
{
    cout << "Loading: " << filename << endl;

    mHalfEdge.read(filename);
    mHalfEdge.update_face_normals();
    mHalfEdge.update_vertex_normals();

    // vertex properties
    Surface_mesh::Vertex_property<Point> vertices = mHalfEdge.get_vertex_property<Point>("v:point");
    Surface_mesh::Vertex_property<Point> vnormals = mHalfEdge.get_vertex_property<Point>("v:normal");

    // vertex iterator
    Surface_mesh::Vertex_iterator vit;

    for(vit = mHalfEdge.vertices_begin(); vit != mHalfEdge.vertices_end(); ++vit)
    {
        mPositions.push_back(Vector3f(vertices[*vit][0],vertices[*vit][1],vertices[*vit][2]));
        mNormals.push_back(Vector3f(vnormals[*vit][0],vnormals[*vit][1],vnormals[*vit][2]));
        Vector3f color(1.,1.,1.);
        mColors.push_back(color);
        posVert.push_back(*vit);
    }

    // face iterator
    Surface_mesh::Face_iterator fit, fend = mHalfEdge.faces_end();
    // vertex circulator
    Surface_mesh::Vertex_around_face_circulator fvit, fvend;
    Surface_mesh::Vertex v0, v1, v2;
    for (fit = mHalfEdge.faces_begin(); fit != fend; ++fit)
    {
        fvit = fvend = mHalfEdge.vertices(*fit);
        v0 = *fvit;
        ++fvit;
        v2 = *fvit;

        do{
            v1 = v2;
            ++fvit;
            v2 = *fvit;
            mIndices.push_back(Vector3i(v0.idx(), v1.idx(), v2.idx()));
        } while (++fvit != fvend);
    }
}

void Mesh::init(Shader *shader)
{
    PointCloud::init(shader);

    glBindVertexArray(mVao);

    glGenBuffers(1, &mIndicesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3i)*mIndices.size(), mIndices.data(),  GL_STATIC_DRAW);

    glBindVertexArray(0);

    mReady = true;
}

void Mesh::draw(Shader *shader, bool drawEdges) 
{
    if (!mReady) {
        cerr<<"Warning: Mesh not ready for rendering" << endl;
        return;
    }

    glBindVertexArray(mVao);

    if(mShader->id() != shader->id()){
        specifyVertexData(shader);
    }

    drawEdges ? glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ) : glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ) ;
    glDrawElements(GL_TRIANGLES, mIndices.size()*sizeof(Vector3i),  GL_UNSIGNED_INT, 0);
    //glDrawElements(drawEdges ? GL_LINE_LOOP : GL_TRIANGLES, mIndices.size()*sizeof(Vector3i),  GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}



void Mesh::specifyVertexDataHole(Shader *shader)
{
    mShaderHole = shader;

    glBindBuffer(GL_ARRAY_BUFFER, mBufsHole[0]);
    int pos_loc = shader->getAttribLocation("vtx_position");
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, mBufsHole[1]);
    int normal_loc = shader->getAttribLocation("vtx_normal");
    if(normal_loc>=0){
        glEnableVertexAttribArray(normal_loc);
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
    }
}



void Mesh::initEdges(Shader *shader)
{


    glGenVertexArrays(1,&mVaoHole);
    glGenBuffers(2,mBufsHole);

    glBindVertexArray(mVaoHole);

    glBindBuffer(GL_ARRAY_BUFFER,mBufsHole[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mPositionsHole.size(), mPositionsHole.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,mBufsHole[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mNormalsHole.size(), mNormalsHole.data(), GL_STATIC_DRAW);

    specifyVertexDataHole(shader);




//    glBindVertexArray(0);


//    glBindVertexArray(mVaoHole);

    glGenBuffers(1, &mIndicesBufferHole);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBufferHole);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3i)*mIndicesHole.size(), mIndicesHole.data(),  GL_STATIC_DRAW);

    glBindVertexArray(0);

    mReadyHole=true;


}

void Mesh::drawEdges(Shader *shader)
    {

    if (!mReadyHole) {
        //std::cerr<<"Warning: PointCloud not ready for rendering" << std::endl;
        return;
    }
          glBindVertexArray(mVaoHole);
          if(mShaderHole->id() != shader->id()){
              specifyVertexDataHole(shader);
          }

          //glDrawArrays(GL_TRIANGLES, 0, mPositionsHole.size());
          glDrawElements(GL_TRIANGLES, mIndicesHole.size()*sizeof(Vector3i),  GL_UNSIGNED_INT, 0);

          glBindVertexArray(0);



    }

void Mesh::detectHole(Shader *shader)
{


    // vertex properties
    std::vector<Vector3f> stockPos;

    Surface_mesh::Vertex_property<Point> vertices = mHalfEdge.get_vertex_property<Point>("v:point");
    Surface_mesh::Vertex_property<Point> vnormals = mHalfEdge.get_vertex_property<Point>("v:normal");

    Surface_mesh::Halfedge_iterator hei;

    Surface_mesh::Halfedge heNext;

    int nbHole=0;

    vector<Surface_mesh::Edge> stockEdge;
    vector<Surface_mesh::Vertex> stockvertice;

    bool stop;

    for (hei=mHalfEdge.halfedges_begin(); hei!=mHalfEdge.halfedges_end(); ++hei)
    {
        stop=false;
        if(mHalfEdge.is_boundary(*hei))
        {
            Surface_mesh::Edge e;
            Surface_mesh::Vertex vbegin,vend,v0;

            heNext=*hei;
            e=mHalfEdge.edge(heNext);

            if(stockEdge.size()==0)
            {
                stockEdge.push_back(e);
            }

            else
            {
                for(int i=0; i<stockEdge.size();++i)
                {
                    if(e==stockEdge[i])
                    {
                        stop=true;
                        i=stockEdge.size();
                    }

                }
            }

            if(stop==false)
            {
                std::vector<surface_mesh::Surface_mesh::Vertex> stockHole;
                if(stockEdge.size()!=0)
                {
                    stockEdge.push_back(e);
                }
                vbegin=mHalfEdge.vertex(e,0);

                vend  =mHalfEdge.vertex(e,1);
                stockvertice.push_back(vend);
                stockHole.push_back(vend);


                while(vend!=vbegin)
                {
                    heNext=mHalfEdge.next_halfedge(heNext);
                    e=mHalfEdge.edge(heNext);
                    stockEdge.push_back(e);
                    vend=mHalfEdge.vertex(e,1);
                    stockvertice.push_back(vend);
                    stockHole.push_back(vend);
                }



//            std::cout<<"vbegin="<<vbegin<<std::endl;
//            std::cout<<"vend="<<vend<<std::endl;
//            std::cout<<"vbegintest="<<vbegintest<<std::endl;
//            std::cout<<"vend="<<vendtest<<std::endl;

                mHoles.push_back(stockHole);
                nbHole++;
            }

        }


    }



    std::cout<<"nbHole="<<mHoles.size()<<std::endl;

    for(int i=0; i<stockvertice.size();++i)
    {

        for(int j=0; j<posVert.size();++j)
        {
            if(stockvertice[i]==posVert[j])
            {

                mColors[j]=Vector3f(1.,0.,0.);
                j=posVert.size();

            }
        }
    }



}

void Mesh::displayHoles()
{
    for(int i=0; i<mHoles.size();++i)
    {
        for(int j=0; j<mHoles[i].size(); ++j)
        {
            std::cout << mHoles[i][j] << std::endl;
        }
        std::cout << "___________" << std::endl;
    }

}

void Mesh::holeTriangulation()
{
    vector<Surface_mesh::Vertex> hole;
    hole=mHoles[2];

    Vector3f geoCenter(0.,0.,0.);
    Vector3f normGeoC(0.,0.,0.);
    Vector3f colGeo(0.,0.,0.);


        for(int i=0; i<hole.size();++i)
        {

            for(int j=0; j<posVert.size();++j)
            {

                if(hole[i]==posVert[j])
                {

                    geoCenter+=mPositions[j];
                    normGeoC+=mNormals[j];
                    colGeo+=mColors[j];
                    j=posVert.size();

                }
            }
        }

        geoCenter = geoCenter/hole.size();
        normGeoC = normGeoC/hole.size();
        colGeo = colGeo/hole.size();

        mPositions.push_back(geoCenter);
        mNormals.push_back(normGeoC);
        mColors.push_back(colGeo);

        //triangulation

            for(int i=0; i<hole.size()-1;++i)
            {

                mIndices.push_back(Vector3i(hole[i].idx(), mPositions.size()-1, hole[i+1].idx()));
            }


            mIndices.push_back(Vector3i(hole[hole.size()-1].idx(), mPositions.size()-1, hole[0].idx()));


}


void Mesh::earClimpy()
{
 vector<Surface_mesh::Vertex> hole;
 hole=mHoles[2];

Vector3f v0,v1,v2;
 for(int i=0; i<hole.size();++i)
 {
     for(int j=0; j<posVert.size();++j)
     {

         if(hole[i]==posVert[j])
         {

             j=posVert.size();

         }
     }
 }



}

