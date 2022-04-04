/*******************************************************************
 Multi-Part Model Construction and Manipulation
 ********************************************************************/

/*******************************************************************
 Jan Jedrasik Assign1 500642763
 *******************************************************************/

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

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels



bool first_person = false;
bool third_person = false;
bool shootTrig = false;
bool droneCollision = false;



int texWidth;
int texHeight;
int nrChannels;



static int currentButton;
static unsigned char currentKey;
//We're gonna make a rotor switch
//This checks if the 's' key is pressed
static int rotor = 0;
static GLfloat missleSpeed = 0.0;
static GLfloat spin  = 0.0;
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
static GLfloat drone_mat_diffuse[] = { 0.0F, 1.0F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 1.0F };

static int posXThresh = 0;
static int posYThresh = 0;
static int posZThresh = 0;

static int negXThresh = 0;
static int negYThresh = 0;
static int negZThresh = 0;

static float droneY = 4;
static float droneX = 0;
static float droneZ = 0;

static float enemyY = 3.5;
static float enemyX = 0;
static float enemyZ = 0;

static float missleX;
static float missleZ;

static float droneAngle;
static float enemyAngle;

static unsigned int buildingTexture;
static unsigned int droneTexture;
static unsigned int roadTexture;
static unsigned int skyTexture;

static GLfloat droneXCam;
static GLfloat droneZCam;
static GLfloat camXAngle;
static GLfloat camZAngle;
static GLfloat fDroneXCam;
static GLfloat fDroneZCam;
static GLfloat fDroneYCam = 0;
static GLfloat droneTilt = 0;
static GLfloat droneZoomX = 0;
static GLfloat droneZoomZ = 0;
static int zoomCounters = 0;

static CubeMesh enemyDrone;
static CubeMesh projectileArray[100];
static CubeMesh projectile;
static int projectileCounter = 0;
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
void keyboardUp(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);

void drawDroneProps(void);
void spinProps(void);
void draw(void);
void buildDrone(void);

void drawCity(unsigned int texture);
void drawDrone(unsigned int texture);
void drawRoads(unsigned int texture);
void drawSky(unsigned int texture);
void drawEnemyDrone(unsigned int texture);
void projectileMovement(void);
void shootProjectile(unsigned int texture);
void checkCollision(void);
void enemyAI(void);
Vector3D ScreenToWorld(int x, int y);

static CubeMesh building;




int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("Assignment 3");
    
    
    
    // Initialize GL
    initOpenGL(vWidth, vHeight);
    
    // Register callbacks
    glutIdleFunc(display);
    glutIdleFunc(spinProps);
    glutIdleFunc(enemyAI);
    glutIdleFunc(display);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotionHandler);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
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
    //glEnable(GL_LIGHT1);   // This light is currently off
    //Lets set up some textures
    //Loads Building Texture
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
    
    unsigned char *droneData = stbi_load("robottexture.jpg", &texWidth, &texHeight, &nrChannels, 0);
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
    //Road Texture
    unsigned char *roadData = stbi_load("roadtexture.jpg", &texWidth, &texHeight, &nrChannels, 0);
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
    
    unsigned char *skyData = stbi_load("sky.jpg", &texWidth, &texHeight, &nrChannels, 0);
    if (skyData)
    {
        glGenTextures(1, &skyTexture);
        glBindTexture(GL_TEXTURE_2D, skyTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, skyData);
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
    
    stbi_image_free(skyData);
    
    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see
    glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective
    
    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-32.0f, 0.0f, 32.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 64.0, 64.0, dir1v, dir2v);
    
    
    
    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);
    
    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0,  8.0);
    
    
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glutPostRedisplay();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLdouble)650 / 500, 0.2, 500.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    

    
    droneXCam =  droneX + (1.5 * sinf(droneAngle * 3.14159265359 / 180));
    droneZCam = droneZ +(1.5 * cosf(droneAngle * 3.14159265359 / 180));
    
    fDroneYCam = droneY + droneTilt;
    fDroneXCam = (droneX - (5* sinf(droneAngle * 3.14159265359 / 180)));
    fDroneZCam = (droneZ - (5* cosf(droneAngle * 3.14159265359 / 180)));
    
    // Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
     if (third_person) {
        gluLookAt(droneXCam, droneY + 0.3, droneZCam, droneX, droneY , droneZ, 0.0, 1.0, 0.0);
    } else if (first_person) {
        gluLookAt(droneX + droneZoomX, droneY - 0.3, droneZ + droneZoomZ, fDroneXCam,  fDroneYCam, fDroneZCam, 0.0, 1.0, 0.0);
    } else {
        gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    }
    
    
  
    glutIdleFunc(spinProps);
    //And then call postRedisplay for good measure
    glutPostRedisplay();
    drawCity(buildingTexture);
    if(!droneCollision){
        drawDrone(droneTexture);
    }
    drawRoads(roadTexture);
    drawSky(skyTexture);
    drawEnemyDrone(droneTexture);
    if(shootTrig == 1) {
        for(int i = 0; i < 100; i++)
        {
            shootProjectile(buildingTexture);

        }
    }
    glutPostRedisplay();
    
    // Draw ground mesh
    DrawMeshQM(&groundMesh, meshSize);
    
    
    glutSwapBuffers();   // Double buffering, swap buffers
}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
    // Set up viewport, projection, then change to modelview matrix mode -
    // display function will then set up camera and do modeling transforms.
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
    //'s' key here toggles the rotors, 'f' & 'b' move it forward and backward based on the Z axis.
    switch (key)
    {
        case 27:
            exit(0);
            break;
        case 't':
            break;
        case 's':
            if(rotor == 0) {
                rotor = 1;
            } else {
                rotor = 0;
            }
            break;
        case 'f':
            droneX = droneX - 0.2 * sinf(droneAngle * 3.14159265359 / 180);
            droneZ = droneZ - 0.2 * cosf(droneAngle * 3.14159265359 / 180);
            break;
        case 'b':
            droneX = droneX + 0.2 * sinf(droneAngle * 3.14159265359/180);
            droneZ = droneZ + 0.2 * cosf(droneAngle * 3.14159265359/180);
            break;
        case 'c':
            missleX = 0;
            missleZ = 0;
            shootTrig = 1;
            break;
        case 'h':
            printf("Drone Simulator Instructions: \n");
            printf("Press f & b to move drone forward and back \n");
            printf("Up and Down arrow keys change elevation \n");
            printf("Left and Right arrow keys rotate the drone \n");
            printf("c key shoots missles \n");
            printf("l key makes it third person, p key makes it first person \n");
            printf("i and o tilt the first person camera, y and t zoom in/out \n");
    }
    
    glutPostRedisplay();   // Trigger a window redisplay
}

void keyboardUp(unsigned char key, int x, int y)
{
    //'s' key here toggles the rotors, 'f' & 'b' move it forward and backward based on the Z axis.
    switch (key)
    {
        case 'p':
            first_person = !first_person;
            break;
        case 'l':
            third_person = !third_person;
            break;
        case 'i':
            droneTilt = droneTilt - 0.1;
            break;
        case 'o':
            droneTilt = droneTilt + 0.1;
            break;
        case 'y':
    //Zooms in
            if(zoomCounters < 15) {
                droneZoomX = droneZoomX - 0.1 * sinf(droneAngle * 3.14159265359/180);
                droneZoomZ = droneZoomZ - 0.1 * cosf(droneAngle * 3.14159265359/180);
                zoomCounters++;
            }
            break;
        case 't':
    //Zooms out
            if (zoomCounters != 0) {
                droneZoomX = droneZoomX + 0.1 * sinf(droneAngle * 3.14159265359/180);
                droneZoomZ = droneZoomZ + 0.1 * cosf(droneAngle * 3.14159265359/180);
                zoomCounters--;
            }
            break;
    }
    glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
    // Help key
    if (key == GLUT_KEY_F1)
    {
       
    }
    // Do transformations with arrow keys
    //else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
    //{
    //}
    //GLUT_UP and GLUT_DOWN move the drone up and down on the y-axis, while LEFT and RIGHT change the rotation
    if (key == GLUT_KEY_UP) {
        droneY = droneY + 0.2;
    } else if (key == GLUT_KEY_DOWN) {
        droneY = droneY - 0.2;
       
    } else if (key == GLUT_KEY_RIGHT) {
        droneAngle = droneAngle - 20;
    } else if (key == GLUT_KEY_LEFT) {
        droneAngle = droneAngle + 20;
    }
    glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to
void mouse(int button, int state, int x, int y)
{
    currentButton = button;
    
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                
                
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                
            }
            break;
        default:
            break;
    }
    
    glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to
void mouseMotionHandler(int xMouse, int yMouse)
{
    if (currentButton == GLUT_LEFT_BUTTON)
    {
       
    }
    
    glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}






void drawCity(unsigned int texture) {
   
    
    //Lets just make a...city of sorts..
    for (int i = 1; i < 3; i++)
    {
        glPushMatrix();
        building = newCube();
        glScalef(1, 3, 1);
        glTranslatef(3 +(-i * i),1,2*i);
        drawCube(&building, texture);
        glPopMatrix();
    }
    
    for(int i = -2; i< 3; i++)
    {
        glPushMatrix();
        building = newCube();
        glScalef(2, 1, 1);
        glTranslatef(-3 * i ,0,1 * i);
        drawCube(&building, texture);
        glPopMatrix();
    }
    
    glPushMatrix();
    building = newCube();
    glScalef(2, 1, 1);
    glTranslatef(-3,0,0);
    drawCube(&building, texture);
    glPopMatrix();
    
    glPushMatrix();
    building = newCube();
    glTranslatef(-8,1,-6);
    drawCube(&building, texture);
    glPopMatrix();
    
    glPushMatrix();
    building = newCube();
    glTranslatef(-1,1,-9);
    glScalef(3, 1, 3);
    drawCube(&building, texture);
    glPopMatrix();
    
    glPushMatrix();
    building = newCube();
    glTranslatef(3,0,2);
    drawCube(&building, texture);
    glPopMatrix();
    
    glPushMatrix();
    building = newCube();
    glScalef(1, 0.5, 1);
    glTranslatef(-3,0.5,8);
    drawCube(&building, texture);
    glPopMatrix();
    
    glPushMatrix();
    building = newCube();
    glTranslatef(8,0,5);
    drawCube(&building, texture);
    glPopMatrix();
    
    glPushMatrix();
    building = newCube();
    glTranslatef(3,0,2);
    drawCube(&building, texture);
    glPopMatrix();
    
    
}
////This draws the player drone
void drawDrone(unsigned int texture) {

    checkCollision();
    //Bottom Body
    glPushMatrix();
    CubeMesh playerDrone = newCube();
    glTranslatef(droneX,droneY,droneZ);
    glRotatef(droneAngle, 0, 1.0, 0);

        //Left Propellor
        glPushMatrix();
        CubeMesh droneLeftProp = newCube();
        glTranslatef(-0.275, 0.0, 0.0);
        if (rotor) {
            glRotatef(spin, 0, 1.0, 0);
        }
        glScalef(0.01, 0.02, 0.15);
        drawCube(&droneLeftProp,texture);
        glPopMatrix();

        //Right Propellor
        glPushMatrix();
        CubeMesh droneRightProp = newCube();
        glTranslatef(0.275, 0.0, 0.0);
        if (rotor == 1) {
            glRotatef(spin, 0, 1.0, 0);
        }
        glScalef(0.01, 0.02, 0.15);
        drawCube(&droneRightProp,texture);
        glPopMatrix();

        //Crossface to make the drone look cooler
        glPushMatrix();
        CubeMesh droneCrossFace = newCube();
        glTranslatef(0.0, 0.0, -0.275);
        glScalef(0.02,0.05,0.5);
        drawCube(&droneCrossFace,texture);
        glPopMatrix();
        //Rear bit
        glPushMatrix();
        CubeMesh rearTail = newCube();
        glTranslatef(0.0,0.0,0.275);
        glScalef(0.02,0.1,0.02);
        drawCube(&rearTail,texture);
        glPopMatrix();

        //top Propellor
        glPushMatrix();
        CubeMesh droneTopProp = newCube();
        glTranslatef(0.0, 0.1, 0.275);
        if (rotor == 1) {
            glRotatef(spin, 0, 1.0, 0);
        }
        glScalef(0.01, 0.02, 0.15);
        drawCube(&droneTopProp,texture);
        glPopMatrix();

        glPushMatrix();
        CubeMesh droneBottomProp = newCube();
        glTranslatef(0.0, -0.1, 0.275);
        if (rotor == 1) {
            glRotatef(spin, 0, 1.0, 0);
        }
        glScalef(0.01, 0.02, 0.15);
        drawCube(&droneBottomProp,texture);
        glPopMatrix();

        glPushMatrix();
        CubeMesh droneWing2 = newCube();
        glTranslatef(0.0,0.0,-0.325);
        glScalef(0.30, 0.02, 0.01);
        drawCube(&droneWing2,texture);
        glPopMatrix();

        //Front Right Propellor
        glPushMatrix();
        CubeMesh droneFrontRightProp = newCube();
        glTranslatef(0.275, 0.0, -0.325);
        if (rotor == 1) {
            glRotatef(spin, 0, 1.0, 0);
        }
        glScalef(0.01, 0.02, 0.15);
        drawCube(&droneFrontRightProp,texture);
        glPopMatrix();

        //Right Propellor
        glPushMatrix();
        CubeMesh droneFrontLeftProp = newCube();
        glTranslatef(-0.275, 0.0, -0.325);
        if (rotor == 1) {
            glRotatef(spin, 0, 1.0, 0);
        }
        glScalef(0.01, 0.02, 0.15);
        drawCube(&droneFrontLeftProp,texture);
        glPopMatrix();

    glScalef(0.09, 0.01, 0.50);
    drawCube(&playerDrone,texture);
    glPopMatrix();

    //The main wing of the drone
    glPushMatrix();
    CubeMesh droneWing1 = newCube();
    glTranslatef(droneX,droneY,droneZ);
    glRotatef(droneAngle, 0, 1.0, 0);
    glScalef(0.30, 0.03, 0.01);
    drawCube(&droneWing1,texture);
    glPopMatrix();

}
////This draws the enemy drone
void drawEnemyDrone(unsigned int texture) {
    enemyAI();
    //Bottom Body
    glPushMatrix();
    CubeMesh enemyDrone = newCube();
    glTranslatef(enemyX,enemyY,enemyZ);
    glRotatef(enemyAngle, 0, 1.0, 0);

        //Left Propellor
        glPushMatrix();
        CubeMesh enemyLeftProp = newCube();
        glTranslatef(-0.275, 0.0, 0.0);
        glRotatef(spin, 0, 1.0, 0);
        glScalef(0.01, 0.02, 0.15);
        drawCube(&enemyLeftProp,texture);
        glPopMatrix();

        //Right Propellor
        glPushMatrix();
        CubeMesh enemyRightProp = newCube();
        glTranslatef(0.275, 0.0, 0.0);
        glRotatef(spin, 0, 1.0, 0);
        glScalef(0.01, 0.02, 0.15);
        drawCube(&enemyRightProp,texture);
        glPopMatrix();

        //Crossface to make the drone look cooler
        glPushMatrix();
        CubeMesh enemyCrossFace = newCube();
        glTranslatef(0.0, 0.0, -0.275);
        glScalef(0.02,0.05,0.5);
        drawCube(&enemyCrossFace,texture);
        glPopMatrix();
        //Rear bit
        glPushMatrix();
        CubeMesh enemyRearTail = newCube();
        glTranslatef(0.0,0.0,0.275);
        glScalef(0.02,0.1,0.02);
        drawCube(&enemyRearTail,texture);
        glPopMatrix();

        //top Propellor
        glPushMatrix();
        CubeMesh enemyTopProp = newCube();
        glTranslatef(0.0, 0.1, 0.275);
        glRotatef(spin, 0, 1.0, 0);
        glScalef(0.01, 0.02, 0.15);
        drawCube(&enemyTopProp,texture);
        glPopMatrix();

        glPushMatrix();
        CubeMesh enemyBottomProp = newCube();
        glTranslatef(0.0, -0.1, 0.275);
        glRotatef(spin, 0, 1.0, 0);
        glScalef(0.01, 0.02, 0.15);
        drawCube(&enemyBottomProp,texture);
        glPopMatrix();

    glScalef(0.09, 0.01, 0.50);
    drawCube(&enemyDrone,texture);
    glPopMatrix();

    //The main wing of the drone
    glPushMatrix();
    CubeMesh enemyWing1 = newCube();
    glTranslatef(enemyX,enemyY,enemyZ);
    glRotatef(enemyAngle, 0, 1.0, 0);
    glScalef(0.30, 0.02, 0.01);
    drawCube(&enemyWing1,texture);
    glPopMatrix();


}
//


void drawRoads(unsigned int texture)
{
    //This a road
    
    glPushMatrix();
    building = newCube();
    glScalef(1,0.01,15);
    glRotatef(90, 0.0, 0.0, 1.0);
    drawCube(&building, texture);
    glPopMatrix();
    
    //This a road too
    glPushMatrix();
    building = newCube();
    glScalef(15,0.01,1);
    glRotatef(90, 0, 0, 1.0);
    drawCube(&building, texture);
    glPopMatrix();
    for(int i = 0; i<27; i+=3)
        {
            //This a road too
            glPushMatrix();
            building = newCube();
            glScalef(15,0.01,1);
            glTranslatef(-1, 0, -8 + i);
            glRotatef(90, 0, 0, 1.0);
            drawCube(&building, texture);
            glPopMatrix();
        }
}

//This is the function from one of the examples, changed to spin the propellers
void spinProps(void)
{
    
    spin+= 20;
    if (spin > 360.0) {
        spin -= 360.0;
    }
}

void projectileMovement(void)
{
    missleX = missleX - 0.01 * sinf(droneAngle * 3.14159265359 / 180);
    missleZ = missleZ - 0.01 * cosf(droneAngle * 3.14159265359 / 180);

}

void shootProjectile(unsigned int texture)
{

    projectileMovement();
    glPushMatrix();
  //  CubeMesh projectile = newCube();
    glTranslatef(droneX + missleX, droneY, droneZ + missleZ);
    glScalef(0.01, 0.01, 0.02);
    drawCube(&bullet,NULL);
    glPopMatrix();

}

void enemyAI(void)
{
    if (posXThresh == 0) {
        enemyAngle = -90;
        enemyX = enemyX + 0.03;
    }
   
    if(enemyX > 6 && posXThresh == 0){
        posXThresh = 1;
    }
    if(posXThresh == 1 && posZThresh == 0) {
        enemyAngle = 180;
        enemyZ = enemyZ + 0.03;
    }
    if (enemyZ > 7 && posZThresh == 0){
        posZThresh = 1;
        negXThresh = 0;
    }
    if (posXThresh == 1 && posZThresh == 1 && negXThresh == 0) {
        enemyAngle = 90;
        enemyX = enemyX - 0.03;
    }
    if (enemyX < -6) {
        negXThresh = 1;
    }
    if (negXThresh == 1 && posZThresh == 1) {
        enemyAngle = 0;
        enemyZ = enemyZ - 0.03;
    }
    if (enemyZ < -7) {
        posZThresh = 0;
        posXThresh = 0;
    }
    
    if(posYThresh == 0) {
        enemyY = enemyY + 0.03;
    }
    if (enemyY > 4.5) {
        posYThresh = 1;
    }
    
    if(posYThresh == 1){
        enemyY = enemyY - 0.03;
    }
    
    if (enemyY < 3) {
        posYThresh = 0;
    }
}

void drawSky(unsigned int texture)
{
    glPushMatrix();
    building = newCube();
    glScalef(300,300,300);
    drawCube(&building, texture);
    glPopMatrix();
}

void checkCollision(void)
{
    if(droneY < 0){
        droneCollision = true;
    }
    
    
}
