#ifndef GENERICMETHODS_H
#define GENERICMETHODS_H

void showaxis();
void popandpush();
void rotatepropeller(float acc);
void setmainaxis(float x, float y);
void updatemodelview(float mod[]);
void printmodelview();
void rotatePointX(Point3d *p, float alpha);
void rotatePointY(Point3d *p, float alpha);
void rotatePointZ(Point3d *p, float alpha);
void initpaintgl();
void setplanematerial();
void initeverything();
#endif // GENERICMETHODS_H
