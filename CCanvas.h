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
    const char *pathToFile[2];

    explicit CCanvas(QWidget *parent = 0) : QGLWidget(parent),
        texturePlane(readfile()),
        textureTree(readfile()),
        textureScene(readfile())
    {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        timer->start(10);
        pathToFile[0]= readfile();
        pathToFile[1] = readfile();
    }


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
        Main_Body = 0,    // View the scene from a perspective (from above, from a side, or whatever)
        Takeoff,             // View the scene from the train cockpit (if you want, or whatever other view)
        Axis,
        Propeller
    };

    void setView(View _view);

    // Models and textures
    Texture texturePlane,textureTree,textureScene;


};

#endif 
