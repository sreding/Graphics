#ifndef OBJMODEL_H
#define OBJMODEL_H

#include <QtOpenGL>
#include "Point3.h"
#include "Point2.h"

class ObjModel
{
public:
    ObjModel();
    void init();
    void draw();
    std::vector<GLfloat> fvertices;
    std::vector<GLfloat> fuvs;
    std::vector<GLfloat> fnormals;

private:
     // Won't be used at the moment

    GLuint vertexBuffer;
    GLuint uvBuffer;
    GLuint normalsBuffer;
};

#endif // SPHERE_H
