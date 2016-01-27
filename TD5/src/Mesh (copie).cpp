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
        mDefaultColor = Vector3f(1.,1.,1.);
        mColors.push_back(mDefaultColor);
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
    mCurrentShader = shader;
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





//Composantes connexes *****************************************************************************
void Mesh::nbConnexTest2()
{
    //remise a zero de mConnex pour éviter les doublons
    mConnex.erase(mConnex.begin(),mConnex.end());

    vector<Surface_mesh::Vertex> component, tmpComp;

    Surface_mesh::Edge_iterator eit;
    Surface_mesh::Vertex_iterator vit;
    Surface_mesh::Vertex_around_vertex_circulator vvit,vvend;

    Surface_mesh::Vertex v,v0,v1,v2;


    bool isCon,in, in0, in1;


    for(vit=mHalfEdge.vertices_begin(); vit!=mHalfEdge.vertices_end(); ++vit)
    {
        //////////////////////////////////
        cout <<"~~~~~~~~~~~~~~"<< endl;
        cout<< "->" <<*vit<<":"<<endl;

        vvit=vvend=mHalfEdge.vertices(*vit);
        do{
            v=*vvit;
            cout<<v<<endl;
            ++vvit;
          }while(vvit!=vvend);

////////////////////////////////////////////////////////////
//        isCon=false;

//        vvit=vvend=mHalfEdge.vertices(*vit);
//        //v=*vvit;

//        if(vit==mHalfEdge.vertices_begin())
//        {
//            component.push_back(*vit);

//            do{
//                v=*vvit;
//                component.push_back(v);
//                ++vvit;
//            }while(vvit!=vvend);

//        }

//        else
//        {
//           tmpComp.push_back(*vit);
//           do{
//               v=*vvit;
//               tmpComp.push_back(v);
//               ++vvit;
//           }while(vvit!=vvend);


//           for(int i=0; i<component.size();++i)
//           {

//               for(int j=0; j<tmpComp.size();++j)
//               {

//                   if(component[i]==tmpComp[j])
//                   {
//                       isCon=true;
//                       j=tmpComp.size();
//                       i=component.size();
//                   }
//               }
//           }

//           if(isCon==true)
//           {
//                for(int j=0; j<tmpComp.size();++j)
//                {
//                    in=false;

//                    for (int i=0; i<component.size();++i)
//                    {
//                        if(tmpComp[j]==component[i])
//                        {
//                            in=true;
//                            i=component.size();
//                        }
//                    }

//                    if(in==false)
//                        component.push_back(tmpComp[j]);

//                }
//           }
//           else
//           {
//               mConnex.push_back(component);
//               component=tmpComp;

//           }
//           tmpComp.erase(tmpComp.begin(),tmpComp.end());



//        }

            ////////////////////////////////////////////


    }
    mConnex.push_back(component);
    //cout <<"~~~~~~~~~~~~~~"<< endl;
    cout <<"nb composantes connexes :"<< mConnex.size() << endl;

    //color
    for(int i=0; i<mConnex.size();++i)
    {

        for(int j=0; j<mConnex[i].size();++j)
        {
            for(int k=0; k<posVert.size();++k)
            {
                if(posVert[k]==mConnex[i][j])
                    mColors[k]=Vector3f(float(i)/(mConnex.size()-1),float(i)/(mConnex.size()-1),float(i)/(mConnex.size()-1));


            }
        }

    }
    Mesh::init(mCurrentShader);

}
///test2//////////////////////////////////////
//Composantes connexes *****************************************************************************
void Mesh::nbConnexTest()
{

    //remise a zero de mConnex pour éviter les doublons
    mConnex.erase(mConnex.begin(),mConnex.end());

    vector<Surface_mesh::Vertex> component, tmpComp, stockVertices;

    Surface_mesh::Edge_iterator eit;
    Surface_mesh::Vertex_iterator vit;
    Surface_mesh::Vertex_around_vertex_circulator vvit,vvend;

    Surface_mesh::Vertex v,v0,v1,v2;


    bool isCon,in, in0, in1;
    //int ind;


    for(vit=mHalfEdge.vertices_begin(); vit!=mHalfEdge.vertices_end(); ++vit)
    {
//////////////////////////////////////////////////////////////
//        cout <<"~~~~~~~~~~~~~~"<< endl;
//        cout<< "->" <<*vit<<":"<<endl;

//        vvit=vvend=mHalfEdge.vertices(*vit);
//        do{
//            v=*vvit;
//            cout<<v<<endl;
//            ++vvit;
//          }while(vvit!=vvend);

////////////////////////////////////////////////////////////
        isCon=false;

        vvit=vvend=mHalfEdge.vertices(*vit);
        //v=*vvit;

        if(vit==mHalfEdge.vertices_begin())
        {

            component.push_back(*vit);

            do{
                v=*vvit;
                component.push_back(v);
                ++vvit;
            }while(vvit!=vvend);
            mConnex.push_back(component);
        }

        else
        {
           tmpComp.erase(tmpComp.begin(),tmpComp.end());
           tmpComp.push_back(*vit);
           do{
               v=*vvit;
               tmpComp.push_back(v);
               ++vvit;
           }while(vvit!=vvend);

            //Verifie si connecté à un sommet déjà parcouru
           int ind=-1;
           for(int i=0; i<mConnex.size();++i)
           {

                for(int j=0;j<mConnex[i].size();++j)
                {

                    for(int k=0; k<tmpComp.size();++k)
                    {

                        if(mConnex[i][j]==tmpComp[k])
                        {
                            ind=i;
                            k=tmpComp.size();
//                          j=mConnex[i].size();
//                          i=mConnex.size();
                        }

                    }

                    //if(ind!=-1)
                    //    break;
                }
                //if(ind!=-1)
                //    break;
           }

           //si connecté alors remplir la composante concernée
           if(ind!=-1)
           {

             for(int i=0; i<tmpComp.size();++i)
             {
                 in=false;

                 for(int j=0; j<mConnex[ind].size();++j)
                 {
                    if(tmpComp[i]==mConnex[ind][j])
                    {
                        in=true;
                        j=mConnex[ind].size();
                    }
                 }

                 if(in==false)
                 {
                     mConnex[ind].push_back(tmpComp[i]);
                 }

             }
           }
           else //Sinon, création d'une nouvelle composante
           {

            mConnex.push_back(tmpComp);

           }





           //////





        }

            ////////////////////////////////////////////


    }
    //mConnex.push_back(component);
    //cout <<"~~~~~~~~~~~~~~"<< endl;
    cout <<"nb composantes connexes :"<< mConnex.size() << endl;

//    //color
//    for(int i=0; i<mConnex.size();++i)
//    {

//        for(int j=0; j<mConnex[i].size();++j)
//        {
//            for(int k=0; k<posVert.size();++k)
//            {
//                if(posVert[k]==mConnex[i][j])
//                    mColors[k]=Vector3f(float(i)/(mConnex.size()-1),float(i)/(mConnex.size()-1),float(i)/(mConnex.size()-1));


//            }
//        }

//    }
//    Mesh::init(mCurrentShader);

}


///////////////////////////////////////////////
//Afficher les sommets des Composantes connexes******************************************************************

void Mesh::displayConnex()
{
    std::cout << "___________" << std::endl;
    for(int i=0; i<mConnex.size();++i)
    {
        for(int j=0; j<mConnex[i].size(); ++j)
        {
            std::cout << mConnex[i][j] << std::endl;
        }
        std::cout << "___________" << std::endl;
    }

}
/************************************************************************************************/
//Detection de trou**********************************************************************************

void Mesh::detectHole()
{
    //remise a zero de mHoles pour éviter les doublons
    mHoles.erase(mHoles.begin(),mHoles.end());


    //std::vector<Vector3f> stockPos;

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


                mHoles.push_back(stockHole);
                nbHole++;
            }

        }


    }



    std::cout<<"nbHole="<<mHoles.size()<<std::endl;

    if(mHoles.empty())
    {
        for (int i=0; i<posVert.size(); ++i)
        {
         mColors[i]=mDefaultColor;
        }
    }

    else
      {
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
    Mesh::init(mCurrentShader);

}

//Afficher les sommets des trous******************************************************************

void Mesh::displayHoles()
{
    std::cout << "___________" << std::endl;
    for(int i=0; i<mHoles.size();++i)
    {
        for(int j=0; j<mHoles[i].size(); ++j)
        {
            std::cout << mHoles[i][j] << std::endl;
        }
        std::cout << "___________" << std::endl;
    }

}
/************************************************************************************************/
void Mesh::holeTriangulation(vector<Surface_mesh::Vertex> &hole)
{
//    vector<Surface_mesh::Vertex> hole;
//    hole=mHoles[2];

    //calcu geocentre
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

        Point vG(geoCenter[0],geoCenter[1],geoCenter[2]);


        mHalfEdge.add_vertex(vG);
        Surface_mesh::Vertex_iterator vit;
        vit=--mHalfEdge.vertices_end();
        Surface_mesh::Vertex vertG = *vit;

        posVert.push_back(vertG);
        //triangulation


            for(int i=0; i<hole.size()-1;++i)
            {

                mIndices.push_back(Vector3i(hole[i].idx(), vertG.idx(), hole[i+1].idx()));
                mHalfEdge.add_triangle(hole[i], hole[i+1],vertG);
            }


            mIndices.push_back(Vector3i(hole[hole.size()-1].idx(), vertG.idx(), hole[0].idx()));
            mHalfEdge.add_triangle(hole[hole.size()-1], hole[0], vertG);



}


//EarClimpy***********************************************************************************************************/

//********

Vector3f getCoordVertex(Surface_mesh::Vertex v, vector<Surface_mesh::Vertex> &vert, vector<Vector3f> &coord)
{
    Vector3f vector;
    for (int i=0; i<vert.size(); ++i)
    {
        if(v==vert[i])
        {
            vector=coord[i];
            i=vert.size();
        }
    }

    return vector;
}

//********
Vector3f calcGeoCenter(vector<Surface_mesh::Vertex> &hole, vector<Surface_mesh::Vertex> &vert, vector<Vector3f> &coord)
{
    Vector3f G(0.,0.,0.);

    for(int i = 0; i<hole.size(); ++i)
    {
        Vector3f V=getCoordVertex(hole[i],vert,coord);
        G+=V;
    }

    G=G/hole.size();

    return G;

}

//********
bool isInTriangle(Vector3f& A, Vector3f& B, Vector3f& C, Vector3f& M)
{
    Vector3f AB = B-A;
    Vector3f AM = M-A;
    Vector3f AC = C-A;

    Vector3f BA = A-B;
    Vector3f BM = M-B;
    Vector3f BC = C-B;

    Vector3f CA = A-C;
    Vector3f CM = M-C;
    Vector3f CB = B-C;

    float v0m = (AB.cross(AM)).dot(AM.cross(AC));
    float v1m = (BA.cross(BM)).dot(BM.cross(BC));
    float v2m = (CA.cross(CM)).dot(CM.cross(CB));

    if(v0m>=0 && v1m>=0 && v2m>=0)
        return true;
    else
        return false;

}





//*********
bool isEar(Surface_mesh::Vertex& v0, Surface_mesh::Vertex& v1, Surface_mesh::Vertex& v2,
           vector<Surface_mesh::Vertex> &vert , vector<Vector3f> &coord, Vector3f& G,
           vector<Surface_mesh::Vertex> &hole)
{
    bool validAngle = false;
    bool inHole = false;
    bool notIntersec = true;

    Vector3f vec0=getCoordVertex(v0,vert,coord);
    Vector3f vec1=getCoordVertex(v1,vert,coord);
    Vector3f vec2=getCoordVertex(v2,vert,coord);

    //contrainte d'angle
    Vector3f AB = vec1-vec2;
    Vector3f AC = vec0-vec1;

    double cosBAC=( AB.dot(AC) ) / (AB.norm() * AC.norm());

    double angle=acos(cosBAC);
    std::cout << "pi="<<M_PI<<" et angle ("<<v0<<","<<v1<<","<<v2<<") ="<<angle<<std::endl;

    if(angle < M_PI)
    {
        validAngle = true;
    }

    //contrainte d'appartenance au trou


    Vector3f m = (vec0+vec2)/2;
//    std::cout << "v0=" << vec0 << std::endl;
//    std::cout << "v2=" << vec2 << std::endl;
//    std::cout << "m="  << m << std::endl;
    if((isInTriangle(vec1,G,vec0,m)==true) || (isInTriangle(vec1,G,vec2,m)==true) )
    {
          inHole=true;
          ///////////////////////////////
          ////std::cout << "("<<v0<<", "<<v1<<", "<<v2<<") est à l'interieur du trou" << std::endl;
          ///////////////////////////////

    }

    if(validAngle==true && inHole==true)
    {
         //contrainte d'intersection
         vector<Surface_mesh::Vertex> otherVert;

         for(int i=0; i<hole.size();++i)
         {

             if(hole[i]!=v0 && hole[i]!=v1 && hole[i]!=v2)
                 otherVert.push_back(hole[i]);
         }


         for(int j=0; j<otherVert.size(); ++j)
         {
            Vector3f M = getCoordVertex(otherVert[j],vert,coord);

            if (isInTriangle(vec0,vec1,vec2,M)==true)
            {
                notIntersec=false;
                j=otherVert.size();

            }


         }

         return notIntersec;


    }
    else
    {   //std::cout << "("<<v0<<", "<<v1<<", "<<v2<<") intersecte une face extérieure, Oreille Invalide" << std::endl;
        return false;
    }


}

//**********
void twoNextVert(Surface_mesh::Vertex& v0, Surface_mesh::Vertex& v1, Surface_mesh::Vertex& v2, vector<Surface_mesh::Vertex> &hole)
{
    int ind;
    for(int i=0; i<hole.size();++i)
    {
        if(hole[i]==v0)
        {
            ind=i;
            i=hole.size();
        }
    }


    if(ind<hole.size()-2)
    {
        v1=hole[ind+1];
        v2=hole[ind+2];
    }
    else
    {
        if(ind==hole.size()-2)
        {
            v1=hole[ind+1];
            v2=hole[0];
        }
        else
        {
            v1=hole[0];
            v2=hole[1];
        }
    }
}
//*********
void Mesh::earClimpy(vector<Surface_mesh::Vertex> &hole)
{
    //vector<Surface_mesh::Vertex> hole;
    vector<Surface_mesh::Vertex> newHole;
    //hole=mHoles[2];
    newHole=hole;
    vector<Vector3f> coordVertHole;
    Vector3f geoCenter(0.,0.,0.);


       for(int i=0; i<hole.size();++i)
       {
            for(int j=0; j<posVert.size();++j)
           {

                if(hole[i]==posVert[j])
               {
                coordVertHole.push_back(mPositions[j]);
                j=posVert.size();

               }
           }
       }



     geoCenter = calcGeoCenter(newHole,hole,coordVertHole);


      Surface_mesh::Vertex v0, v1, v2;

      bool findEar/*=true*/;

      while(newHole.size()>3 )
      {
          findEar=false;

            for(int i=0; i<newHole.size(); i++)
            {

                v0=newHole[i];
                twoNextVert(v0,v1,v2,newHole);
                ///////////////////////////////
                std::cout <<"Sommet de départ :"<< v0 << std::endl;
                ///////////////////////////////
                if(isEar(v0,v1,v2,hole,coordVertHole,geoCenter,newHole)==true)
                {
                    findEar=true;
                    i=newHole.size();
                }

            }
            if(findEar==true)
            {

                while(isEar(v0,v1,v2,hole,coordVertHole,geoCenter,newHole)==true)
                {

                    mIndices.push_back(Vector3i(v0.idx(), v1.idx(), v2.idx()));
                    mHalfEdge.add_triangle(v0,v1,v2);
                    //std::cout << "("<<v0<<", "<<v1<<", "<<v2<<") est une oreille valide, Construction de la face" << std::endl;
                    vector<Surface_mesh::Vertex> tmpHole;

                    for (int j=0; j<newHole.size();++j)
                    {

                        if(newHole[j]!=v1)
                            tmpHole.push_back(newHole[j]);
                    }

                    newHole=tmpHole;
                    geoCenter = calcGeoCenter(newHole,hole,coordVertHole);
                    twoNextVert(v0,v1,v2,newHole);

                }




            }
            else
            {

                std::cout<<"pas d'oreille"<<std::endl;
                return;

            }
      }

      if(findEar=true)
      {v0=newHole[0];
      v1=newHole[1];
      v2=newHole[2];
      mIndices.push_back(Vector3i(v0.idx(), v1.idx(), v2.idx()));
      mHalfEdge.add_triangle(v0,v1,v2);
    }

}

//Réparation des trous***********************************************
void Mesh::fillHole(int choix)
{
    detectHole();

    if(choix==0)
    {
        for (int i=0; i<mHoles.size(); ++i)
            Mesh::holeTriangulation(mHoles[i]);
    }
    if(choix==1)
    {
        for (int i=0; i<mHoles.size(); ++i)
            Mesh::earClimpy(mHoles[i]);
    }


    init(mCurrentShader);
}

//*******************************************************************





