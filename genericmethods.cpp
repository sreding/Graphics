#include "CCanvas.h"
#include "genericmethods.h"

void rotatePointX(Point3d *p, float alpha){
  float y = cos(alpha)*p->y() - sin(alpha)*p->z();
  float z = sin(alpha)*p->y()+ cos(alpha)*p->z();
  p->y() = y;
  p->z() = z;
}

void rotatePointY(Point3d *p, float alpha){
  float x = cos(alpha)*p->x() + sin(alpha)*p->z();
  float z = -sin(alpha)*p->x()+ cos(alpha)*p->z();
  p->x() = x;
  p->z() = z;
}

void rotatePointZ(Point3d *p, float alpha){
  float x = cos(alpha)*p->x() - sin(alpha)*p->y();
  float y = sin(alpha)*p->x()+ cos(alpha)*p->y();
  p->x() = x;
  p->y() = y;
}

void printmodelview(){
  GLfloat matrix[16];
  glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
  for(int i=0; i < 16; i++){
    cout<< matrix[i] << " ";
  }
  cout << "\n----------------" << endl;
}

void rotatepropeller(float acc){
  static float propeller = 0;
  if(propeller>360){
    propeller+=255;
  }else{
    propeller+=acc;
    propeller *= 1.2;
    propeller = fmod(propeller,10000000);
  }
  glTranslated(0.0, 0.0, -3.4); // move in proper position
  glRotated(propeller,0.0,0.0,1.0);
}

void showaxis(){
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(-6.0f, 0.0f, 0.0f);
        glVertex3f(6.0f, 0.0f, 0.0f);
    glEnd();
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, -6.0f, 0.0f);
        glVertex3f(0.0f, 6.0f, 0.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, -6.0f);
        glVertex3f(0.0f, 0.0f, 6.0f);
    glEnd();
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
