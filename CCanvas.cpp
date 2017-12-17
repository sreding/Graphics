#include "CCanvas.h"
#include "Base.h"
#include "Sphere.h"
#include "genericmethods.h"
#include "ObjectGroup.h"
#include "objloader.hpp"
using namespace std;

static ObjectGroup scene("");
static ObjectGroup plane("");
static ObjectGroup Skybox("");
static CCanvas::CamView camView;
static bool cockpit = false;
//-----------------------------------------------------------------------------
Point3d camera_position(0.0f, 0.0f, 0.0f);
Point3d camera_direction(0.0f,0.0f,-1.0f);
Point2d lastPos(0.0f,0.0f);
bool delta_defined = false;
GLfloat cockpit_view[] = {0, 0, 0};
Point3d cockpit_direction(0,0,-1);
//-----------------------------------------------------------------------------

void CCanvas::initializeGL(){
  initeverything();
  texturePlane.setTexture();
  textureGrass.setTexture();
  textureMountain.setTexture();
  textureWater.setTexture();
  textureLane.setTexture();
  textureTree.setTexture();
  loadOBJ(pathToFile[0],scene);
  loadOBJ(pathToFile[1],plane);
  camView = Still;
  grabKeyboard();
}
//-----------------------------------------------------------------------------

bool CCanvas::event(QEvent *event){
  float moveSpeed = 0.2;
  switch (event->type()) {
    case QEvent::KeyPress : {
      QKeyEvent *ke = static_cast<QKeyEvent *>(event);
      if (ke->key() == Qt::Key_R) {
        // camView = CCanvas::Rotate;
        cockpit = !cockpit;
        // CCanvas::camView = Rotate;
        // rotateScene = !rotateScene;
        return true;
      }else if(ke->key() == Qt::Key_Right || ke->key() == Qt::Key_D ){
        camera_position[0]-=0.2;
      }else if(ke->key() == Qt::Key_Left || ke->key() == Qt::Key_A ){
        camera_position[0]+=0.2;
      }else if(ke->key() == Qt::Key_Up || ke->key() == Qt::Key_W ){
        camera_position += camera_direction.normalized()*moveSpeed;
      }else if(ke->key() == Qt::Key_Down || ke->key() == Qt::Key_S ){
        camera_position -= camera_direction.normalized()*moveSpeed;
      }
      break;
    }

    case QEvent::MouseMove : {
      QMouseEvent *me = static_cast<QMouseEvent *>(event);
      if(!delta_defined){
        delta_defined = true;
        lastPos.x() = me->x();
        lastPos.y() = me->y();
      }else{
        float speed = 0.0001;
        float dx = me->x() - lastPos.x();
        //            camera_direction.x() += dx*speed;
        rotatePointY(&camera_direction,-dx*speed);
        float dy = me->y() - lastPos.y();
        rotatePointX(&camera_direction,-dy*speed);
        //            camera_direction.y() -= dy*speed;
      }
      break;
    }

    case QEvent::MouseButtonRelease :{
      delta_defined = false;
      break;
    }
  }
  return QWidget::event(event);
}

void setmainaxis(float x, float y){
  static float distance_camera = -25;
  static float axis_rotate = 0.0;

  if(camView == CCanvas::Rotate){
    axis_rotate+=0.2;
  }

  GLfloat lightpos[] = {0.0, 4.0, distance_camera, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

  glTranslatef(0.0, -4.0, distance_camera);
  float scaleFactor = 2.0f;
  glScalef(scaleFactor,scaleFactor,scaleFactor);
  glRotatef(90 + axis_rotate, 0.0f, 1.0f, 0.0f);
}

void CCanvas::setView(View _view) {
  static bool tookoff =false;
  static float x = -4.5;
  static float y = 0.1;
  static float z = 0.0;
  static float acc = 0.01;
  static float radius = 3.0;
  static float centerx = 0.0;
  static float centerz = 0.0;
  static float curve = 0.2;
  static float angle = 0.0;
  static bool reached_max = false;
  static bool check = true;
  static int plane_state = 0;
  static Path path[7] = {Start, Land,Circle,Right, Left, Up, Down};
  switch(_view) {
    case Axis:
      setmainaxis(x, y);
      break;

    case Main_Body: {
      switch(path[plane_state]){
        case Start: {
          x+=acc;
          acc *=1.0075;
          if(x <= 1){
            y = 0.2;
          }else{
            if(y < 2){
              y =   max(x * x * 0.05, 0.2);

            }else{
              tookoff =true;
              plane_state = 2;
            }
          }

          glTranslatef(x,y,z);
          static float rotate_show = 0.0;
          // rotate_show += 0.2;
          glRotatef(270, 0.0,1.0,0.0);
          glRotatef(15 + rotate_show, 1.0,0.0,0.0);
          break;
        }
        case Land:{
          x+=acc;
          acc *=0.9;
          if(x <= 1){
            y = 10;
          }else{
            if(y < 6)
            y =  x * x * -0.05;
          }
          break;
        }
        case Left:

        x = x-0.2;
        z = z-0.2;
        if(reached_max && curve > 0.0){
          curve -=0.2;
          angle += 0.1;
        }else if(curve > 10){
          reached_max = true;
        }else if(curve < 0.0){
          reached_max = false;
          curve = 0.2;
          //set next path
          plane_state = 0;
        }else{
          curve += 0.2;
          angle += 0.1;
        }
        glTranslatef(x,y, z); // put in the axis
        glRotated(angle *9,-1.0,1.0,1.0);
        glRotated(270, 0.0,1.0,0.0);
        glRotated(15, 1.0,0.0,0.0);
        // rotatePointX(&cockpit_direction, 15);
        // rotatePointY(&cockpit_direction, 270);
        // rotatePointZ(&cockpit_direction,angle *9);
        // rotatePointY(&cockpit_direction,angle *9);
        // rotatePointX(&cockpit_direction,angle *9);
        break;
        case Circle:
          if(check){
            centerz = -z-radius;
            centerx = -x;
            acc = 1.5;
            check = false;
          }
          x = -(centerx + (radius * cos(acc)));
          z = (centerz + (radius * sin(acc)));
          if(reached_max && curve > 0.0){
            // curve -=0.2;
            angle += 0.016;
          }else if(curve > 10){
            reached_max = true;
          }else if(curve < 0.0){
            reached_max = false;
            curve = 0.2;
            //set next path
            plane_state = 0;
          }else{
            curve += 0.05;
            angle += 0.016;
          }
          acc += 0.01;
          glTranslatef(x,y, z); // put in the axis
          glRotated(270, 0.0,1.0,0.0);
          glRotated(angle *36,0.0,1.0,0.0);
          glRotatef(curve*4.5,0.0,0.0,1.0);
          glRotated(15, 1.0,0.0,0.0);
          // rotatePointX(&cockpit_direction, 15);
          // rotatePointZ(&cockpit_direction, curve*4.5);
          // rotatePointY(&cockpit_direction, 270 + angle *36);

          break;
        case Right:
          x--;
          break;
        case Up:
          y++;
          break;
        case Down:
          y--;
          break;
      }
      glScaled(0.1,0.1,0.1);
      break;
    }

    case Propeller:
      rotatepropeller(acc);
      break;

    case L_Wheel:
      static float l_wheel = 3.4;
      if(l_wheel > 0.0 && tookoff)
        l_wheel -= 0.05;
      glTranslated(-2.2,-0.3,-1.6); // l wheel
      glRotated(-5, 0.0,1.0,0.0);
      glRotated(cos(l_wheel) * 50 + 33, 0.0,0.0,1.0);
      break;

    case R_Wheel:
      static float r_wheel = 3.4;
      if(r_wheel > 0.0 && tookoff)
        r_wheel -= 0.05;
      glTranslated(2.2,-0.3,-1.6); // r wheel
      glRotated(5, 0.0,1.0,0.0);
      glRotated(-cos(r_wheel) * 50 - 33, 0.0,0.0,1.0);
      break;

    case L_Flap:
      static float l_flap = 0.0;
      l_flap += 0.05;
      glTranslated(-4.0,-0.1,-0.6); // l flap
      glRotated(-7.0,0.0,1.0,0.0);
      glRotated(-8,0.0,0.0,1.0);
      //            glRotated(cos(l_flap) * 60 - 30, 1.0,0.0,0.0);
      break;

    case R_Flap:
      static float r_flap = 0.0;
      r_flap += 0.05;
      glTranslated(4.0,-0.1,-0.6); // r flap
      glRotated(7.0,0.0,1.0,0.0);
      glRotated(8,0.0,0.0,1.0);
      //            glRotated(cos(r_flap) * 60 - 30, 1.0,0.0,0.0);
      break;

    case Back:
      static float back = 0.0;
      back += 0.05;
      glTranslated(-0.0,0.25,4.55); // back
      //            glRotated(-cos(back) * 60 , 0.0,1.0,0.0);
      glRotated(3, 1.0,0.0,0.0);
      break;

    case R_B_Flap:
      static float r_b_flap = 0.0;
      r_b_flap += 0.05;
      glTranslated(0.95,0.0,4.0); // r back flap
      //            glRotated(cos(r_b_flap) * 60 - 30, 1.0,0.0,0.0);
      break;

    case L_B_Flap:
      static float l_b_flap = 0.0;
      l_b_flap += 0.05;
      glTranslated(-0.95,0.0,4.0); // l back flap
      //            glRotated(cos(l_b_flap) * 60 - 30, 1.0,0.0,0.0);
      break;
  }
}

void CCanvas::free_camera_lookat(){
  lookAt(camera_position[0],camera_position[1],camera_position[2],
    camera_position[0]+camera_direction[0],
    camera_position[1]+camera_direction[1],
    camera_position[2]+camera_direction[2],  0,1,0); // camera position , "look at" point , view-up vector
  }

void updatemodelview(float mod[]){
  // Point3d a(0,0,-1);
  // rotatePointX(&a, 90 * PI / 180);
  // cout << a.x() << " " << a.y() << " "  << a.z() << endl;
    cockpit_view[0] += mod[12];
    cockpit_view[1] += mod[13] + 0.3;
    cockpit_view[2] += mod[14];
}

void CCanvas::paintGL()
{
  initpaintgl();
  glPushMatrix(); // IDENTITY IDENTITY
  glLoadIdentity();
//      free_camera_lookat();
  if(cockpit){
      lookAt(cockpit_view[0], cockpit_view[1], cockpit_view[2],
        cockpit_view[0] + cockpit_direction.x(), cockpit_view[1] + cockpit_direction.y(), cockpit_view[2] + cockpit_direction.z(),
        0,1,0);
        // cout << cockpit_direction.x() << " " << cockpit_direction.y() << " "  << cockpit_direction.z() << endl;

        cockpit_direction.x() = 0;
        cockpit_direction.y() = 0;
        cockpit_direction.z() = -1;
  }

  //    ##################AXES##################
  setView(View::Axis);
//      showaxis();
  glPushMatrix(); // IDENTITY AXIS AXIS
  glEnable(GL_LIGHTING);

  // ##################AIRPLANE##################
  setplanematerial();

  texturePlane.bind();
  setView(View::Main_Body);
  plane.objects[0].draw();
  glPushMatrix(); // IDENTITY AXIS MAIN_BODY MAIN_BODY

  // glTranslatef(xx,0,0);
  float model[16];
  glGetFloatv (GL_MODELVIEW, model);
  updatemodelview(model);
  // popandpush();

  static View views[8] = {View::L_Wheel, R_Wheel, R_Flap, L_Flap, Back, L_B_Flap, R_B_Flap, Propeller};
  for(int i = 0; i < 8; i++){
    setView(views[i]);
    plane.objects[i+1].draw();
    popandpush();
  }
  texturePlane.unbind();

  glPopMatrix(); // IDENTITY AXIS MAIN_BODY
  popandpush(); // IDENTITY AXIS AXIS

  static float scaleplane = 2.0;
  textureLane.bind(); // LANE
  glTranslated(0.0,0.01,0.0);
  glScaled(scaleplane,scaleplane,scaleplane);
  scene.objects[0].draw();
  popandpush();
  textureLane.unbind();

  textureWater.bind(); // WATER
  glScaled(scaleplane,scaleplane,scaleplane);
  scene.objects[2].draw();
  popandpush();
  textureWater.unbind();


  textureTree.bind(); // TREE
  glScaled(scaleplane,scaleplane,scaleplane);
  scene.objects[1].draw();
  popandpush();
  textureTree.unbind();
  for(int i =5; i < 12; i++){
    textureTree.bind(); // TREE
    glScaled(scaleplane,scaleplane,scaleplane);
    scene.objects[i].draw();
    popandpush();
    textureTree.unbind();
  }

  textureGrass.bind(); // GRASS
  glScaled(scaleplane,scaleplane,scaleplane);
  scene.objects[3].draw();
  popandpush();
  textureGrass.unbind();

  textureMountain.bind();  // MOUNTAIN
  glScaled(scaleplane,scaleplane,scaleplane);
  scene.objects[4].draw();
  popandpush();
  textureMountain.unbind();

  glPopMatrix(); // IDENTITY AXIS
  glPopMatrix(); // IDENTITY

}
