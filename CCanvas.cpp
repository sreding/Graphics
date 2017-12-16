#include "CCanvas.h"
#include "Base.h"
#include "Sphere.h"

#include "ObjectGroup.h"
#include "objloader.hpp"
using namespace std;
static ObjectGroup scene("");
static ObjectGroup plane("");
static ObjectGroup Skybox("");
static CCanvas::CamView camView;
//-----------------------------------------------------------------------------
Point3d camera_position(0.0f, 0.0f, 0.0f);
Point3d camera_direction(0.0f,0.0f,-1.0f);
void CCanvas::initializeGL()
{
  glClearColor(0.0f, 0.8f, 0.8f, 0.5f);			   // black background
  glClearDepth(1.0f);								   // depth buffer setup
  glEnable(GL_DEPTH_TEST);						   // enables depth testing
  glDepthFunc(GL_LEQUAL);							   // the type of depth testing to do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // really nice perspective calculations
  glShadeModel(GL_SMOOTH);

  // One light source
  glEnable(GL_LIGHTING);

  glEnable(GL_LIGHT0);
  /*
  * The position is transformed by the modelview matrix when glLightfv is called (just as if it were
  * a point), and it is stored in eye coordinates. If the w component of the position is 0.0,
  * the light is treated as a directional source. Diffuse and specular lighting calculations take
  * the light's direction, but not its actual position, into account, and attenuation is disabled.
  * Otherwise, diffuse and specular lighting calculations are based on the actual location of the
  * light in eye coordinates, and attenuation is enabled. The default position is (0,0,1,0); thus,
  * the default light source is directional, parallel to, and in the direction of the -z axis.
  */

  GLfloat ambient[] =  { 0.3, 0.3, 0.3, 0.0 };
  GLfloat diffuse[] =  { 1.0, 1.0, 1.0, 0.0 };
  GLfloat specular[] = { 1.0, 1.0, 1.0, 0.0 };
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
  glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

  loadOBJ(pathToFile[0],scene);
  loadOBJ(pathToFile[1],plane);
//  loadOBJ(pathToFile[2],plane);

  /*
  * Before you can use the texture you need to initialize it by calling the setTexture() method.
  * Before you can use OBJ/PLY model, you need to initialize it by calling init() method.
  */
  texturePlane.setTexture();
  textureGrass.setTexture();
  textureMountain.setTexture();
  textureWater.setTexture();
  textureLane.setTexture();
  textureTree.setTexture();

  //    CCanvas::rotateScene = false;
  grabKeyboard();
  camView = Still;
}

//-----------------------------------------------------------------------------

void CCanvas::glPerspective(const GLdouble fovy, const GLdouble aspect, const GLdouble zNear, const GLdouble zFar)
{
  const GLdouble d = 1.0 / tan(fovy / 360.0 * PI);
  const GLdouble delta = zNear - zFar;

  GLdouble *mat = new GLdouble[16];

  mat[0] = d / aspect;
  mat[1] = 0.0;
  mat[2] = 0.0;
  mat[3] = 0.0;

  mat[4] = 0.0;
  mat[5] = d;
  mat[6] = 0.0;
  mat[7] = 0.0;

  mat[8]  = 0.0;
  mat[9]  = 0.0;
  mat[10] = (zNear + zFar) / delta;
  mat[11] = -1.0;

  mat[12] = 0.0;
  mat[13] = 0.0;
  mat[14] = 2.0 * zNear * zFar / delta;
  mat[15] = 0.0;

  glMultMatrixd(mat);

  delete[] mat;
}

void CCanvas::lookAt(const GLdouble eyeX,
  const GLdouble eyeY,
  const GLdouble eyeZ,
  const GLdouble centerX,
  const GLdouble centerY,
  const GLdouble centerZ,
  const GLdouble upX,
  const GLdouble upY,
  const GLdouble upZ)
  {
    Point3d VP(eyeX, eyeY, eyeZ);
    Point3d q(centerX, centerY, centerZ);
    Point3d VUP(upX, upY, upZ);
    Point3d VPN = VP - q;

    GLdouble *mat = new GLdouble[16]; // remember: column-major order!

    // set up the LookAt matrix correctly!
    Point3d p_prime = VP;
    Point3d z_prime = VPN.normalized();
    Point3d x_prime = (VUP ^ z_prime).normalized();
    Point3d y_prime = z_prime ^ x_prime;

    // column 1
    mat[0] = x_prime[0];
    mat[1] = y_prime[0];
    mat[2] = z_prime[0];
    mat[3] = 0;

    // column 2
    mat[4] = x_prime[1];
    mat[5] = y_prime[1];
    mat[6] = z_prime[1];
    mat[7] = 0;

    // column 3
    mat[8] = x_prime[2];
    mat[9] = y_prime[2];
    mat[10] = z_prime[2];
    mat[11] = 0;

    // sure? column 4
    mat[12] = -x_prime * p_prime;
    mat[13] = -y_prime * p_prime;
    mat[14] = -z_prime * p_prime;
    mat[15] = 1;

    glMultMatrixd(mat);

    delete[] mat;
  }

  void CCanvas::resizeGL(int width, int height)
  {
    // set up the window-to-viewport transformation
    glViewport(0, 0, width, height);

    // vertical camera opening angle
    double beta = 80.0;

    // aspect ratio
    double gamma;
    if(height > 0) gamma = width / (double)height;
    else gamma = width;

    // front and back clipping plane at
    double n = -0.01;
    double f = -1000.0;

    // frustum corners
    // double t = -tan(beta * 3.14159 / 360.0) * n;
    // double b = -t;
    // double r = gamma * t;
    // double l = -r;

    // set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glFrustum(l,r, b,t, -n,-f);

    // alternatively, directly from alpha and gamma
    glPerspective(beta, gamma, -n, -f);
  }

  //-----------------------------------------------------------------------------
  Point2d lastPos(0.0f,0.0f);
  bool delta_defined = false;
  void rotatePointY(Point3d *p, float alpha){
    float x = cos(alpha)*p->x() + sin(alpha)*p->z();
    float z = -sin(alpha)*p->x()+ cos(alpha)*p->z();
    p->x() = x;
    p->z() = z;
  }
  void rotatePointX(Point3d *p, float alpha){
    float y = cos(alpha)*p->y() - sin(alpha)*p->z();
    float z = sin(alpha)*p->y()+ cos(alpha)*p->z();
    p->y() = y;
    p->z() = z;
  }


  bool CCanvas::event(QEvent *event){
    float moveSpeed = 0.2;
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *ke = static_cast<QKeyEvent *>(event);
      if (ke->key() == Qt::Key_R) {
        qDebug() << "R";
        camView = CCanvas::Rotate;
        //            CCanvas::camView = Rotate;
        //            rotateScene = !rotateScene;
        return true;
      }else if(ke->key() == Qt::Key_Right || ke->key() == Qt::Key_D ){
        camera_position[0]-=0.2;
      }else if(ke->key() == Qt::Key_Left || ke->key() == Qt::Key_A ){
        camera_position[0]+=0.2;
      }else if(ke->key() == Qt::Key_Up || ke->key() == Qt::Key_W ){
        camera_position += camera_direction.normalized()*moveSpeed;
        //            camera_position[2]-=0.2;
      }else if(ke->key() == Qt::Key_Down || ke->key() == Qt::Key_S ){
        camera_position -= camera_direction.normalized()*moveSpeed;
      }
    }else if(event->type() == QEvent::MouseMove){
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

    }else if(event->type() == QEvent::MouseButtonRelease){
      delta_defined = false;
    }

    return QWidget::event(event);
  }

  Point3d position(0.0,0.0,0.0);
  Point3d rot_position(0.0,0.0,-1.0);

  void updateposition(float x, float y, float z){
    position[0] += x;
    position[1] += y;
    position[2] += z;
    rot_position[0] += x;
    rot_position[1] += y;
    rot_position[2] += z;
    //    cout << "camera position is translated: " << position[0] << " " << position[1] << " " << position[2] << endl;
  }

  void Translate(float x, float y, float z){
    glTranslatef(x,y,z);
    updateposition(x,y,z);
  }

  void rotatePointZ(Point3d *p, float alpha){
    float x = cos(alpha)*p->x() - sin(alpha)*p->y();
    float z = sin(alpha)*p->x()+ cos(alpha)*p->y();
    p->x() = x;
    p->z() = z;
  }

  void gRotate(float angle, float x, float y, float z){
    if(x == 1){
      glRotated(angle,1.0,0,0);
      rotatePointX(&rot_position,angle);
      cout << "rotate position is rot x: " << rot_position[0] << " " << rot_position[1] << " " << rot_position[2] << endl;
    }

    if(y == 1){
      glRotated(angle,0.0,1.0,0);
      rotatePointY(&rot_position,angle);
      cout << "rotate position is rot y: " << rot_position[0] << " " << rot_position[1] << " " << rot_position[2] << endl;
    }
    if(z == 1){
      glRotated(angle,0.0,0,1.0);
      rotatePointZ(&rot_position,angle);
      cout << "rotate position is rot z: " << rot_position[0] << " " << rot_position[1] << " " << rot_position[2] << endl;
    }
  }


  void setmainaxis(float x, float y){
    static float distance_camera = -25;
    static float axis_rotate = 0.0;

    if(camView == CCanvas::Rotate){
      axis_rotate+=0.2;
    }

    GLfloat lightpos[] = {0.0, 4.0, distance_camera, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    //    Translate(0.0, -4.0, distance_camera);
    glTranslatef(0.0, -4.0, distance_camera);

    float scaleFactor = 2.0f;
    glScaled(scaleFactor,scaleFactor,scaleFactor);

    //    gRotate(90 + axis_rotate, 0.0f, 1.0f, 0.0f);
    glRotatef(90 + axis_rotate, 0.0f, 1.0f, 0.0f);

    rotatePointY(&rot_position, 90);
    updateposition(0.0, -4.0, distance_camera);
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
            //                        x+=acc;
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
            float high_show = 1.5;
            //                        Translate(x,y, z);
            glTranslatef(x,y,z);

            static float rotate_show = 0.0;
            //                        rotate_show += 0.2;
            //                        gRotate(270, 0.0,1.0,0.0);
            //                        gRotate(15 + rotate_show, 1.0,0.0,0.0);
            glRotatef(270, 0.0,1.0,0.0);
            glRotatef(15 + rotate_show, 1.0,0.0,0.0);

            rotatePointX(&rot_position, 15);
            rotatePointY(&rot_position,270);
            updateposition(x,y,z);

            break;
          }
          case Land:{// needs more
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
          //                    glRotatef(curve*9,4.0,0.0,0.0);
          //                    y =   x * x * 0.05;
          //                    Translate(x,y, z); // put in the axis
          //                    gRotate(angle *9,-1.0,1.0,1.0);
          //                    gRotate(270, 0.0,1.0,0.0);
          //                    gRotate(15, 1.0,0.0,0.0);

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
            //                        curve -=0.2;
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

          //                    y =   x * x * 0.05;
          //                    Translate(x,y, z); // put in the axis
          //                    gRotate(270, 0.0,1.0,0.0);
          //                    gRotate(angle *36,0.0,1.0,0.0);
          //                    gRotate(curve*4.5,0.0,0.0,1.0);
          //                    gRotate(15, 1.0,0.0,0.0);

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

  void popandpush(){
    glPopMatrix(); // IDENTITY AXIS MAIN_BODY
    glPushMatrix(); // IDENTITY AXIS MAIN_BODY MAIN_BODY
  }

  void printmodelview(float * position){
    GLfloat matrix[16];
    glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
    for(int i=0; i < 16; i++){
      cout<< matrix[i] << " ";
    }
    cout <<endl;
    //    position[0] = matrix[12];
    //    position[1] = matrix[13];
    //    position[2] = matrix[14];

  }

  void CCanvas::free_camera_lookat(){
    lookAt(camera_position[0],camera_position[1],camera_position[2],
      camera_position[0]+camera_direction[0],
      camera_position[1]+camera_direction[1],
      camera_position[2]+camera_direction[2],  0,1,0); // camera position , "look at" point , view-up vector
    }

    void resetposition(){
      position[0] = 0;
      position[1] = 0;
      position[2] = 0;
      rot_position[0] = 0;
      rot_position[1] = 0;
      rot_position[2] = 0;
    }

    void CCanvas::paintGL()
    {
      // clear screen and depth buffer
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // set model-view matrix
      glMatrixMode(GL_MODELVIEW);

      glLoadIdentity();
//      static float position = 0.0;
//      position += 0.02;
      //    lookAt(sin(position),cos(position),0,  0,0,-1,  0,1,0); // camera position , "look at" point , view-up vector
      free_camera_lookat();
      cout << "camera position " << position[0] << " " << position[1] << " " << position[2] << endl;
      cout << "rotation position " << rot_position[0] << " " << rot_position[1] << " " << rot_position[2] << endl;
      cout << "------------------------" << endl;
      lookAt(position[0], position[1], position[2], rot_position[0], rot_position[1], rot_position[2],  0,1,0); // camera position , "look at" point , view-up vector
      resetposition();

      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glPushMatrix(); // IDENTITY IDENTITY

      //    ##################AXES##################
      setView(View::Axis);
      //    glDisable(GL_LIGHTING);
      //    glColor3f(1.0f, 0.0f, 0.0f);
      //    glBegin(GL_LINES);
      //        glVertex3f(-6.0f, 0.0f, 0.0f);
      //        glVertex3f(6.0f, 0.0f, 0.0f);
      //    glEnd();
      //    glColor3f(0.0f, 1.0f, 0.0f);
      //    glBegin(GL_LINES);
      //        glVertex3f(0.0f, -6.0f, 0.0f);
      //        glVertex3f(0.0f, 6.0f, 0.0f);
      //    glEnd();
      //    glColor3f(0.0f, 0.0f, 1.0f);
      //    glBegin(GL_LINES);
      //        glVertex3f(0.0f, 0.0f, -6.0f);
      //        glVertex3f(0.0f, 0.0f, 6.0f);
      //    glEnd();
      //    glEnable(GL_LIGHTING);
      //    glMatrixMode(GL_MODELVIEW);
      //    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glPushMatrix(); // IDENTITY AXIS AXIS
      glEnable(GL_LIGHTING);

      // ##################AIRPLANE##################

      // ##################MATERIALS##################
      #if 1
      GLfloat amb[]  = {1.0f, 1.0f, 1.0f};
      GLfloat diff[] = {0.7f, 0.7f, 0.7f};
      GLfloat spec[] = {0.1f, 0.1f, 0.1f};
      GLfloat shin = 0.0001;
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shin);
      glColor3f(1.0f, 1.0f, 1.0f);

      texturePlane.bind();
      setView(View::Main_Body);
      plane.objects[0].draw();
      glPushMatrix(); // IDENTITY AXIS MAIN_BODY MAIN_BODY

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

      //     GLfloat matrix[16];
      //     glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
      //     for(int i=0; i < 16; i++){
      //         cout << matrix[i] << " ";
      //     }
      //     cout << endl;
      #endif

    }
