#include <stdio.h>           // Standard C/C++ Input-Output
#include <math.h>            // Math Functions
#include <windows.h>         // Standard Header For MSWindows Applications
#include <gl/glut.h>            // The GL Utility Toolkit (GLUT) Header
#include <time.h>
#include <ctime>



// The Following Directive Fixes The Problem With Extra Console Window
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

// Uncomment the following if you want to use the glut library from the current directory
//#pragma comment(lib, "lib/glut32.lib")

// Global Variables
bool g_gamemode;				// GLUT GameMode ON/OFF
bool g_fullscreen;				// Fullscreen Mode ON/OFF (When g_gamemode Is OFF)
bool b_culling = false;			// Culling Enabled is Mandatory for this assignment do not change
float aspect = 1;

const float PI = 3.1415926535897932384626433832795028;
const float epsilon = 0.001;
float p1_x = -35;		// starting_position of plane 1;
float p2_x = -35;		// starting_position of plane 2;
float s_x = 25;			// starting position of sun x-axis
float s_y = 27;			// starting position of sun y-axis
int rotation = 0;		// rotation counter for the sun
float speed1 = rand() % 100 / 100.0;
//float speed2 = rand() % 100 / 100.0;
float speed2 = 0;
float acceleration = 1;

int camera_y_state = 0;
float camera_y = 1;
float mouse_y = 0;
float camera_zoom = 00;		//prev was 90 GP
float mouse_scroll = 0;
int player[2] = { 1,1 };

const int arraysize = 11;

bool lilmaze[arraysize][arraysize] = {
	{1,1,1,1,1,0,1,1,1,1,1},//1
	{1,0,0,0,0,0,0,0,0,0,0},
	{1,0,1,1,1,1,1,1,0,1,1},
	{1,0,0,0,0,0,0,1,0,0,0 },
	{ 1,1,1,1,0,1,1,1,1,0,1 },
	{ 0,0,0,0,0,1,0,0,0,0,0 },//6
	{ 1,0,1,0,1,1,1,1,0,1,1 },
	{ 1,0,1,0,0,0,1,0,0,0,0 },
	{ 1,0,1,1,1,0,0,0,1,1,1 },
	{ 1,0,0,0,1,0,1,0,0,0,0 },
	{ 1,0,1,0,1,0,1,0,1,0,1 },//11
};

bool maze[arraysize*arraysize][arraysize*arraysize];


//makes maze by placing small mazes together
void makemaze() {
	int side = 0;
	side = rand() % 4 + 1;		//dont need it for now

	for (int i = 0; i < arraysize*arraysize; i++) {
		for (int j = 0; j < arraysize*arraysize; j++) {

			maze[i][j] = lilmaze[i % arraysize][j % arraysize];
			
			if ((i == 0)||(i == arraysize*arraysize)) {
				maze[i][j] = 1;
			}
			if ((j == 0) || (j == arraysize*arraysize)) {
				maze[i][j] = 1;
			}
			maze[6][0] = 0;												//start
			maze[arraysize*arraysize - 5][arraysize*arraysize] = 0;		//finish
		}
	}
}

GLfloat sun_light_ambient_diffuse[] = { 1.0, 1.0, 0.0, 0.5 };
GLfloat sun_light_specular[] = { 1.0, 1.0, 0.0, 0.5 };

//Function Prototypes
void render(void);
void initLights(void);
bool init(void);
void reshape(int w,int h);
void keyboard(unsigned char key, int x, int y);
void special_keys(int a_keys, int x, int y);
void drawAxes();
void create_square(float x,float y, float size, int v);
void create_player();

void initLights(void) {

	//glEnable(GL_LIGHT0);							   // Quick And Dirty Lighting (Assumes Light0 Is Set Up)
	
	
	//glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);

	// sun
	glLightfv(GL_LIGHT7, GL_AMBIENT_AND_DIFFUSE, sun_light_ambient_diffuse);
	glLightfv(GL_LIGHT7, GL_SPECULAR, sun_light_specular);
	
	glEnable(GL_LIGHT1);		// plane 1
	glEnable(GL_LIGHT2);		// plane 1
	glEnable(GL_LIGHT3);		// plane 2
	glEnable(GL_LIGHT4);		// plane 2

	glEnable(GL_LIGHT7);		// sun
	glEnable(GL_LIGHTING);							   // Enable Lighting
}


// Our GL Specific Initializations. Returns true On Success, false On Fail.
bool init(void)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);             // Pixel Storage Mode To Byte Alignment
	glEnable(GL_TEXTURE_2D);                           // Enable Texture Mapping 
    glClearColor(0.0f, 1.0f, 1.0f, 0.5f);			   // Black Background (CHANGED)
    glClearDepth(1.0f);								   // Depth Buffer Setup
    glDepthFunc(GL_LEQUAL);							   // The Type Of Depth Testing To Do
    glEnable(GL_DEPTH_TEST);						   // Enables Depth Testing
    glShadeModel(GL_SMOOTH);						   // Enable Smooth Shading
	initLights();
	glEnable(GL_COLOR_MATERIAL);					   // Enable Material Coloring
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Hint for nice perspective interpolation
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	// Set the color tracking for both faces for both the ambient and diffuse components
	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);                               //Counter Clock Wise definition of the front and back side of faces
	glCullFace(GL_BACK);                               //Hide the back side
	
	return true;
}

void positionCamera(){

	glMatrixMode(GL_PROJECTION);     // Select The Projection Matrix

	glLoadIdentity();                // Reset The Projection Matrix
	gluPerspective(60.0f, aspect, 10.1, 600.0);  // was 90.0f x		was 45 now 60 GP
	gluLookAt(0.0f, 0.0f, 600.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f);
	
	//camera transformations go here
	
	glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix

}

// Our Reshaping Handler (Required Even In Fullscreen-Only Modes)
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	// Calculate The Aspect Ratio And Set The Clipping Volume
	if (h == 0) h = 1;
	aspect = (float)w/(float)h;
	positionCamera();
	glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix
	initLights();
}

// Our Keyboard Handler (Normal Keys)
void keyboard(unsigned char key, int x, int y)
{
	//printf("%c", key);
	switch (key) {
		case 'w':
			break;
		case 's':
			break;

		case 32:        // When space Is Pressed...
			
			break;      // Ready For Next Case

		case 27:        // When Escape Is Pressed...
			exit(0);    // Exit The Program
			break;      // Ready For Next Case
		default:
		break;
	}
	glutPostRedisplay();
}

// Our Keyboard Handler For Special Keys (Like Arrow Keys And Function Keys)
void special_keys(int a_keys, int x, int y)
{
	//printf("key: %d x: %d y: %d", a_keys, x, y);
	//printf("key: %d", maze[player[0]][player[1] + 1]);
	switch (a_keys) {
		case GLUT_KEY_F1:
			// We Can Switch Between Windowed Mode And Fullscreen Mode Only
			if (!g_gamemode) {
				g_fullscreen = !g_fullscreen;       // Toggle g_fullscreen Flag
				if (g_fullscreen) glutFullScreen(); // We Went In Fullscreen Mode
				else {
					glutPositionWindow(400, 50);
					glutReshapeWindow(1000, 1000);   // We Went In Windowed Mode
				}
			}
		break;
		case GLUT_KEY_F2:
			
			break;
		case GLUT_KEY_UP:
			if (maze[player[0] - 1][player[1]] != 1) {
				player[0]--;
			}
			break;
		case GLUT_KEY_DOWN:
			if (maze[player[0] + 1][player[1]] != 1) {
				player[0]++;
			}
			break;
		case GLUT_KEY_LEFT:
			if (maze[player[0]][player[1] - 1] != 1) {
				player[1]--;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (maze[player[0]][player[1] + 1] != 1) {
				player[1]++;
			}
			break;
		default:
			;
	}
	printf("player: %d | %d\n", player[0], player[1]);

	glutPostRedisplay();
}

void create_square(float x, float y, float size, int v) {
	if (v == 1) { // wall
		glColor3f(100.0f, 0.0f, 0.0f);
	}
	else { // walking space
		glColor3f(0.0f, 100.0f, 0.0f);
	}
	glPushMatrix();
		glTranslatef(x, y, -1);
		glutSolidCube(size);
	glPopMatrix();
}

void create_player(float size,float sq_size,float s_x, float s_y) {
	float x = s_x + sq_size * player[1];
	float y = s_y - sq_size * player[0];
	glColor3f(0.0f, 0.0f, 0.0f);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glutSolidSphere(size, 50, 50);
	glPopMatrix();
}

// Main Function For Bringing It All Together.
int main(int argc, char** argv){
	glutInit(&argc, argv);								// GLUT Initializtion
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE); // (CHANGED)|
	makemaze();

	if (g_gamemode) {
		glutGameModeString("1024x768:32");				// Select 1024x768 In 32bpp Mode
		if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
			glutEnterGameMode();						// Enter Full Screen
		else
			g_gamemode = false;							// Cannot Enter Game Mode, Switch To Windowed
	}
	if (!g_gamemode) {
		glutInitWindowPosition(400, 50);
		glutInitWindowSize(1000, 1000);					// Window Size If We Start In Windowed Mode
		glutCreateWindow("EPL426 – Project");		// Window Title 
	}
	if (!init()) {										// Our Initialization
		fprintf(stderr,"Initialization failed.");
		return -1;
	}
	
	glutDisplayFunc(render);							// Register The Display Function
	glutReshapeFunc(reshape);							// Register The Reshape Handler
	glutKeyboardFunc(keyboard);							// Register The Keyboard Handler
	glutSpecialFunc(special_keys);						// Register Special Keys Handler
	glutIdleFunc(NULL);                        			// We Do Rendering In Idle Time
	glutMainLoop();										// Go To GLUT Main Loop
	return 0;
}


// Our Rendering Is Done Here
int timer = 0;
void render(void)   
{
	srand(time(NULL));
	//timer++;
	//printf("timer: %d | %d\n", time,timer);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

	// Do we have culling enabled?
	if (b_culling == true)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	float s_x = -300.0;	// starting x position of maze bottom-left
	float s_y = 300.0;	// starting y position of maze bottom-left
	float size = 5.0;		// size of each cube of maze;
	float p_size = 2;		// size of player;

	// Big Black Cube for background
	/*glPushMatrix();
		glColor3f(0.0f, 0.0f, 0.0f);
		glTranslatef(0, 0, -52);
		glutSolidCube(100);
	glPopMatrix();*/

	for (int i = 0; i < arraysize*arraysize; i++) {
		for (int j = 0; j < arraysize*arraysize; j++) {
			float sq_x = s_x + size * j;	// this square x position
			float sq_y = s_y - size * i;	// this square y position
			create_square(sq_x, sq_y, size, maze[i][j]);
		}
	}
	create_player(p_size,size,s_x,s_y);

	glLoadIdentity();

    // Swap The Buffers To Make Our Rendering Visible
    glutSwapBuffers();
	glutPostRedisplay(); //animation
}