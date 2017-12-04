#include "CCanvas.h"
#include "Base.h"
#include "Sphere.h"

#include "ObjectGroup.h"
#include "objloader.hpp"
using namespace std;
static ObjectGroup tree("");
static ObjectGroup plane("");
//-----------------------------------------------------------------------------

void CCanvas::initializeGL()
{
    glClearColor(0.0f, 0.8f, 0.8f, 0.5f);			   // black background
    glClearDepth(1.0f);								   // depth buffer setup
    glEnable(GL_DEPTH_TEST);						   // enables depth testing
    glDepthFunc(GL_LEQUAL);							   // the type of depth testing to do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // really nice perspective calculations
    glShadeModel(GL_SMOOTH);

    // One light source
//    glEnable(GL_LIGHTING);

//    glEnable(GL_LIGHT0);
    /*
     * The position is transformed by the modelview matrix when glLightfv is called (just as if it were
     * a point), and it is stored in eye coordinates. If the w component of the position is 0.0,
     * the light is treated as a directional source. Diffuse and specular lighting calculations take
     * the light's direction, but not its actual position, into account, and attenuation is disabled.
     * Otherwise, diffuse and specular lighting calculations are based on the actual location of the
     * light in eye coordinates, and attenuation is enabled. The default position is (0,0,1,0); thus,
     * the default light source is directional, parallel to, and in the direction of the -z axis.
     */
    GLfloat lightpos[] = {0.0, 0.0, 1.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

//    GLfloat lightAmb[]  = {0.3, 0.3, 0.3};
//    GLfloat lightDiff[] = {0.4, 0.4, 0.4};
//    GLfloat lightSpec[] = {0.5, 0.5, 0.5};

//    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
//    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmb);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiff);

    GLfloat ambient[] =  { 0.0, 0.0, 0.0, 1.0 };
    GLfloat diffuse[] =  { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 0.0, 1.0, 1.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );

    loadOBJ(pathToFile[0],tree);
    loadOBJ(pathToFile[1],plane);

    /*
     * Before you can use the texture you need to initialize it by calling the setTexture() method.
     * Before you can use OBJ/PLY model, you need to initialize it by calling init() method.
     */
    textureTrain.setTexture();
    textureScene.setTexture();
    textureTree.setTexture();


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

void CCanvas::lookAt(const GLdouble eyex,
                     const GLdouble eyey,
                     const GLdouble eyez,
                     const GLdouble centerx,
                     const GLdouble centery,
                     const GLdouble centerz,
                     const GLdouble upx,
                     const GLdouble upy,
                     const GLdouble upz)
{
    GLdouble *mat = new GLdouble[16];

    // TODO: add computation for the lookat here!
    Point3d X, Y, Z;

    // create new coordinate system
    Z = Point3d(eyex - centerx, eyey - centery, eyez - centerz);
    Z.normalize();

    // compute Y and X
    Y = Point3d(upx, upy, upz);
    X = Y ^ Z;

    // recompute X
    Y = Z ^ X;

    // normalize
    X.normalize();
    Y.normalize();

    Point3d eye(eyex, eyey, eyez);

    mat[0] = X.x();
    mat[1] = X.y();
    mat[2] = X.z();
    mat[3] = -X * eye;

    mat[4] = Y.x();
    mat[5] = Y.y();
    mat[6] = Y.z();
    mat[7] = -Y * eye;

    mat[8]  = Z.x();
    mat[9]  = Z.y();
    mat[10] = Z.z();
    mat[11] = -Z * eye;

    mat[12] = 0.0;
    mat[13] = 0.0;
    mat[14] = 0.0;
    mat[15] = 1.0;

    glMultMatrixd(mat);

    delete[] mat;
}

void CCanvas::resizeGL(int width, int height)
{
    // set up the window-to-viewport transformation
    glViewport(0, 0, width, height);

    // vertical camera opening angle
    double beta = 60.0;

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

void CCanvas::setView(View _view) {
    static float x = 0.0;
    static int stabilize = 0;
    static bool stab = false;

    switch(_view) {
        case Show:
            glTranslatef(-1.0, -4.5, -10.0);
            glTranslatef(1.0, 1.5, -8.0);
//            glScaled(0.5,0.5,0.5);
            break;
        case Takeoff:
            x+=0.2;
            glTranslatef(-1.0, -4.5, -10.0);
            glTranslatef(1.0, 1.5 + x/2, -8.0 - x); // take off
//            glTranslatef(1.0, -2.5, -10.0); // put in the axis
//            glRotatef(x, 0.0f, x, 0.0f); //rotate on itself
            if(x/2 < 30 && stabilize == 0){
                glRotatef(x/2, 1.0f, 0.0f, 0.0f); //take off
            }else{
                if(!stab){
                    stabilize = x/2;
                    stab = true;
                }
                glRotatef(stabilize, 1.0f, 0.0f, 0.0f); // to modify (stabilize)
            }

            break;
        case Axis:
            // Maybe you want to have an option to view the scene from the train cockpit, up to you
            glTranslatef(1.0, -2.5, -10.0);
            glScaled(2.0f,2.0f,2.0f);
//            glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
            break;
        }

}

void CCanvas::paintGL()
{
    // clear screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    lookAt(0,0,0,  0,0,-1,  0,1,0); // camera position , "look at" point , view-up vector

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPushMatrix();


//    ##################LIGHT##################
    GLfloat lightpos[] = { -3.5, 3.5, 10.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);


//    ##################AXES##################
// setView(View::Axis);
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
//    glLoadIdentity();
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    glDisable(GL_LIGHTING);
//    glPopMatrix();

//    ##################FLAT SURFACE (GREEN PLANE)##################
//    setView(View::Axis);

//    glColor3f(0.0f, 0.5f, 0.0f);
//        glNormal3d(0.0000, 1.0000, 0.0000);
//        glBegin(GL_TRIANGLE_STRIP);
//        glVertex3f(-108.231724, 0.358408, 109.108315);
//        glVertex3f(100.418297, 0.358408, 109.108315);
//        glVertex3f(-108.231724, 0.358408, -109.541705);
//        glVertex3f(100.418297, 0.358408, -109.541705);
//        glEnd();
//     glPopMatrix();

//     glColor3f(1.0f, 1.0f, 1.0f);




    // ##################AIRPLANE##################
    static bool showb = true;

    if(showb){
        setView(View::Show);

    }else{

        setView(View::Takeoff);
    }

    // ##################MATERIALS##################

//    glColor3f(0.5f, 0.5f, 0.5f);
    GLfloat amb[]  = {1.0f, 1.0f, 1.0f};
    GLfloat diff[] = {0.7f, 0.7f, 0.7f};
    GLfloat spec[] = {0.1f, 0.1f, 0.1f};
    GLfloat shin = 0.0001;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shin);

    // Drawing the object with texture
    textureTrain.bind();
    /*
     * Obtaining the values of the current modelview matrix
     *  GLfloat matrix[16];
     *  glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
    */

    if(showb){

        static float show = 0.1;
        float showp = 0;
        show += 0.5;
        for(int i =0; i<plane.objects.size(); i++){
            if(show > showp){
                plane.objects[i].draw();
            }
            showp += 10;
        }
        if(show > 100){
            showb = false;
        }
     }else{
        for(int i =0; i<plane.objects.size(); i++){
            plane.objects[i].draw();
        }
    }


//    for(int i =0; i<plane.objects.size(); i++){
//        plane.objects[i].draw();
//    }
    glPopMatrix();

    setView(View::Axis);
    textureTrain.unbind();
    textureTree.bind();
    for(int i =0; i<tree.objects.size(); i++){
        if(i == 1){
           textureTree.unbind();
           textureScene.bind();
           tree.objects[i].draw();
           textureScene.unbind();
           textureTree.bind();
        }else{
           tree.objects[i].draw();
        }

    }
    textureTree.unbind();



    /*
     * The models you load can have different scales. If you are drawing a proper model but nothing
     * is shown, check the scale of the model, your camera could be for example inside of it.
     */
    //glScalef(0.02f, 0.02f, 0.02f);

    // Remove the last transformation matrix from the stack - you have drawn your last
    // object with a new transformation and now you go back to the previous one
    glPopMatrix();

}
