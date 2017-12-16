/***********************************************************************aaaaaa*/
/* Guards                                                               */
/************************************************************************/
#ifndef CCANVAS_H
#define CCANVAS_H

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <QtOpenGL>
#include <QGLWidget>
#include <QTimer>
#include <QDebug>


#include "Base.h"
#include "texture.hpp"

#include "ObjModel.h"
#include "PlyModel.h"

using namespace std;

/************************************************************************/
/* Canvas to draw                                                       */
/************************************************************************/
class CCanvas : public QGLWidget
{
  Q_OBJECT

public:
    const char *pathToFile[3];

    explicit CCanvas(QWidget *parent = 0) : QGLWidget(parent),
        texturePlane(readfile()),
        textureGrass(readfile()),
        textureMountain(readfile()),
        textureWater(readfile()),
        textureLane(readfile()),
        textureTree(readfile())
    {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        timer->start(10);
        pathToFile[0]= readfile(); // scene
        pathToFile[1] = readfile(); // plane
        pathToFile[2] = readfile();
    }
    bool event(QEvent *event);
    enum CamView {
        Still = 0,
        Rotate,
        Plane
    };


protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();


private:

    void lookAt(const GLdouble eyex,
                const GLdouble eyey,
                const GLdouble eyez,
                const GLdouble centerx,
                const GLdouble centery,
                const GLdouble centerz,
                const GLdouble upx,
                const GLdouble upy,
                const GLdouble upz);
    void free_camera_lookat();

    void glPerspective(const GLdouble fovy,
                       const GLdouble aspect,
                       const GLdouble zNear,
                       const GLdouble zFar);

    inline const char * readfile(){
       static ifstream file("../../../path.txt");
       if(file){
           char * data = (char *) malloc(100);
           memset(data,'\0',100);
           file >> data;
           return data;
       }else{
           cout << "WRONG PATH GIVEN" << endl;
       }
        return NULL;
    }


    enum View {
        Main_Body = 0,
        Axis,
        Propeller,
        L_B_Flap,
        R_B_Flap,
        L_Flap,
        R_Flap,
        Back,
        L_Wheel,
        R_Wheel
    };

    enum Path {
        Start = 0,
        Land,
        Left,
        Right,
        Up,
        Down,
        Circle
    };
//    CamView camView;

    void setView(View _view);

    // Models and textures
    Texture texturePlane,textureGrass,textureMountain,textureWater,textureLane, textureTree;


};

#endif 
