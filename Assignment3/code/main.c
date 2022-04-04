/*******************************************************************
 Multi-Part Model Construction and Manipulation
 ********************************************************************/

#ifdef __APPLE__
//#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Vector3D.c"
#include "QuadMesh.c"
#include "CubeMesh.c"



#define PI 3.14159265

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 1280;     // Viewport width in pixels
const int vHeight = 720;    // Viewport height in pixels
int width = 1280;
int height = 720;

const double droneXSCALE = 1.0;
const double droneYSCALE = 0.5;
const double droneZSCALE = 1.0;

static unsigned int buildingTexture;
static unsigned int droneTexture;
static unsigned int roadTexture;
static unsigned int skyTexture;

double droneXPOS = 0.0;
double droneYPOS = 1.5;
double droneZPOS = 0.0;

double droneTHETA = 90.0;
double droneXROTATE = 0.0;
double droneYROTATE = 1.0;
double droneZROTATE = 0.0;


double edroneXPOS = 7.0;
double edroneYPOS = 3.0;
double edroneZPOS = 0.0;

double edroneXSCALE = 1.0;
double edroneYSCALE = 1.5;
double edroneZSCALE = 1.0;

double edroneTHETA = 90.0;
double edroneXROTATE = 0.0;
double edroneYROTATE = 1.0;
double edroneZROTATE = 0.0;
bool hit = false;
GLdouble droneXCAM;
GLdouble droneZCAM;
GLdouble fDroneX;
GLdouble fDroneY;
GLdouble fDroneZ;
GLdouble tilt = 0;
GLdouble zoomX;
GLdouble zoomY;
GLdouble zoomZ;
int zoomRange = 0;
double buildingRanges[200];

bool thirdPerson = false;
bool firstPerson = false;
bool crashed = false;

int shooting = 0;
bool brokenDrone;

GLdouble misslePOSX;
GLdouble misslePOSY;
GLdouble misslePOSZ;

double propRotate1 = 45.0;

//Variables for creating textures
int texWidth;
int texHeight;
int nrChannels;

//Texture variables for each required texture
static unsigned int droneTexture;

int buildings = 17;
static GLfloat theta1 = 0.0;

float x, y;
float z = 22;
GLint leftMouseButton, rightMouseButton;    //status of the mouse buttons
int mouseX = 0, mouseY = 0;                 //last known X and Y of the mouse
float cameraTheta, cameraPhi, cameraRadius; //camera position in spherical coordinate
GLdouble FOV = 60;

static int currentButton;
static unsigned char currentKey;

static CubeMesh droneCube, buildCube, streetCube, bullet, sky;


// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void drawBody();
void drawArm1();
void drawArm2();
void drawArm3();
void drawArm4();
void drawEnemy();
void spinDisplay(void);
void recomputeOrientation();
void bulletAngle(void);
void fireBullet(void);
void moveEnemy();
void returnEnemy();
void drawSky();
void detectCollision();
bool isCrashed();
void isHit();

Vector3D ScreenToWorld(int x, int y);


int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("Assignment 3");
    
    cameraRadius = 20.0f;
    cameraTheta = 2.80;
    cameraPhi = 2.0;
    recomputeOrientation();
    
    // Initialize GL
    initOpenGL(vWidth, vHeight);
    
    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotionHandler);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(functionKeys);
    
    // Start event loop, never returns
    glutMainLoop();
    
    return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
     //This light is currently on
    
    unsigned char *droneData = stbi_load("droneTex.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (droneData)
    {
        glGenTextures(1, &droneTexture);
        glBindTexture(GL_TEXTURE_2D, droneTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, droneData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    }
    else
    {
        printf("Failed to load texture \n");
        
    }
    
    stbi_image_free(droneData);
    
    unsigned char *buildingData = stbi_load("buildNEW.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (buildingData)
    {
        glGenTextures(1, &buildingTexture);
        glBindTexture(GL_TEXTURE_2D, buildingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, buildingData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    }
    else
    {
        printf("Failed to load texture \n");
        
    }
    
    stbi_image_free(buildingData);
    
    unsigned char *roadData = stbi_load("roadTex.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (roadData)
    {
        glGenTextures(1, &roadTexture);
        glBindTexture(GL_TEXTURE_2D, roadTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, roadData);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    }
    else
    {
        printf("Failed to load texture \n");
        
    }
    
    stbi_image_free(roadData);
    
    
    
    
    
    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see
    glClearColor(0.0F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective
    
    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-25.0f, 0.0f, 25.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
   // InitMeshQM(&groundMesh, meshSize, origin, 50.0, 50.0, dir1v, dir2v);
    
    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
 //   SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);
    
    droneCube = newCube();
    buildCube = newCube();
    streetCube = newCube();
    bullet = newCube();
    sky = newCube();
    
    
}

void recomputeOrientation()
{
    //Equations found online for the camera position
    x = cameraRadius * sinf(cameraTheta)*sinf(cameraPhi);
    z = cameraRadius * -cosf(cameraTheta)*sinf(cameraPhi);
    y = cameraRadius * -cosf(cameraPhi);
    glutPostRedisplay();
    
}

void drawBody()
{
    
    //Pushes the current transformation matric (CTM) on the stack. (Though the current one is empty).
    
    glPushMatrix();
    
    glTranslatef(droneXPOS, droneYPOS, droneZPOS);
    
    glRotatef(droneTHETA, droneXROTATE, droneYROTATE, droneZROTATE);
    
    glScalef(droneXSCALE/2.0, droneYSCALE/2.0, droneZSCALE/2.0);
    
    drawCube(&droneCube, droneTexture);
    //draw
    
    
    glScalef(1 / droneXSCALE, 1 / droneYSCALE, 1 / droneZSCALE);
    //Inverse the rotation
    // CTM = T1.
    glRotatef(-45.0, 0.0, 1.0, 0.0);
    // Finally inverse the translation.
    glTranslatef(-1.0, -3.5, -8.0);
    
    drawArm1();
    drawArm2();
    drawArm3();
    drawArm4();
    //Pop the current CTM matrix.
    glPopMatrix();
}

void drawArm1()
{
    glPushMatrix();
    
    glTranslatef(2.0, 3.50, 8.0);
    
    glRotatef(90.0, 0.0, 1.0, 0.0);
    
    glScalef(0.3, 0.15, 1.5);
    //Draw the arm
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    
    glScalef(1 / 0.3, 1 / 0.15, 1 / 1.5);
    //CTM = T1 * R1
    glRotatef(-90.0, 0.0, 1.0, 0.0);
    //CTM = T1
    glTranslatef(-5.0, -3.50, -8.0);
    //CTM Is empty
    //CTM = T1
    glTranslatef(6.0, 3.5, 8.0);
    //CTM = T1 * R1
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    //CTM = T1 * R1 * S1
    glScalef(0.5/2.0, 0.75/2.0, 1.0/2.0);
    //Draw the end piece of the arm
    drawCube(&droneCube, droneTexture);
    //Push the next matrix on the stack
    glPushMatrix();
    //inverse the previous transformations again
    glScalef(1/0.5, 1/0.75, 1/1.0);
    glTranslatef(-6.0, -3.5, -8.65);
    //CTM = T1
    glTranslatef(6.0, 3.5, 9.2);
    //CTM = T1 * R1
    glRotatef(theta1, 0.0, 0.0, 1.0);
    //CTM = T1 * R1 * R2
    //Second rotation was needed here to re-allign the propeller to stay flat while turning
    glRotatef(90.0, 0.0, 1.0, 0.0);
    //CTM = T1 * R1 * R2 * S1
    glScalef(0.1, 0.15, 1.5);
    //Draw the propeller
    drawCube(&droneCube, droneTexture);
    
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
    //Pop the matricies of the stack
}
void drawArm2()
{
    //I drew each arm in almost the exact same pattern/order every time just with slightly different translation numbers
    //Same scaling factor everytime, so I will not go over the CTM's since it's the same as the previous method.
    glPushMatrix();
    
    glTranslatef(1.0, 3.50, 9.0);
    glRotatef(15.0, 0.0, 1.0, 0.0);
    glScalef(0.3, 0.15, 1.5);
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    //Inverse so we can add another object
    glScalef(1 / 0.3, 1 / 0.15, 1 / 1.5);
    glRotatef(-15.0, 0.0, 1.0, 0.0);
    glTranslatef(-1.0, -3.50, -10.0);
    
    glTranslatef(1.3, 3.5, 10.8);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(73.0, 0.0, 0.0, 1.0);
    glScalef(0.5/2.0, 0.75/2.0, 1.0/2.0);
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    
    glScalef(1/0.5, 1/0.75, 1/1.0);
    glTranslatef(-1.8, -3.5, -12.35);
    
    glTranslatef(1.8, 3.5, 11.8);
    glRotatef(theta1, 0.0, 0.0, 1.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glScalef(0.1, 0.15, 1.5);
    drawCube(&droneCube, droneTexture);
    
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
}

void drawArm3()
{
    glPushMatrix();
    //Inverse x translation as I knew this arm needed to be the same as arm1 just reflected over the x axis.
    glTranslatef(0.0, 3.50, 8.0);
    glRotatef(-90.0, 0.0, -1.0, 0.0);
    glScalef(0.3, 0.15, 1.5);
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    //Inverse so we can add another object
    glScalef(1 / 0.3, 1 / 0.15, 1 / 1.5);
    glRotatef(-15.0, 0.0, 1.0, 0.0);
    glTranslatef(-0.4, -3.50, -7.5);
    
    glTranslatef(0.2, 3.5, 7.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(73.0, 0.0, 0.0, 1.0);
    glScalef(0.5/2.0, 0.75/2.0, 1.0/2.0);
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    
    glScalef(1 / 0.5, 1 / 0.75, 1 / 1.0);
    glTranslatef(-0.2, -3.5, -6.35);
    
    glTranslatef(0.2, 3.5, 5.8);
    glRotatef(theta1, 0.0, 0.0, 1.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glScalef(0.1, 0.15, 1.5);
    drawCube(&droneCube, droneTexture);
    
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
}

void drawArm4()
{
    glPushMatrix();
    //Inverse x translation here again as this is essentially a flipped image of arm2.
    glTranslatef(0.8, 3.50, 7.0);
    glRotatef(-15.0, 0.0, -1.0, 0.0);
    glScalef(0.3, 0.15, 1.5);
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    //Inverse so we can add another object
    glScalef(1 / 0.3, 1 / 0.15, 1 / 1.5);
    glRotatef(-15.0, 0.0, 1.0, 0.0);
    glTranslatef(-0.4, -3.50, -7.465);
    
    glTranslatef(0.2, 3.5, 7.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(73.0, 0.0, 0.0, 1.0);
    glScalef(0.5/2.0, 0.75/2.0, 1.0/2.0);
    drawCube(&droneCube, droneTexture);
    
    glPushMatrix();
    
    glScalef(1 / 0.5, 1 / 0.75, 1 / 1.0);
    glTranslatef(-0.2, -3.5, -6.35);
    glTranslatef(0.2, 3.5, 5.8);
    glRotatef(theta1, 0.0, 0.0, 1.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glScalef(0.1, 0.15, 1.5);
    drawCube(&droneCube, droneTexture);
    
    glPopMatrix();
    
    glPopMatrix();
    
    glPopMatrix();
}

void drawCity()
{
    // top middle right
    glPushMatrix();
    glTranslatef(7.0, 4.0, -7.0);
    glScalef(2.0, 4.0, 1.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[0] = 5.0;
    buildingRanges[1] = 9.0;
    buildingRanges[2] = -8.0;
    buildingRanges[3] = -6.0;
    buildingRanges[4] = 8.0;
    
    
    glPushMatrix();
    glTranslatef(10.0, 3.0, -10);
    glScalef(3.0/2.0, 6.0/2.0, 3.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[5] = 8.5;
    buildingRanges[6] = 11.5;
    buildingRanges[7] = -8.5;
    buildingRanges[8] = -11.5;
    buildingRanges[9] = 6.0;
    
    glPushMatrix();
    glTranslatef(5.0, 2.0, -11);
    glScalef(3.0/2.0, 4.0/2.0, 4.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[10] = 3.6;
    buildingRanges[11] = 6.5;
    buildingRanges[12] = -13;
    buildingRanges[13] = 9.0;
    buildingRanges[14] = 4.0;

    
    glPushMatrix();
    glTranslatef(8.0, 5.5, -15);
    glScalef(4.0/2.0, 11.0/2.0, 3.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[15] = 6.0;
    buildingRanges[16] = 10.0;
    buildingRanges[17] = -16.5;
    buildingRanges[18] = -13.5;
    buildingRanges[19] = 10.5;
    
    glPushMatrix();
    glTranslatef(-8.0, 3.0, -8.0);
    glScalef(4.0/2.0, 6.0/2.0, 5.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[20] = -10;
    buildingRanges[21] = -6;
    buildingRanges[22] = -10.5;
    buildingRanges[23] = -5.5;
    buildingRanges[24] = 6.0;
    
    
    glPushMatrix();
    glTranslatef(-8.0, 6.0, -8.0);
    glScalef(3.0/2.0, 9.0/2.0, 3.5/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[25] = -9.5;
    buildingRanges[26] = -6.6;
    buildingRanges[27] = -9.75;
    buildingRanges[28] = -6.25;
    buildingRanges[29] = 10.5;
    
    glPushMatrix();
    glTranslatef(-8.0, 8.0, -8.0);
    glScalef(2.0/2.0, 11.0/2.0, 2.5/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[30] = -9.5;
    buildingRanges[31] = -6.6;
    buildingRanges[32] = -9.75;
    buildingRanges[33] = -6.25;
    buildingRanges[34] = 10.5;
    
    glPushMatrix();
    glTranslatef(-8.0, 3.0, -14.0);
    glScalef(4.0/2.0, 6.0/2.0, 5.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[35] = -10;
    buildingRanges[36] = -6;
    buildingRanges[37] = -16.5;
    buildingRanges[38] = -11.5;
    buildingRanges[39] = 6.0;
    
    
    glPushMatrix();
    glTranslatef(-8.0, 6.0, -14.0);
    glScalef(3.0/2.0, 9.0/2.0, 3.5/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[40] = -9.5;
    buildingRanges[41] = -6.5;
    buildingRanges[42] = -15.75;
    buildingRanges[43] = -12.25;
    buildingRanges[44] = 10.5;
    
    glPushMatrix();
    glTranslatef(-8.0, 8.0, -14.0);
    glScalef(2.0/2.0, 11.0/2.0, 2.5/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[45] = -9.0;
    buildingRanges[46] = -7.0;
    buildingRanges[47] = -14.5;
    buildingRanges[48] = -13.5;
    buildingRanges[49] = 13.5;
    
    //bottom left middle
    
    glPushMatrix();
    glTranslatef(-7.0, 4.0, 11.0);
    glScalef(5.0/2.0, 8.0/2.0, 13.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[50] = -9.5;
    buildingRanges[51] = -4.5;
    buildingRanges[52] = 4.5;
    buildingRanges[53] = 17.5;
    buildingRanges[54] = 8.0;
    
    glPushMatrix();
    glTranslatef(-7.0, 5.0, 11.0);
    glScalef(3.0/2.0, 11.0/2.0, 10.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[55] = -8.5;
    buildingRanges[56] = -5.5;
    buildingRanges[57] = 6.0;
    buildingRanges[58] = 16.0;
    buildingRanges[59] = 10.5;
    
    // bottom right middle
    
    glPushMatrix();
    glTranslatef(7.0, 7.0, 17.8);
    glScalef(4.0/2.0, 15.0/2.0, 2.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[60] = 5.0;
    buildingRanges[61] = 10.0;
    buildingRanges[62] = 16.8;
    buildingRanges[63] = 15.8;
    buildingRanges[64] = 14.5;
    
    glPushMatrix();
    glTranslatef(7.0, 6.0, 15.0);
    glScalef(4.0/2.0, 12.0/2.0, 2.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[65] = 5.0;
    buildingRanges[66] = 9.0;
    buildingRanges[67] = 14.0;
    buildingRanges[68] = 15.0;
    buildingRanges[69] = 12.0;
    
    glPushMatrix();
    glTranslatef(7.0, 5.0, 12.0);
    glScalef(4.0/2.0, 10.0/2.0, 2.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[70] = 5.0;
    buildingRanges[71] = 9.0;
    buildingRanges[72] = 11.0;
    buildingRanges[73] = 13.0;
    buildingRanges[74] = 10.0;
    
    glPushMatrix();
    glTranslatef(7.0, 4.0, 9.0);
    glScalef(4.0/2.0, 8.0/2.0, 2.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[75] = 5.0;
    buildingRanges[76] = 9.0;
    buildingRanges[77] = 8.0;
    buildingRanges[78] = 10.0;
    buildingRanges[79] = 8.0;
    
    glPushMatrix();
    glTranslatef(7.0, 3.0, 6.0);
    glScalef(4.0/2.0, 6.0/2.0, 2.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    buildingRanges[80] = 5.0;
    buildingRanges[81] = 9.0;
    buildingRanges[82] = 5.0;
    buildingRanges[83] = 7.0;
    buildingRanges[84] = 6.0;
    
    //bottom right
    
    glPushMatrix();
    glTranslatef(21.0, 4.0, 6.0);
    glScalef(4.0/2.0, 8.0/2.0, 5.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(21.0, 2.0, 14.0);
    glScalef(2.0/2.0, 5.0/2.0, 8.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    //bot left
    
    glPushMatrix();
    glTranslatef(21.0, 1.5, -16.0);
    glScalef(4.0/2.0, 3.0/2.0, 5.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(22.0, 2.0, -7.0);
    glScalef(4.0/2.0, 4.0/2.0, 6.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    //top left
    
    glPushMatrix();
    glTranslatef(-21.0, 1.5, -16.0);
    glScalef(4.0/2.0, 3.0/2.0, 5.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-21.0, 5.5, -7.0);
    glScalef(4.0/2.0, 11.0/2.0, 6.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    //top right
    
    glPushMatrix();
    glTranslatef(-21.0, 6.0, 16.0);
    glScalef(4.0/2.0, 12.0/2.0, 5.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-20.0, 6.0, 7.0);
    glScalef(4.0/2.0, 12.0/2.0, 6.0/2.0);
    drawCube(&buildCube, buildingTexture);
    glPopMatrix();
    
    //streets
    
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.0);
    glScalef(6.0, 0.125, 49.0);
    drawCube(&streetCube, roadTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(15.0, 0.1, 0.0);
    glScalef(6.0, 0.125, 49.0);
    drawCube(&streetCube, roadTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-15.0, 0.1, 0.0);
    glScalef(6.0, 0.125, 49.0);
    drawCube(&streetCube, roadTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.1, 0.0);
    glScalef(49.0, 0.125, 6.0);
    drawCube(&streetCube, roadTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.1, 22.0);
    glScalef(49.0, 0.125, 6.0);
    drawCube(&streetCube, roadTexture);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.1, -22.0);
    glScalef(49.0, 0.125, 6.0);
    drawCube(&streetCube, roadTexture);
    glPopMatrix();
}

void display(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //up vector is (0,1,0) (positive Y)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (thirdPerson) {
        droneXCAM = droneXPOS + (2.5 * sinf((droneTHETA * PI) / 180));
        droneZCAM = droneZPOS + (2.5 * cosf((droneTHETA * PI) / 180));
        gluLookAt(droneXCAM, droneYPOS + 3.0, droneZCAM, droneXPOS, droneYPOS, droneZPOS, 0.0, 1.0, 0.0);
    }
    
    else if (firstPerson) {
        fDroneY = droneYPOS + tilt;
        fDroneX = (droneXPOS - (5 * sinf((droneTHETA * PI) / 180)));
        fDroneZ = (droneZPOS - (5 * cosf((droneTHETA * PI) / 180)));
        
        gluLookAt(droneXPOS + zoomX, droneYPOS + 0.5, droneZPOS + zoomZ, fDroneX, fDroneY, fDroneZ, 0.0, 1.0, 0.0);
    }
    else {
        gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
    
    // Set drone material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);
    
    glutPostRedisplay();
    
    brokenDrone = isCrashed();
    if (brokenDrone == false)
    {
        drawBody();
    }
    
    if (!hit) {
        drawEnemy();
        moveEnemy();
    }
    //drawSky();
    drawCity();
    
    if (shooting == 1) {
        //misslePOSX = misslePOSX - 0.05 * sinf(droneTHETA * PI / 180);
        //misslePOSZ = misslePOSZ - 0.05 * cosf(droneTHETA * PI / 180);
        
        for (int i = 0; i < 100; i++)
        {
            fireBullet();
            
        }
        
        
    }
    glutPostRedisplay();
    
    
    
    // Draw ground mesh
  //  DrawMeshQM(&groundMesh, meshSize);
    
    glutSwapBuffers();   // Double buffering, swap buffers
}

void drawEnemy()
{
    
    glPushMatrix();
    
    glTranslatef(edroneXPOS, edroneYPOS, edroneZPOS);
    
    glRotatef(edroneTHETA, edroneXROTATE, edroneYROTATE, edroneZROTATE);
    
    glScalef(edroneXSCALE/2.0, edroneYSCALE/4.0, edroneZSCALE/2.0);
    
    drawCube(&droneCube, droneTexture);
    //draw
    
    
    glScalef(1 / edroneXSCALE, 1 / edroneYSCALE, 1 / edroneZSCALE);
    //Inverse the rotation
    // CTM = T1.
    glRotatef(-45.0, 0.0, 1.0, 0.0);
    // Finally inverse the translation.
    glTranslatef(-1.0, -3.5, -8.0);
    
    drawArm1();
    drawArm2();
    drawArm3();
    drawArm4();
    //Pop the current CTM matrix.
    glPopMatrix();
}

void drawSky()
{
    glPushMatrix();
    glScalef(300,300,300);
    drawCube(&sky, skyTexture);
    glPopMatrix();
}


void moveEnemy()
{
    if(edroneXPOS <= 20){
        edroneXPOS++;
    }else{
        edroneXPOS = -25;
    }
}


void returnEnemy()
{
    edroneXPOS = edroneXPOS - 1;
    if ((edroneXPOS <= 20)) {
        moveEnemy();
    }
}
void bulletAngle(void)
{
    misslePOSX = misslePOSX - 0.01 * sinf(droneTHETA * PI / 180);
    misslePOSZ = misslePOSZ - 0.01 * cosf(droneTHETA * PI / 180);
    
}

void fireBullet(void)
{
    bulletAngle();
    glPushMatrix();
   // CubeMesh projectile = newCube();
    glTranslatef(droneXPOS + misslePOSX, droneYPOS, droneZPOS + misslePOSZ);
    glScalef(0.1, 0.1, 0.1);
    drawCube(&bullet, NULL);
    glPopMatrix();
    
}

bool isCrashed()
{
    if (droneYPOS <= 0.5)
    {
        droneYPOS = -250;
        return true;
    }
    
    for (int i = 0; i < buildings; i++)
    {
        if (droneXPOS >= buildingRanges[0 + (5 * i)] && droneXPOS <= buildingRanges[1 + (5 * i)] && droneZPOS >= buildingRanges[2 + (5 * i)] && droneZPOS <= buildingRanges[3 + (5 * i)] && droneYPOS <= buildingRanges[4 + (5 * i)])
        {
            droneYPOS = -250;
            return true;
        }
    }
    return false;

}

void isHit()
{
    if (misslePOSX <= (edroneXPOS + 1) && misslePOSX >= (edroneXPOS - 1) && (misslePOSY == edroneYPOS) && misslePOSZ <= (edroneXPOS + 1) && misslePOSZ >= (edroneZPOS - 1))
    {
        hit = true;
    }
    hit = false;
    
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
    
    //Used for the zoom
    w = width;
    h = height;
    
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
    //gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'q':
        case 'Q':
            exit(0);
            break;
        case 'F':
        case 'f':
            isCrashed();
            droneXPOS = droneXPOS - 0.2 * sinf(droneTHETA * PI / 180);
            droneZPOS = droneZPOS - 0.2 * cosf(droneTHETA * PI / 180);
            glutIdleFunc(spinDisplay);
            break;
        case 'B':
        case 'b':
            isCrashed();
            droneXPOS = droneXPOS + 0.2 * sinf(droneTHETA * PI / 180);
            droneZPOS = droneZPOS + 0.2 * cosf(droneTHETA * PI / 180);
            glutIdleFunc(spinDisplay);
            break;
        case 'Z':
        case 'z':
            droneTHETA += 5.5;
            break;
        case 'X':
        case 'x':
            droneTHETA += -5.5;
            break;
        case 'S':
        case 's':
            glutIdleFunc(spinDisplay);
            break;
        case'p':
            FOV = FOV - 5;
            reshape(width, height);
            break;
        case'l':
            FOV = FOV + 5;
            reshape(width, height);
            break;
        case'i':
            tilt = tilt + 0.1;
            break;
        case'o':
            tilt = tilt - 0.1;
            break;
            
        case ' ':
            shooting = 1;
            isHit();
            //fireBullet();
            if ((misslePOSX = edroneXPOS) && (misslePOSZ = edroneZPOS)) {
                printf("hit");
                //hit = true;
            }
            break;
        case 'r':
            shooting = 0;
            break;
            
        case'y':
            if (zoomRange < 20) {
                zoomX = zoomX - 0.1 * sinf(droneTHETA * PI / 180);
                zoomZ = zoomZ - 0.1 * cosf(droneTHETA * PI / 180);
                zoomRange++;
            }
            break;
        case't':
            if (zoomRange != 0) {
                zoomX = zoomX + 0.1 * sinf((droneTHETA * PI) / 180);
                zoomZ = zoomZ + 0.1 * cosf(droneTHETA * PI / 180);
                zoomRange--;
                break;
            }
    }
    glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
    // Help menu
    if (key == GLUT_KEY_F1)
    {
        printf("HELP MENU\n f = move forward\n b = move backwards\n s = start propeller spin\n z = turn left\n x = turn right\n and the arrow keys to control movement\n. Hitting f2 enters third person mode.\n Hitting f3 enters first person mode. \nIn first person you can use y and t to zoom the camera, while using i and o to tilt the camera. \n Space shoots the bullet. \n Q to quit.");
    }
    
    else if (key == GLUT_KEY_F2)
    {
        thirdPerson = true;
        glutPostRedisplay();
    }
    
    else if (key == GLUT_KEY_F3)
    {
        thirdPerson = false;
        firstPerson = true;
        glutPostRedisplay();
    }
    else if (key == GLUT_KEY_UP)
    {
        //Drone flies up
        isCrashed();
        droneYPOS += 0.5;
        glutIdleFunc(spinDisplay);
    }
    
    else if (key == GLUT_KEY_DOWN)
    {
        //Drone flies down
        isCrashed();
        droneYPOS += -0.5;
        
        glutIdleFunc(spinDisplay);
        
    }
    
    else if (key == GLUT_KEY_RIGHT)
    {
        isCrashed();
        droneZPOS = droneZPOS - 0.5 * sinf(droneTHETA * PI / 180);
        droneXPOS = droneXPOS + 0.5 * cosf(droneTHETA * PI / 180);
        glutIdleFunc(spinDisplay);
        
    }
    
    else if (key == GLUT_KEY_LEFT)
    {
        isCrashed();
        //Drone flies left
        droneZPOS = droneZPOS + 0.5 * sinf(droneTHETA * PI / 180);
        droneXPOS = droneXPOS - 0.5 * cosf(droneTHETA * PI / 180);
        glutIdleFunc(spinDisplay);
    }
    
    glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to
void mouse(int button, int state, int thisX, int thisY)
{
    //update the left and right mouse button states, if applicable
    //Handles the camera, when the mouse button is clicked...
    if (button == GLUT_LEFT_BUTTON)
        leftMouseButton = state;
    else if (button == GLUT_RIGHT_BUTTON)
        rightMouseButton = state;
    
    //and update the last seen X and Y coordinates of the mouse
    mouseX = thisX;
    mouseY = thisY;
    
    currentButton = button;
    //Unused
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                ;
                
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                ;
            }
            break;
            
            
            
    }
    
    
}


// Mouse motion callback - use only if you want to
void mouseMotionHandler(int x, int y)
{
    //Handles when you click to drag the camera, updates the Theta and Phi angles BASED ON the x and y position of your
    //mouse cursor (only when you hold down the left button).
    if (leftMouseButton == GLUT_DOWN)
    {
        cameraTheta += (x - mouseX)*0.005;
        cameraPhi += (y - mouseY)*0.005;
        
        // make sure that phi stays within the range (0 -> 3.14159)
        if (cameraPhi <= 0)
            cameraPhi = 0 + 0.001;
        if (cameraPhi >= 3.14159)
            cameraPhi = 3.14159 - 0.001;
        
        recomputeOrientation();     //update camera (x,y,z) based on (radius,theta,phi)
    }
    else if (rightMouseButton == GLUT_DOWN) {
        double totalChangeSq = (x - mouseX) + (y - mouseY);
        
        cameraRadius += totalChangeSq * 0.01;
        
        //limit the camera radius so you can't go in weird areas
        if (cameraRadius < 2.0)
            cameraRadius = 2.0;
        if (cameraRadius > 10.0)
            cameraRadius = 10.0;
        
        recomputeOrientation();
    }
    mouseX = x;
    mouseY = y;
}


Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}

void spinDisplay(void)
{
    
    theta1 += 20.2;
    if (theta1 > 360.0)
        theta1 -= 360.0;
    
    glutPostRedisplay();
}

// this is with working bullet
