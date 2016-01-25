#include "Mesh.h"
#include "Pointcloud.h"
#include "Shader.h"

class Analysis : public PointCloud
{
public:

    Analysis(const std::string& filename, bool drawModel = true);

    void draw(Shader *shader, bool drawEdges = false);

    void init(Shader *shader);
    void specifyVertexData(Shader *shader);


    void makeUnitary();

    int getNbFaces();
    int getNbVertices();

    int getNbHoles() const;
    int getNbConnectedComponent() const;

    float getMeanValence() const;

private:
    Mesh mesh;

    Shader *mAnalyseShader;
    int nbVertices;
    int nbFaces;

    int nbHoles;
    int nbConnectedComponents;

    float meanValence;

    bool drawModel;


    void convertRGBToHSV(const float RGB[3], float HSV[3]);
    void convertHSVToRGB(const float HSV[3], float RGB[3]);

    //surface_mesh::Surface_mesh mHalfEdge;


    std::vector<int> mIndicesValenceVertex;
    std::vector<Eigen::Vector3f> mVertexValence;
    std::vector<Eigen::Vector3f> mNormalsValence;
    std::vector<Eigen::Vector3f> mColorsValence;

    std::vector<Eigen::Vector2i> mIndicesBoundaryEdges;
    std::vector<Eigen::Vector3f> mVertexBoundaryEdges;
    std::vector<Eigen::Vector3f> mNormalsBoundaryEdges;
    std::vector<Eigen::Vector3f> mColorBoundaryEdges;

    std::vector<Eigen::Vector3i> mIndicesFacesHoles;
    std::vector<Eigen::Vector3f> mPositionsHoles;
    std::vector<Eigen::Vector3f> mNormalsHoles;
    std::vector<Eigen::Vector3f> mColorsHoles;

    GLuint mVaoHoles, mVaoBoundaryEdge, mVaoValence;
    GLuint mBufsValence[3];
    GLuint mBufBoundaryEdge[3];
    GLuint mBufsHoles[3];

};
