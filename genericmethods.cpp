#include "CCanvas.h"
#include "genericmethods.h"

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
void CCanvas::lookAt2(const GLdouble eyeX,
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
    glLoadIdentity();
    glMultMatrixd(mat);
    GLfloat model[16];
    glGetFloatv (GL_MODELVIEW, model);
    updatemodelview(model);
    glMultMatrixf(model);

    delete[] mat;
}

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

void popandpush(){
  glPopMatrix(); // IDENTITY AXIS MAIN_BODY
  glPushMatrix(); // IDENTITY AXIS MAIN_BODY MAIN_BODY
}

void rotatePointX(Point3d *p, float alpha){
  alpha = alpha * PI / 180;
  float y = cos(alpha)*p->y() - sin(alpha)*p->z();
  float z = sin(alpha)*p->y()+ cos(alpha)*p->z();
  p->y() = y;
  p->z() = z;
}

void rotatePointY(Point3d *p, float alpha){
  alpha = alpha * PI / 180;
  float x = cos(alpha)*p->x() + sin(alpha)*p->z();
  float z = -sin(alpha)*p->x()+ cos(alpha)*p->z();
  p->x() = x;
  p->z() = z;
}

void rotatePointYRad(Point3d *p, float alpha){
  float x = cos(alpha)*p->x() + sin(alpha)*p->z();
  float z = -sin(alpha)*p->x()+ cos(alpha)*p->z();
  p->x() = x;
  p->z() = z;
}

void rotatePointZ(Point3d *p, float alpha){
  alpha = alpha * PI / 180;
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

void initpaintgl(){
    // clear screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glEnable( GL_NORMALIZE );
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void setplanematerial(){
    GLfloat amb[]  = {1.0f, 1.0f, 1.0f};
    GLfloat diff[] = {0.7f, 0.7f, 0.7f};
    GLfloat spec[] = {0.1f, 0.1f, 0.1f};
    GLfloat shin = 0.0001;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shin);
//      glColor3f(1.0f, 1.0f, 1.0f);
}

void initeverything(){
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

  // loadOBJ(pathToFile[0],scene);
  // loadOBJ(pathToFile[1],plane);
//  loadOBJ(pathToFile[2],plane);

  /*
  * Before you can use the texture you need to initialize it by calling the setTexture() method.
  * Before you can use OBJ/PLY model, you need to initialize it by calling init() method.
  */
  // texturePlane.setTexture();
  // textureGrass.setTexture();
  // textureMountain.setTexture();
  // textureWater.setTexture();
  // textureLane.setTexture();
  // textureTree.setTexture();

  //    CCanvas::rotateScene = false;
  // grabKeyboard();
  // camView = Still;
}
