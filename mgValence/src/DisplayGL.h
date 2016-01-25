#ifndef VISU3D_H
#define VISU3D_H

//#define GLEW_STATIC
//#include <GL/glew.h>
//#include <GL/glu.h>


#include <QGLWidget>
#include <QGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMouseEvent>
#include <QWheelEvent>


static const int TRANS = 10;

class DisplayGL : public QGLWidget
{
    Q_OBJECT
public:
    // Passage des données par pointeur
    Modelisation(QVector<QVector3D> *_listVertices, QWidget *parent = 0);
    ~Modelisation();

    void findCentroid();
protected:


    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void makeLandMark(const QMatrix4x4 &mTransformation);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);



private:

    QVector<QVector3D> *listPoints;
    QVector<QVector3D> *listEdges;
    QVector<QVector3D> *listFaces;





    QVector3D center;
    QMatrix4x4 pMatrix;

    QPoint lastMousePosition;

    double rotY;
    double rotX;
    double transX;
    double transY;
    double transZ;
    double distance;




};

#endif // VISU3D_H
