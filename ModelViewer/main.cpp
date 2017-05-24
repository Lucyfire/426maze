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

const int gametime = 120;	// in seconds
const int arraysize = 11;	// initial maze size
const int mazesize = arraysize * arraysize;	// maze size
/*
int player[2] = { 1,1 };	// player starting position
int trophy[2] = { mazesize - 2,mazesize - 2 };

*/
int player[2] =  { 6,0 };	// player starting position
int trophy[2] ={ mazesize - 6, mazesize - 1 };

bool game_status = 1;		// 1 is playing, 0 is gameover
int game_elapsed_time = 0;

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

bool maze[mazesize][mazesize];
//makes maze by placing small mazes together
void makemaze() {
	int side = 0;

	for (int i = 0; i < mazesize; i++) {
		for (int j = 0; j < mazesize; j++) {
			side = rand() % 4 + 1;

			if (side == 1) {
				//	maze[i][j] = lilmaze[j % arraysize][i % arraysize];
				maze[i][j] = lilmaze[i % arraysize][j % arraysize];
			}
			else if (side == 2) {
				maze[i][j] = lilmaze[i % arraysize][j % arraysize];
			}
			else if (side == 3) {
				maze[i][j] = lilmaze[(mazesize)-(i % arraysize)][(mazesize)-(j % arraysize)];
			}
			else if (side == 4) {
				maze[i][j] = lilmaze[i % arraysize][j % arraysize];
			}
			//maze[i][j] = lilmaze[i % arraysize][j % arraysize];

			if ((i == 0) || (i == mazesize)) {
				maze[i][j] = 1;
			}
			if ((j == 0) || (j == mazesize)) {
				maze[i][j] = 1;
			}
		}	
	}
	// make all outer edges walls
	for (int i = 0; i < mazesize; i++) {
		maze[0][i] = 1;
		maze[i][0] = 1;
		maze[mazesize - 1][i] = 1;
		maze[i][mazesize - 1] = 1;
	}
	maze[6][0] = 0;										//start
	maze[mazesize - 6][mazesize - 1] = 0;				//finish
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
void initGame();
void create_square(float x,float y, float size, int v);
void create_player(float size, float sq_size, float s_x, float s_y);
void create_trophy(float size, float sq_size, float s_x, float s_y);

void initGame() {
	player[0] = player[1] = 1;

	
	//trophy[0] = trophy[1] = mazesize - 2;
	game_status = 1;		// 1 is playing, 0 is gameover
	game_elapsed_time = 0;
}

void initLights(void) {

	//glEnable(GL_LIGHT0);							   // Quick And Dirty Lighting (Assumes Light0 Is Set Up)
	
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
			initGame();
			break;
		case GLUT_KEY_UP:
			if (game_status == 0) {
				return;
			}
			if (maze[player[0] - 1][player[1]] != 1) {
				player[0]--;
			}
			break;
		case GLUT_KEY_DOWN:
			if (game_status == 0) {
				return;
			}
			if (maze[player[0] + 1][player[1]] != 1) {
				player[0]++;
			}
			break;
		case GLUT_KEY_LEFT:
			if (game_status == 0) {
				return;
			}
			if (maze[player[0]][player[1] - 1] != 1) {
				player[1]--;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (game_status == 0) {
				return;
			}
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

void create_trophy(float size, float sq_size, float s_x, float s_y) {
	float x = s_x + sq_size * trophy[1];
	float y = s_y - sq_size * trophy[0];
	glColor3f(50.0f, 0.0f, 100.0f);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glutSolidSphere(size, 50, 50);
	glPopMatrix();
}

void create_timeblock(float x, float y, float size) {
	glColor3f(0.0f, 0.0f, 100.0f);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glutSolidCube(size);
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
int temp_time = 0;
void render(void)   
{
	int  ct = glutGet(GLUT_ELAPSED_TIME);
	ct = ct / 1000;
	if ( game_status == 1 && ct > temp_time) {
		temp_time = ct;
		game_elapsed_time++;
		printf("time: %d\n", game_elapsed_time);
	}
	if (game_elapsed_time > gametime) {
		game_status = 0;
	}
	if (player[0] == trophy[0] && player[1] == trophy[1]) {
		game_status = 0;
	}
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
	float t_size = 2.3;		// size of trophy;
	float tb_size = 20;	// size of time block;

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
	create_trophy(t_size, size, s_x, s_y);

	const int time_per_cube = 10;

	int number_of_blocks = gametime / time_per_cube;
	number_of_blocks -= game_elapsed_time / time_per_cube;
	for (int i = 0; i < number_of_blocks; i++) {
		float tb_x = s_x + tb_size * 2 * i;
		tb_x += 100;
		float tb_y = s_y + 20;
		
		create_timeblock(tb_x, tb_y,tb_size);
	}
	

	glLoadIdentity();

    // Swap The Buffers To Make Our Rendering Visible
    glutSwapBuffers();
	glutPostRedisplay(); //animation
}