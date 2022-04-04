/*******************************************************************
           Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"

const int meshSize = 16;    // Default Mesh Size
const int vWidth = 650;     // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

const double droneXSCALE = 9.0;
const double droneYSCALE = 1.0;
const double droneZSCALE = 2.5;

double droneXPOS = 1.0;
double droneYPOS = 1.5;
double droneZPOS = 8.0;

double droneTHETA = 90.0;
double droneXROTATE = 0.0;
double droneYROTATE = 1.0;
double droneZROTATE = 0.0;

double propRotate1 = 45.0;

//static GLfloat spin = 0.0;

static GLfloat theta1 = 0.0;

static int currentButton;

static unsigned char currentKey;

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
void drawDrone();
void drawBody();
void drawArm1();
void drawArm2();
void drawArm3();
void drawArm4();
void spinDisplay(void);
void drawStaticObjects();

Vector3D ScreenToWorld(int x, int y);


int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("Assignment 1");

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
    glEnable(GL_LIGHT1);   //This light is currently on
	
    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.0F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-8.0f, 0.0f, 8.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 16.0, 16.0, dir1v, dir2v);

    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0,  8.0);
}



void drawBody()
{	
	
	//Pushes the current transformation matric (CTM) on the stack. (Though the current one is empty).
	glPushMatrix();

		//T1: Translate so the body of the drone is near the middle of the screen
		// Gloabal variables were used as these needed to be updated for the moving functions.
		// Done last to the body since these are in reverse order.
		// CTM = T1.
		glTranslatef(droneXPOS, droneYPOS, droneZPOS);
		//R1: Rotate the body so it's 90 degrees on the screen.
		// Again use of global variables as these will be updated later on
		// CTM = T1 * R1.
		glRotatef(droneTHETA, droneXROTATE, droneYROTATE, droneZROTATE);
		//S1: Scale the body so it's large and easy to view.
		// CTM = T1* R1 * S1
		glScalef(droneXSCALE, droneYSCALE, droneZSCALE);
		// Now draw the body.
		glutSolidCube(1.0);	
		
		//INVERSE The scaling from before to the CTM.
		// CTM = T1 * R1 * (S1 * 1/S1).
		// (droneXSCALE * (1 / droneXSCALE) = 1). 
		// CTM = T1 * R1.
		glScalef(1 / droneXSCALE, 1 / droneYSCALE, 1 / droneZSCALE);
		//Inverse the rotation
		// CTM = T1.
		glRotatef(-45.0, 0.0, 1.0, 0.0);
		// Finally inverse the translation.
		glTranslatef(-1.0, -3.5, -8.0);
		//Transforms for obj2
		//Now we call the other methods to draw the rest of the object.
		//This sets up the object in a hierarchy, so that transformations done to the main body, will
		//affect the entire drone, since the arms will be children of their parent node (the body), and 
		//all parents transformations affect the children as well.
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
	//Start a new CTM since the last one had it's transformations inversed and popped off the stack.
	//CTM = T1
		glTranslatef(3.0, 3.50, 8.0);
		//CTM = T1 * R1
		glRotatef(90.0, 0.0, 1.0, 0.0);
		//CTM = T1 * R1 * S1
		glScalef(0.6, 0.3, 3.0);
		//Draw the arm
		glutSolidCube(2.0);
			
			glPushMatrix();
			//Now we push a SECOND matrix on the stack
			//Inverse the FIRST matrix so we can add another object easily
			//Transformations here are done in an inverse order of normal transformations.
			//CTM = T1 * (S1 * 1/S1) * R1.
			glScalef(1 / 0.6, 1 / 0.3, 1 / 3.0);
			//CTM = T1 * R1
			glRotatef(-90.0, 0.0, 1.0, 0.0);
			//CTM = T1 
			glTranslatef(-3.0, -3.50, -8.0);
			//CTM Is empty
			//CTM = T1
			glTranslatef(6.0, 3.5, 8.0);
			//CTM = T1 * R1
			glRotatef(-90.0, 1.0, 0.0, 0.0);
			//CTM = T1 * R1 * S1
			glScalef(1.0, 1.5, 2.0);
			//Draw the end piece of the arm
			glutSolidCube(1.0);
				//Push the next matrix on the stack
				glPushMatrix();
					//inverse the previous transformations again
					glScalef(1/1.0, 1/1.5, 1/2.0);
					glTranslatef(-6.0, -3.5, -8.0);
					//CTM = T1
					glTranslatef(6.0, 3.5, 9.2);	
					//CTM = T1 * R1
					glRotatef(theta1, 0.0, 0.0, 1.0);
					//CTM = T1 * R1 * R2
					//Second rotation was needed here to re-allign the propeller to stay flat while turning
					glRotatef(90.0, 0.0, 1.0, 0.0);
					//CTM = T1 * R1 * R2 * S1
					glScalef(0.2, 0.3, 3.0);
					//Draw the propeller
					glutSolidCube(1.0);
				
	
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

	glTranslatef(1.0, 3.50, 10.0);
	glRotatef(15.0, 0.0, 1.0, 0.0);
	glScalef(0.6, 0.3, 3.0);
	glutSolidCube(2.0);

			glPushMatrix();
			//Inverse so we can add another object
			glScalef(1 / 0.6, 1 / 0.3, 1 / 3.0);
			glRotatef(-15.0, 0.0, 1.0, 0.0);
			glTranslatef(-1.0, -3.50, -10.0);
			
			glTranslatef(1.8, 3.5, 13.0);
			glRotatef(90.0, 1.0, 0.0, 0.0);
			glRotatef(73.0, 0.0, 0.0, 1.0);
			glScalef(1.0, 1.5, 2.0);
			glutSolidCube(1.0);

				glPushMatrix();

					glScalef(1/1.0, 1/1.5, 1/2.0);
					glTranslatef(-1.8, -3.5, -13.0);
					
					glTranslatef(1.8, 3.5, 11.8);
					glRotatef(theta1, 0.0, 0.0, 1.0);
					glRotatef(90.0, 0.0, 1.0, 0.0);
					glScalef(0.2, 0.3, 3.0);
					glutSolidCube(1.0);

				glPopMatrix();

			glPopMatrix();

	glPopMatrix();
}

void drawArm3()
{
	glPushMatrix();
	//Inverse x translation as I knew this arm needed to be the same as arm1 just reflected over the x axis.
	glTranslatef(-1.0, 3.50, 8.0);
	glRotatef(-90.0, 0.0, -1.0, 0.0);
	glScalef(0.6, 0.3, 3.0);
	glutSolidCube(2.0);

			glPushMatrix();
			//Inverse so we can add another object
			glScalef(1 / 0.6, 1 / 0.3, 1 / 3.0);
			glRotatef(-15.0, 0.0, 1.0, 0.0);
			glTranslatef(-1.0, -3.50, -10.0);
			
			glTranslatef(0.2, 3.5, 7.0);
			glRotatef(90.0, 1.0, 0.0, 0.0);
			glRotatef(73.0, 0.0, 0.0, 1.0);
			glScalef(1.0, 1.5, 2.0);
			glutSolidCube(1.0);

				glPushMatrix();

					glScalef(1/1.0, 1/1.5, 1/2.0);
					glTranslatef(-0.2, -3.5, -7.0);
					
					glTranslatef(0.2, 3.5, 5.8);
					glRotatef(theta1, 0.0, 0.0, 1.0);
					glRotatef(90.0, 0.0, 1.0, 0.0);
					glScalef(0.2, 0.3, 3.0);
					glutSolidCube(1.0);

				glPopMatrix();
	
			glPopMatrix();	
		
	glPopMatrix();
}

void drawArm4()
{
	glPushMatrix();
	//Inverse x translation here again as this is essentially a flipped image of arm2.
	glTranslatef(0.0, 3.50, 6.0);
	glRotatef(-15.0, 0.0, -1.0, 0.0);
	glScalef(0.6, 0.3, 3.0);
	glutSolidCube(2.0);

			glPushMatrix();
			//Inverse so we can add another object
			glScalef(1 / 0.6, 1 / 0.3, 1 / 3.0);
			glRotatef(-15.0, 0.0, 1.0, 0.0);
			glTranslatef(-1.0, -3.50, -10.0);
			
			glTranslatef(0.2, 3.5, 7.0);
			glRotatef(90.0, 1.0, 0.0, 0.0);
			glRotatef(73.0, 0.0, 0.0, 1.0);
			glScalef(1.0, 1.5, 2.0);
			glutSolidCube(1.0);

				glPushMatrix();

					glScalef(1 / 1.0, 1 / 1.5, 1 / 2.0);
					glTranslatef(-0.2, -3.5, -7.0);
					glTranslatef(0.2, 3.5, 5.8);
					glRotatef(theta1, 0.0, 0.0, 1.0);
					glRotatef(90.0, 0.0, 1.0, 0.0);
					glScalef(0.2, 0.3, 3.0);
					glutSolidCube(1.0);

				glPopMatrix();

		glPopMatrix();

glPopMatrix();
}

void drawStaticObjects()
{
	glPushMatrix();
	glTranslatef(0.0, 2.0, 0.0);
	//glRotatef(-15.0, 0.0, -1.0, 0.0);
	//glScalef(3.0, 3.3, 3.0);
	glutSolidSphere(1.0, 50.0, 50.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.0, 2.3, 0.0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glScalef(1.0, 1.0, 1.0);
	glutSolidTorus(0.5, 1.0, 50.0, 50.0);
	glPopMatrix();

	
}

void drawDrone()
{
	//Method to draw the scene, first draw the body (which then calls the other draw methods for the drone)
	//then the static objects in the scene.
	drawBody();
	drawStaticObjects();

}

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw Drone

	// Set drone material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

	// Apply transformations to move drone
	// ...

	// Apply transformations to construct drone, modify this!
	drawDrone();


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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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
		//NOTE: I interpreted forward as going AWAY from the screen and backwards as coming towards the screen
		//as my object is already facing away from the screen when it is drawn.

		//I did a bit of research online in some gaming forums and found how to handle an object having
		//a new forward direction after it's rotated.
		//I found that sin(object's angle) handled that, althought I had to add 90 degrees as it was offset at first,
		//and I had to multiple it by pi/180 to convert it to radians, as openGL handles it in radians.
		droneXPOS += 1.0 * sinf((droneTHETA + 90.0f) * 3.14159 / 180);
		droneZPOS += 1.0 * cosf((droneTHETA + 90.0f) * 3.14159 / 180);
		glutIdleFunc(spinDisplay);
		break;
	case 'B':
	case 'b':
		//Same thing for going backwards, just flip the sign.
		droneXPOS -= 1.0 * sinf((droneTHETA + 90.0f) * 3.14159 / 180);
		droneZPOS -= 1.0 * cosf((droneTHETA + 90.0f) * 3.14159 / 180);
		glutIdleFunc(spinDisplay);
		break;
	case 'Z':
	case 'z':
		droneTHETA += 3.5;
		break;
	case 'X':
	case 'x':
		droneTHETA += -3.5;
		break;
	case 'S':
	case 's':
		glutIdleFunc(spinDisplay);
		break;


	}
    glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
    // Help menu
    if (key == GLUT_KEY_F1)
    {
		printf("HELP MENU\n f = move forward\n b = move backwards\n s = start propeller spin\n z = turn left\n x = turn right\n and the arrow keys to control movement. ");
    }
    // Do transformations with arrow keys
    //else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
    //{
    //}
	else if (key == GLUT_KEY_UP)
	{
		//Drone flies up
		droneYPOS += 0.5;
	}

	else if (key == GLUT_KEY_DOWN)
	{
		//Drone flies down
		droneYPOS += -0.5;
	}

	else if (key == GLUT_KEY_RIGHT)
	{
		//Drone flies right
		droneXPOS += 0.5;
	}

	else if (key == GLUT_KEY_LEFT)
	{
		//Drone flies left
		droneXPOS += -0.5;
	}

    glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	//Also optional, clicking the mouse also starts the spinning.
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(spinDisplay);
		break;
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(NULL);
		break;
	default:
		break;
	}
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
    if (currentButton == GLUT_LEFT_BUTTON)
    {
        ;
    }

    glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}

void spinDisplay(void)
{
	//Callnback function for spinning the propeller theta degrees.
	//Since the angle in openGL exceeds 360, we must re set it to 0 everytime it reaches 360.
	theta1 += 1.2;
	if (theta1 > 360.0)
		theta1 -= 360.0;

	glutPostRedisplay();
}

