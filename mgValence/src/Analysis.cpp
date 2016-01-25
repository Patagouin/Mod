#include "Analysis.h"
#include <iostream>
#include "Meshloader.h"

using namespace std;
using namespace Eigen;
using namespace surface_mesh;

// TODO faire en sorte que mHalfEdge soit rempli

Analysis::Analysis(const std::string& filename, bool _drawModel)
{
    mesh.load(filename);
    mesh.makeUnitary();

    drawModel = _drawModel;

    nbVertices = 0;
    nbFaces = 0;
    nbHoles = 0;
    nbConnectedComponents = 0;
    meanValence = 0;
    Surface_mesh::Vertex_property<Point> vertices = mesh.mHalfEdge.get_vertex_property<Point>("v:point");
    Surface_mesh::Vertex_property<Point> normals = mesh.mHalfEdge.get_vertex_property<Point>("v:normal");
    Surface_mesh::Vertex v0, v1, v2;

    // VALENCE
    Surface_mesh::Vertex_iterator vit;
    Surface_mesh::Vertex_around_vertex_circulator vc, vc_end;
    unsigned int valence;
    unsigned int cpt;
    for (vit = mesh.mHalfEdge.vertices_begin(); vit != mesh.mHalfEdge.vertices_end(); ++vit){
        vc = mesh.mHalfEdge.vertices(*vit);
        vc_end = vc;

        //reset counter
        valence = 0;

        // loop over all incident vertices
        do{
            ++valence;
        }while (++vc != vc_end);
        meanValence += valence;

        //--------------------//

        mVertexValence.push_back(Vector3f(vertices[*vit][0],
                                          vertices[*vit][1],
                                          vertices[*vit][2]));
        mNormalsValence.push_back(Vector3f(normals[*vit][0],
                                           normals[*vit][1],
                                           normals[*vit][2]));
        Vector3f color;
        float HSV[3] = { ((valence*60)%256)/256., 1., 1.}; //
        float RGB[3];
        convertHSVToRGB(HSV, RGB);
        color  = Vector3f(RGB[0]/255., RGB[1]/255., RGB[2]/255.);

        mColorsValence.push_back(color);

        // Index not useful because we send only points and no tris

        mIndicesValenceVertex.push_back(cpt);
        ++cpt;
    }
    meanValence /= mesh.numPoints();

    std::cout << "Mean valence = " << meanValence << std::endl;
    //makeUnitary(mVertexValence);

//    // Boundary
//   // Surface_mesh::Face_iterator fit, fend = mHalfEdge.faces_end();
//    // vertex circulator
//    Surface_mesh::Halfedge_iterator halfEdgeIt;
//    Surface_mesh::Halfedge curEdge;
////    Surface_mesh::Vertex_around_face_circulator fvit, fvend;
//    Surface_mesh::Edge_iterator edgeIt;


//    for (halfEdgeIt = mHalfEdge.halfedges_begin(); halfEdgeIt !=  mHalfEdge.halfedges_end(); ++halfEdgeIt)
//    {


//        curEdge = mHalfEdge.vertices(*halfEdgeIt);
//        if (mHalfEdge.is_boundary(curEdge)){
//            v0 = mHalfEdge.vertex(mHalfEdge.edge(curEdge), 0);
//            v1 = mHalfEdge.vertex(mHalfEdge.edge(curEdge), 1);
//            mBoundaryEdges.push_back(Vector3f(vertices[v0][0],
//                                              vertices[v0][1],
//                                              vertices[v0][2]) );
//            mBoundaryEdges.push_back(Vector3f(vertices[v1][0],
//                                              vertices[v1][1],
//                                              vertices[v1][2]) );

//            mIndicesBoundaryEdges.push_back(Vector2i(v0, v1));



//        }
//    }
}

void Analysis::draw(Shader *shader, bool drawEdges){
    static int nbDrawCall = 0;
    nbDrawCall++;
    std::cout << "nbDrawCall " << nbDrawCall << std::endl;

    if (drawModel)
        mesh.draw(shader, drawEdges);

    // If shader change between init and the draw call
    if(mAnalyseShader->id() != shader->id()){
        specifyVertexData(shader);
    }

//    glBindVertexArray(0);
//    glBindVertexArray(mVaoHoles);
//    glDrawElements(GL_TRIANGLES, mIndicesFacesHoles.size()*sizeof(Vector3i),  GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(mVaoValence);
    glDrawArrays(GL_POINTS, 0, mIndicesValenceVertex.size());
    //glDrawElements(GL_POINTS, mIndicesValenceVertex.size(),  GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_POINTS, mIndicesValenceVertex.size(),  GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);


//    glBindVertexArray(mVaoBoundaryEdge);
//    glDrawElements(GL_LINES, mIndicesBoundaryEdges.size()*sizeof(Vector2i),  GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);


}

void Analysis::init(Shader *shader){
    mesh.init(shader);

//    glGenVertexArrays(1, &mVaoHoles);
//    glGenBuffers(3, mBufsHoles);
//    glBindVertexArray(mVaoHoles);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufsHoles[0]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mPositionsHoles.size(), mPositionsHoles.data(), GL_STATIC_DRAW);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufsHoles[1]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mNormalsHoles.size(), mNormalsHoles.data(), GL_STATIC_DRAW);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufsHoles[2]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mColorsHoles.size(), mColorsHoles.data(), GL_STATIC_DRAW);

//    glBindVertexArray(0);
    glGenVertexArrays(1, &mVaoValence);
    glGenBuffers(3, mBufsValence);
    glBindVertexArray(mVaoValence);

    glBindBuffer(GL_ARRAY_BUFFER, mBufsValence[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mVertexValence.size(), mVertexValence.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mBufsValence[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mNormalsValence.size(), mNormalsValence.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mBufsValence[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mColorsValence.size(), mColorsValence.data(), GL_STATIC_DRAW);

    specifyVertexData(shader);

    glBindVertexArray(0);

//    glGenVertexArrays(1, &mVaoBoundaryEdge);
//    glGenBuffers(1, mBufBoundaryEdge);
//    glBindVertexArray(mVaoBoundaryEdge);


//    glBindBuffer(GL_ARRAY_BUFFER, mBufBoundaryEdge[0]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mVertexBoundaryEdges.size(), mVertexBoundaryEdges.data(), GL_STATIC_DRAW);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufBoundaryEdge[1]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mNormalsBoundaryEdges.size(), mNormalsBoundaryEdges.data(), GL_STATIC_DRAW);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufBoundaryEdge[2]);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3f)*mColorBoundaryEdges.size(), mColorBoundaryEdges.data(), GL_STATIC_DRAW);

//    glBindVertexArray(0);




    mReady = true;


}
void Analysis::specifyVertexData(Shader *shader){
    int loc;
    mAnalyseShader = shader;
    std::cout<< "je suis dans le specify vertex" << std::endl;
//     Valence
    glBindBuffer(GL_ARRAY_BUFFER, mBufsValence[0]);
    loc = shader->getAttribLocation("vtx_position");
    if(loc>=0){
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mBufsValence[1]);
    loc = shader->getAttribLocation("vtx_normal");
    if(loc>=0){
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
    }

    glBindBuffer(GL_ARRAY_BUFFER,   mBufsValence[2] );
    loc = shader->getAttribLocation("vtx_color");
    if(loc>=0){
        glEnableVertexAttribArray(loc);
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0 );
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Holes
//    glBindBuffer(GL_ARRAY_BUFFER, mBufsHoles[0]);
//    loc = shader->getAttribLocation("vtx_position");
//    if(loc>=0){
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufsHoles[1]);
//    loc = shader->getAttribLocation("vtx_normal");
//    if(loc>=0){
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    glBindBuffer(GL_ARRAY_BUFFER,   mBufsHoles[2] );
//    loc = shader->getAttribLocation("vtx_color");
//    if(loc>=0){
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0 );
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    // Boundary Edges
//    glBindBuffer(GL_ARRAY_BUFFER, mBufBoundaryEdge[0]);
//    loc = shader->getAttribLocation("vtx_position");
//    if(loc>=0){
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    glBindBuffer(GL_ARRAY_BUFFER, mBufBoundaryEdge[1]);
//    loc = shader->getAttribLocation("vtx_normal");
//    if(loc>=0){
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0);
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    glBindBuffer(GL_ARRAY_BUFFER,   mBufBoundaryEdge[2] );
//    loc = shader->getAttribLocation("vtx_color");
//    if(loc>=0){
//        glEnableVertexAttribArray(loc);
//        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*)0 );
//    }
//    glBindBuffer(GL_ARRAY_BUFFER, 0);


}
void Analysis::makeUnitary()
{
    // computes the lowest and highest coordinates of the axis aligned bounding box,
    // which are equal to the lowest and highest coordinates of the vertex positions.
    Vector3f lowest, highest;
    lowest.fill(std::numeric_limits<float>::max());   // "fill" sets all the coefficients of the vector to the same value
    highest.fill(-std::numeric_limits<float>::max());

    for(std::vector<Eigen::Vector3f>::iterator v_iter = mVertexValence.begin() ; v_iter!=mVertexValence.end() ; ++v_iter)
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
    for(unsigned i=0; i<mVertexValence.size(); ++i)
    {
        mVertexValence[i] = (mVertexValence[i] - center) / m;
    }
}


int Analysis::getNbFaces()
{
    return nbFaces;
}

int Analysis::getNbVertices()
{
    return nbVertices;
}

int Analysis::getNbHoles() const
{
    return nbHoles;
}

int Analysis::getNbConnectedComponent() const
{
    return nbConnectedComponents;
}
float Analysis::getMeanValence() const
{
    return meanValence;
}

void Analysis::convertRGBToHSV(const float RGB[], float HSV[])
{

    // Copy
    float _RGB[3] = { RGB[0], RGB[1], RGB[2] };

    _RGB[0] /= 255;
    _RGB[1] /= 255;
    _RGB[2] /= 255;
    float minV=_RGB[0];
    float maxV=_RGB[0];
    if (_RGB[0] < _RGB[1])
        if (_RGB[1] < _RGB[2])
            maxV = _RGB[2];
        else if (_RGB[2]<_RGB[0]){
            minV = _RGB[2];
            maxV = _RGB[1];
        }
        else
            maxV = _RGB[1];

    else if (_RGB[0] < _RGB[2]){
        minV = _RGB[1];
        maxV = _RGB[2];
    }
    else
        maxV = _RGB[0];
        if (_RGB[1] < _RGB[2])
            minV = _RGB[1];
        else
            minV = _RGB[2];

   float delta = maxV - minV;
   float deltaR;
   float deltaG;
   float deltaB;
   if (delta == 0){
       HSV[0] = 0;
       HSV[1] = 0;
   }
   else{
       HSV[1] = delta/maxV;
       deltaR = ( ( (delta - _RGB[0])/ 6) + (delta / 2) ) / delta;
       deltaG = ( ( (delta - _RGB[1])/ 6) + (delta / 2) ) / delta;
       deltaB = ( ( (delta - _RGB[2])/ 6) + (delta / 2) ) / delta;
       if ( _RGB[0] == delta) HSV[0] = deltaB - deltaG;
       else if (_RGB[1] == delta) HSV[0] = (1/3) + deltaR - deltaB;
       else if (_RGB[2] == delta) HSV[0] = (2/3) + deltaG - deltaR;

       if (HSV[0] < 0) HSV[0]++;
       if (HSV[0] > 1) HSV[0]--;
   }



}

void Analysis::convertHSVToRGB(const float HSV[], float RGB[])
{
    // Copy
    float _HSV[3] = { HSV[0], HSV[1], HSV[2] };

    if (_HSV[1] == 0){
        RGB[0] = _HSV[2] * 255;
        RGB[1] = _HSV[2] * 255;
        RGB[2] = _HSV[2] * 255;
    }
    else{
        float tmpH = _HSV[0] * 6;
        if (tmpH == 6) tmpH = 0;
        int i = (int) tmpH;
        float tmp1 = _HSV[2] * ( 1 - _HSV[1]);
        float tmp2 = _HSV[2] * ( 1 - _HSV[1] * (tmpH - i) );
        float tmp3 = _HSV[2] * ( 1 - _HSV[1] * (1 - (tmpH - i)) );

        float tmpR;
        float tmpG;
        float tmpB;
        if      (i == 0) {tmpR = _HSV[2]; tmpG = tmp3   ; tmpB = tmp1;}
        else if (i == 1) {tmpR = tmp2   ; tmpG = _HSV[2]; tmpB = tmp1;}
        else if (i == 2) {tmpR = tmp1   ; tmpG = _HSV[2]; tmpB = tmp3;}
        else if (i == 3) {tmpR = tmp1   ; tmpG = tmp2   ; tmpB = _HSV[2];}
        else if (i == 4) {tmpR = tmp3   ; tmpG = tmp1   ; tmpB = _HSV[2];}
        else             {tmpR = _HSV[2]; tmpG = tmp1   ; tmpB = tmp2;}


        RGB[0] = tmpR * 255;
        RGB[1] = tmpG * 255;
        RGB[2] = tmpB * 255;
    }
}
