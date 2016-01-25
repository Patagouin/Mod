#include "DisplayGL.h"


DisplayGL::DisplayGL(QVector<QVector3D> *_listVertices, QWidget *parent)
    : QGLWidget(QGLFormat(), parent), listVertices(_listVertices)
{
    rotY = 25;
    rotX = -25;
    transX = 0;
    transY = 0;
    distance = 100;
    findCentroid();

    updateGL();
}


DisplayGL::~DisplayGL()
{

}

void DisplayGL::findCentroid()
{


    for (int i = 0; i < listVertices->size(); ++i){
        center += listVertices->at(i);
    }
    center /= listVertices->size();


}




void DisplayGL::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPointSize(2);


    qglClearColor(QColor(Qt::black));

}

void DisplayGL::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);
    glViewport(0, 0, width, height);
}

void DisplayGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 vMatrix;

    QMatrix4x4 cameraTransformation;

    cameraTransformation.rotate(rotY, 0, 1, 0);
    cameraTransformation.rotate(rotX, 1, 0, 0);

    QVector3D cameraPosition = cameraTransformation * QVector3D(0,0,distance);

    QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

    cameraTransformation.translate(transX,transY,transZ);
    QVector3D translation = QVector3D(cameraTransformation.column(3));

    vMatrix.lookAt(cameraPosition + center + translation, center+ translation, cameraUpDirection);

    QMatrix4x4 mvpMatrix;
    mvpMatrix = pMatrix * vMatrix;


    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mvpMatrix.constData());

    qglColor(Qt::green);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, listVertices->constData());
    glDrawArrays(GL_POINTS, 0, listVertices->size());

    glDisableClientState(GL_VERTEX_ARRAY);



    glClear(GL_DEPTH_BUFFER_BIT); // On annule le test de profondeur afin que le repere
    makeLandMark(mvpMatrix);      //soit devant en meme temps que la scene


}
void DisplayGL::makeLandMark(const QMatrix4x4 &mTransformation){

    // repere
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mTransformation.constData());
    glTranslated(center.x(), center.y(), center.z());
    glLineWidth(2.5);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1, 0, 0);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0, 1, 0);
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0, 0, 1);
    glEnd();
}


/// Fonction pour la rotation
void DisplayGL::mousePressEvent(QMouseEvent *event)
{
    lastMousePosition = event->pos(); // On sauvegarde les coords pour
                                      // calculer le delta de deplacement
    event->accept(); // L'evenement ne pourra pas declencher d'autre evenement
}

/// Fonction pour la rotation
void DisplayGL::mouseMoveEvent(QMouseEvent *event)
{
    int deltaX = event->x() - lastMousePosition.x();
    int deltaY = event->y() - lastMousePosition.y();

    if (event->buttons() & Qt::LeftButton) {
        rotY -= deltaX;
        while (rotY < 0) { // on garde l'angle rotY entre 0 et 360
            rotY += 360;

        }
        while (rotY >= 360) {
            rotY -= 360;
        }

        rotX -= deltaY;
        if (rotX < -90) { // On bloque la vue afin qu'il n'y est
            rotX = -90;   // pas d'inversion des commandes (ex: la rotation horizontale serait inverse)
        }                 // et permet aussi de pas inverser le "top" de la camera
        if (rotX > 90) {
            rotX = 90;
        }
    }

    lastMousePosition = event->pos();
    event->accept();
    updateGL();

}
/// Fonction pour le zoom/dezoom
void DisplayGL::wheelEvent(QWheelEvent *event)
{
    int delta = event->delta();

    if (event->orientation() == Qt::Vertical) {
        if (delta < 0) {
            distance *= 1.1;
        } else if (delta > 0) {
            distance *= 0.9;
        }
    }

    event->accept();
    updateGL();

}

void DisplayGL::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        transX -= TRANS;
    }
    else if (event->key() == Qt::Key_Right){
        transX += TRANS;
    }
    else if (event->key() == Qt::Key_Up){
        transY += TRANS;
    }
    else if (event->key() == Qt::Key_Down){
        transY -= TRANS;
    }
    else if (event->key() == Qt::Key_Plus){
        transZ -= TRANS;
    }
    else if (event->key() == Qt::Key_Minus){
        transZ += TRANS;
    }
    updateGL();

}


