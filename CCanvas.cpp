#include "CCanvas.h"
#include "Base.h"
#include "Sphere.h"
#include "genericmethods.h"
#include "ObjectGroup.h"
#include "objloader.hpp"
#include <time.h>
using namespace std;

static ObjectGroup scene("");
static ObjectGroup plane("");
static ObjectGroup Skybox("");
static CCanvas::CamView camView;
static bool cockpit = true;
//-----------------------------------------------------------------------------
Point3d camera_position(0.0f, 0.0f, 0.0f);
Point3d camera_direction(0.0f,0.0f,-1.0f);
Point2d lastPos(0.0f,0.0f);
bool delta_defined = false;
bool updateinaxis = true;
GLfloat cockpit_view[] = {0, 0, 0};
static Point3d cockpit_direction(0,0,-1);
//-----------------------------------------------------------------------------

void CCanvas::initializeGL(){
  initeverything();
  texturePlane.setTexture();
  textureGrass.setTexture();
  textureMountain.setTexture();
  textureWater.setTexture();
  textureLane.setTexture();
  textureTree.setTexture();
  textureSky.setTexture();
  loadOBJ(pathToFile[0],scene);
  loadOBJ(pathToFile[1],plane);
  loadOBJ(pathToFile[2],Skybox);
  camView = Still;
  grabKeyboard();
}
//-----------------------------------------------------------------------------

bool CCanvas::event(QEvent *event){
  float moveSpeed = 0.2;
  switch (event->type()) {
    case QEvent::KeyPress : {
      QKeyEvent *ke = static_cast<QKeyEvent *>(event);
      if (ke->key() == Qt::Key_Space) {
        // camView = CCanvas::Rotate;
        cockpit = !cockpit;
        // CCanvas::camView = Rotate;
        // rotateScene = !rotateScene;
        return true;
      }
      if(ke->key() == Qt::Key_R){
        if(camView == CCanvas::Still)
          camView = CCanvas::Rotate;
        else
          camView = CCanvas::Still;
      }
      if(cockpit == false){
        if(ke->key() == Qt::Key_Right || ke->key() == Qt::Key_D ){
          camera_position[0]+=0.2;
        }else if(ke->key() == Qt::Key_Left || ke->key() == Qt::Key_A ){
          camera_position[0]-=0.2;
        }else if(ke->key() == Qt::Key_Q ){
          camera_position[1]+=0.2;
        }else if(ke->key() == Qt::Key_Z ){
          if(camera_position[1] > 0)
            camera_position[1]-=0.2;
        }else if(ke->key() == Qt::Key_Up || ke->key() == Qt::Key_W ){
          camera_position += camera_direction.normalized()*moveSpeed;
        }else if(ke->key() == Qt::Key_Down || ke->key() == Qt::Key_S ){
          camera_position -= camera_direction.normalized()*moveSpeed;
        }
      }
      break;
    }

    case QEvent::MouseMove : {
      QMouseEvent *me = static_cast<QMouseEvent *>(event);
      if(!delta_defined){
        delta_defined = true;
        lastPos.x() = me->x();
        lastPos.y() = me->y();
      }else if(cockpit){
        float speed = 0.1;
        float dx = me->x() - lastPos.x();
        rotatePointY(&cockpit_direction,-dx*speed);
        float dy = me->y() - lastPos.y();
        rotatePointX(&cockpit_direction,-dy*speed);
        lastPos.x()=me->x();
        lastPos.y()=me->y();
      }else{
          float speed = 0.1;
          float dx = me->x() - lastPos.x();
          rotatePointY(&camera_direction,-dx*speed);
          float dy = me->y() - lastPos.y();
          rotatePointX(&camera_direction,-dy*speed);
          lastPos.x()=me->x();
          lastPos.y()=me->y();
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
  static float distance_camera = -15;
  static float axis_rotate = 0.0;

  if(camView == CCanvas::Rotate){
    axis_rotate+=0.2;
  }

  GLfloat lightpos[] = {0.0, 4.0, distance_camera, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

  glTranslatef(0.0, -2.0, distance_camera);
  float scaleFactor = 2.0f;
  glScalef(scaleFactor,scaleFactor,scaleFactor);
  glRotatef(90 + axis_rotate, 0.0f, 1.0f, 0.0f);
}

void CCanvas::setView(View _view) {
  static bool tookoff =false;
  static float yy;
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
  static time_t begin_t = 0;
  if(begin_t == 0){
    begin_t = time(NULL);
  }

  static Path path[8] = {Start,Straight,Circle,Land,Right, Left, Up, Down};
  switch(_view) {
    case Axis:
      setmainaxis(x, y);
      break;
   case Main_Body_inv: {
    switch(path[plane_state]){
      case Start: {
        // updateinaxis = false;
        glPushMatrix();
        glTranslatef(x+acc, yy, z);
        glRotatef(270, 0.0,1.0,0.0);
        glRotatef(15, 1.0,0.0,0.0);
        float model[16];
        glGetFloatv (GL_MODELVIEW, model);
        updatemodelview(model);
        glPopMatrix();

        static float rotate_show =0.0;
        // rotate_show+= 0.2;
        glTranslatef(x,y,z);
        glRotatef(270, 0.0,1.0,0.0);
        // cout << rotate_show << endl;
        // cout << cockpit_direction.x() << " " << cockpit_direction.y() << " "  << cockpit_direction.z() << endl;
        glRotatef( 15, 1.0,0.0,0.0);
        // rotatePointY(&cockpit_direction, rotate_show);
        break;
      }
    case Straight:{
      glTranslatef(x,y, z); // put in the axis
      glRotated(270, 0.0,1.0,0.0);
      glRotated(180,0.0,1.0,0.0);
      glRotatef(curve*4.5,0.0,0.0,1.0);
      // glRotated(15, 1.0,0.0,0.0);

      break;
    }
    case Land:
      glTranslatef(x,y, z); // put in the axis
      glRotated(270, 0.0,1.0,0.0);
//      glRotated(180,0.0,1.0,0.0);
      glRotatef(curve*4.5,0.0,0.0,1.0);
      glRotated(15, 1.0,0.0,0.0);
      break;
    case Left:
      glTranslatef(x,y, z); // put in the axis
      glRotated(270, 0.0,1.0,0.0);
      glRotated(angle *36,0.0,1.0,0.0);
      glRotatef(curve*4.5,0.0,0.0,1.0);
      // glRotated(15, 1.0,0.0,0.0);
    break;

    case Circle:
      glTranslatef(x,y, z); // put in the axis
      glRotated(270, 0.0,1.0,0.0);
      glRotated(angle *36,0.0,1.0,0.0);
      glRotatef(curve*4.5,0.0,0.0,1.0);
      // glRotated(15, 1.0,0.0,0.0);
      break;
    case Right:
    // cout << "RIGHT" << endl;
      break;
    case Up:
      break;
    case Down:
      break;
  }
    glScaled(0.1,0.1,0.1);
    break;
  }

    case Main_Body: {
      switch(path[plane_state]){
        case Start: {
          if(x <= 1){
            y = 0.2;
          }else{
            if(y < 2){
              y = max(x * x * 0.05, 0.2);
            }else{
              tookoff =true;
              plane_state = 2;
            }
          }
          yy = y;
          if(x+acc <= 1){
            yy = 0.2;
          }else{
            if(yy < 2){
              yy = max((x+acc) * (x+acc) * 0.05, 0.2);
            }
          }
          x+=acc;
          acc*=1.0075;
          // updateinaxis = false;
          glPushMatrix();
          glTranslatef(x+acc, yy, z);
          glRotatef(270, 0.0,1.0,0.0);
          glRotatef(15, 1.0,0.0,0.0);
          float model[16];
          glGetFloatv (GL_MODELVIEW, model);
          updatemodelview(model);
          glPopMatrix();

          static float rotate_show =0.0;
          // rotate_show+= 0.2;
          glTranslatef(x,y,z);
          glRotatef(270, 0.0,1.0,0.0);
          // cout << rotate_show << endl;
          // cout << cockpit_direction.x() << " " << cockpit_direction.y() << " "  << cockpit_direction.z() << endl;
          glRotatef( 15, 1.0,0.0,0.0);
          // rotatePointY(&cockpit_direction, rotate_show);

          break;
        }
        case Straight:{
          if(x < -5.5){
            plane_state = 5;
          }
          x -= 0.02;

          if(curve > 0.0){
              curve -= 0.2;

          }
          glTranslatef(x,y, z); // put in the axis
          glRotated(270, 0.0,1.0,0.0);
          glRotated(180,0.0,1.0,0.0);
          glRotatef(curve*4.5,0.0,0.0,1.0);
          // glRotated(15, 1.0,0.0,0.0);

          break;
        }
        case Land:
          if(tookoff){
              tookoff = false;
          }
//          y -= 0.01;
          x += 0.01;
          acc *=0.9025;
          if(y <= 0.2){
            acc = 0.01;
            y = 0.2;
            tookoff =false;

            x = -4.5;
           y = 0.1;
            z = 0.0;
            acc = 0.01;
            radius = 3.0;
            centerx = 0.0;
            centerz = 0.0;
            curve = 0.2;
            angle = 0.0;
           reached_max = false;
            check = true;
        plane_state = 0;
            time_t begin_t = 0;
            if(begin_t == 0){
              begin_t = time(NULL);
            }

            plane_state=0;
          }else{
              y -= 0.01;

          }
          glTranslatef(x,y, z); // put in the axis
          glRotated(270, 0.0,1.0,0.0);
//          glRotated(180,0.0,1.0,0.0);
          glRotatef(curve*4.5,0.0,0.0,1.0);
          // glRotated(15, 1.0,0.0,0.0);
          break;
        case Left:
          if(check){
            centerz = z+9.0;
            centerx = x;
            acc = 1.5;
            curve = 0.0;
            check = false;
            reached_max = false;
            angle = 5.0;
          }

          x = (centerx + (radius * cos(acc)));
          z = -(centerz + (radius * sin(acc)));

          if(reached_max && curve > 0.0){
            curve -= 0.05;
            angle += 0.016;
          }else if(angle > 7.5){
            reached_max = true;
          }else{
            curve += 0.05;
            angle += 0.016;
          }
          if(curve < 0.0){
            check = false;
            plane_state = 3;
          }
          acc += 0.01;
          glTranslatef(x,y, z); // put in the axis
          glRotated(270, 0.0,1.0,0.0);
          glRotated(angle *36,0.0,1.0,0.0);
          glRotatef(curve*4.5,0.0,0.0,1.0);
          // glRotated(15, 1.0,0.0,0.0);
        break;

        case Circle:
          // cout << "CIRCLE" << '\n';
          updateinaxis = false;
          if(check){
            curve = 0.2;
            angle = 0.0;
            reached_max = false;
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
          // glRotated(15, 1.0,0.0,0.0);

          // rotatePointY(&cockpit_direction, angle *36);
          // rotatePointZ(&cockpit_direction, curve*4.5);
          // rotatePointY(&cockpit_direction, 270 + angle *36);
          if((time(NULL) - begin_t > 15) && angle < 15.1 && angle > 14.9){
//                cout << "Angle: "<< angle << endl;
                check = true;
                plane_state = 1;
          }

          break;
        case Right:
        // cout << "RIGHT" << endl;
          updateinaxis = false;
          x--;
          break;
        case Up:
        // cout << "UP" << endl;
          updateinaxis = false;
          y++;
          break;
        case Down:
        // cout << "DOWN" << endl;
          updateinaxis = false;
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
      if(!tookoff && l_wheel < 3.4){
        l_wheel += 0.05;
      }
      glTranslated(-2.2,-0.3,-1.6); // l wheel
      glRotated(-5, 0.0,1.0,0.0);
      glRotated(cos(l_wheel) * 50 + 33, 0.0,0.0,1.0);
      break;

    case R_Wheel:
      static float r_wheel = 3.4;
      if(r_wheel > 0.0 && tookoff)
        r_wheel -= 0.05;
      if(!tookoff && r_wheel < 3.4){
        r_wheel += 0.05;
      }
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
    cockpit_view[0] = mod[12];
    cockpit_view[1] = mod[13] + 0.3;
    cockpit_view[2] = mod[14];
}

void CCanvas::paintGL()
{
  initpaintgl();
  if(!cockpit){
  glPushMatrix();
  glLoadIdentity();
  textureSky.bind();
  free_camera_lookat();
  glScalef(100,100,100);
  Skybox.objects[0].draw();
  textureSky.unbind();
  glPopMatrix();
  }
  glPushMatrix(); // IDENTITY IDENTITY
  glLoadIdentity();
  if(!cockpit)
    free_camera_lookat();
//  if(cockpit){
//      cockpit_direction.normalize();
//      lookAt(cockpit_view[0], cockpit_view[1], cockpit_view[2],
//        cockpit_view[0] + cockpit_direction.x(), cockpit_view[1] + cockpit_direction.y(), cockpit_view[2] + cockpit_direction.z(),
//        0,1,0);
//        // cout << cockpit_view[0] + cockpit_direction.x() << " " << cockpit_view[1] + cockpit_direction.y() << " "
//        // << cockpit_view[2] + cockpit_direction.z() << endl;

//        // cout << cockpit_view[0] << " " << cockpit_view[1] << " "
//        // << cockpit_view[2] << endl;

////        cockpit_direction.x() = 0;
////        cockpit_direction.y() = 0;
////        cockpit_direction.z() = -1;
//  }

  //    ##################AXES##################
  setView(View::Axis);
//      showaxis();
  glPushMatrix(); // IDENTITY AXIS AXIS
  glEnable(GL_LIGHTING);

  // ##################AIRPLANE##################
  setplanematerial();


  setView(View::Main_Body);
  float model[16];
  glGetFloatv (GL_MODELVIEW, model);
  updatemodelview(model);


  if(cockpit){
      glLoadIdentity();
      textureSky.bind();
      lookAt(cockpit_view[0], cockpit_view[1], cockpit_view[2],
        cockpit_view[0] + cockpit_direction.x(), cockpit_view[1] + cockpit_direction.y(), cockpit_view[2] + cockpit_direction.z(),
        0,1,0);
      glScalef(100,100,100);
      Skybox.objects[0].draw();
      textureSky.unbind();
      glPopMatrix();
      cockpit_direction.normalize();
      glLoadIdentity();
    lookAt(cockpit_view[0], cockpit_view[1], cockpit_view[2],
      cockpit_view[0] + cockpit_direction.x(), cockpit_view[1] + cockpit_direction.y(), cockpit_view[2] + cockpit_direction.z(),
      0,1,0);
    setView(View::Axis);
    glPushMatrix();
    setView(View::Main_Body_inv);
  }
  texturePlane.bind();
  plane.objects[0].draw();
  glPushMatrix(); // IDENTITY AXIS MAIN_BODY MAIN_BODY
//  if(!updateinaxis){
//    float model[16];
//    glGetFloatv (GL_MODELVIEW, model);
//    updatemodelview(model);
//    // printmodelview();
//  }

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
