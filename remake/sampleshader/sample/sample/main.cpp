#ifdef _WIN32
#include "windows.h"
#endif

#include "GLUT/glut.h"

#ifdef _WIN32
#include "glext.h"
#endif


#include <stdio.h>           // Standard C/C++ Input-Output
#include <math.h>            // Math Functions
#include <windows.h>         // Standard Header For MSWindows Applications
#include <time.h>
#include <ctime>


#ifdef _WIN32
// As microsoft did not maintain openGL after version 1.1, Windows platform need to go throught this crap ; macosX and Linux are fine.
// This block simply retries openGL function needed for this example.
// I recommend to use GLEW instead of going this way. This is done this way only to ease beginner's compilation and portability


PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

// FrameBuffer (FBO) gen, bin and texturebind
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT ;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT ;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT ;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT ;


// Shader functions
PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB ;
PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB   ;
PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB ;
PFNGLSHADERSOURCEARBPROC         glShaderSourceARB        ;
PFNGLCOMPILESHADERARBPROC        glCompileShaderARB       ;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
PFNGLATTACHOBJECTARBPROC         glAttachObjectARB        ;
PFNGLLINKPROGRAMARBPROC          glLinkProgramARB         ;
PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB  ;
PFNGLUNIFORM1IARBPROC            glUniform1iARB           ;
PFNGLUNIFORM4FARBPROC            glUniform4fARB           ;
PFNGLUNIFORM3FARBPROC            glUniform3fARB           ;
//PFNGLACTIVETEXTUREARBPROC		  glActiveTextureARB;
PFNGLGETINFOLOGARBPROC           glGetInfoLogARB          ;

void getOpenGLFunctionPointers(void)
{
	// FBO
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	glGenFramebuffersEXT		= (PFNGLGENFRAMEBUFFERSEXTPROC)		wglGetProcAddress("glGenFramebuffersEXT");
	glBindFramebufferEXT		= (PFNGLBINDFRAMEBUFFEREXTPROC)		wglGetProcAddress("glBindFramebufferEXT");
	glFramebufferTexture2DEXT	= (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glCheckFramebufferStatusEXT	= (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	
	//Shaders
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB"); 
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB"); 
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB"); 
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB"); 
	glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
	glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
	glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
	glUniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
	
}
#endif

// Expressed as float so gluPerspective division returns a float and not 0 (640/480 != 640.0/480.0).
#define RENDER_WIDTH 640.0
#define RENDER_HEIGHT 480.0
#define SHADOW_MAP_RATIO 2

float aspect = 1;
//Camera position
float p_camera[3] = {32,20,600};

//Camera lookAt
float l_camera[3] = {0,0,600};

//Light position
float p_light[3] = {0,0,0};

//Light lookAt
float l_light[3] = {0,1,0};


//Light mouvement circle radius
float light_mvnt = 30.0f;

// Hold id of the framebuffer for light POV rendering
GLuint fboId;

// Z values will be rendered to this texture when using fboId framebuffer
GLuint depthTextureId;

// Use to activate/disable shadowShader
GLhandleARB shadowShaderId;
GLuint variablelocation;

GLint loc;

const int gametime = 120;	// in seconds
const int arraysize = 11;	// initial maze size
const int mazesize = arraysize * arraysize;	// maze size

int player[2] =  { 6,0 };	// player starting position
int trophy[2] ={ mazesize - 6, mazesize - 1 };

bool game_status = 1;		// 1 is playing, 0 is gameover
int game_elapsed_time = 0;

bool lilmaze[arraysize][arraysize] = {
	{ 1,1,1,1,1,0,1,1,1,1,1 },//1
	{ 1,0,0,0,0,0,0,0,0,0,0 },
	{ 1,0,1,1,1,1,1,1,0,1,1 },
	{ 1,0,0,0,0,0,0,1,0,0,0 },
	{ 1,1,1,1,0,1,1,1,1,0,1 },
	{ 0,0,0,0,0,1,0,0,0,0,0 },//6
	{ 1,0,1,0,1,1,1,1,0,1,1 },
	{ 1,0,1,0,0,0,1,0,0,0,0 },
	{ 1,0,1,1,1,0,0,0,1,1,1 },
	{ 1,0,0,0,1,0,1,0,0,0,0 },
	{ 1,0,1,0,1,0,1,0,1,0,1 },//11
};


//Function Prototypes
void keyboard(unsigned char key, int x, int y);
void special_keys(int a_keys, int x, int y);
void create_square(float x, float y, float size, int v);
void create_player(float size, float sq_size, float s_x, float s_y);
void create_trophy(float size, float sq_size, float s_x, float s_y);
void create_timeblock(float x, float y, float size);
bool init(void);
void reshape(int w, int h);
void positionCamera(void);
void initLights(void);

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

bool init(void){
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
void initLights(void) {

	//glEnable(GL_LIGHT0);							   // Quick And Dirty Lighting (Assumes Light0 Is Set Up)
	
	glEnable(GL_LIGHTING);							   // Enable Lighting
}


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

// Loading shader function
GLhandleARB loadShader(char* filename, unsigned int type)
{
	FILE *pfile;
	GLhandleARB handle;
	const GLcharARB* files[1];
	
	// shader Compilation variable
	GLint result;				// Compilation code result
	GLint errorLoglength ;
	char* errorLogText;
	GLsizei actualErrorLogLength;
	
	char buffer[400000];
	memset(buffer,0,400000);
	
	// This will raise a warning on MS compiler
	pfile = fopen(filename, "rb");
	if(!pfile)
	{
		printf("Sorry, can't open file: '%s'.\n", filename);
		exit(0);
	}
	
	fread(buffer,sizeof(char),400000,pfile);
	//printf("%s\n",buffer);
	
	
	fclose(pfile);
	
	handle = glCreateShaderObjectARB(type);
	if (!handle)
	{
		//We have failed creating the vertex shader object.
		printf("Failed creating vertex shader object from file: %s.",filename);
		exit(0);
	}
	
	files[0] = (const GLcharARB*)buffer;
	glShaderSourceARB(
					  handle, //The handle to our shader
					  1, //The number of files.
					  files, //An array of const char * data, which represents the source code of theshaders
					  NULL);
	
	glCompileShaderARB(handle);
	
	//Compilation checking.
	glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);
	
	// If an error was detected.
	if (!result)
	{
		//We failed to compile.
		printf("Shader '%s' failed compilation.\n",filename);
		
		//Attempt to get the length of our error log.
		glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
		
		//Create a buffer to read compilation error message
		errorLogText = (char *)malloc(sizeof(char) * errorLoglength);
		
		//Used to get the final length of the log.
		glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);
		
		// Display errors.
		printf("%s\n",errorLogText);
		
		// Free the buffer malloced earlier
		free(errorLogText);
	}
	
	return handle;
}

void loadShadowShader()
{
	GLhandleARB vertexShaderHandle;
	GLhandleARB fragmentShaderHandle;
	
	vertexShaderHandle   = loadShader("VertexShader_ori.c",GL_VERTEX_SHADER);
	fragmentShaderHandle = loadShader("FragmentShader_ori.c",GL_FRAGMENT_SHADER);
	
	shadowShaderId = glCreateProgramObjectARB();
	
	glAttachObjectARB(shadowShaderId,vertexShaderHandle);
	glAttachObjectARB(shadowShaderId,fragmentShaderHandle);
	glLinkProgramARB(shadowShaderId);

	//setup uniform here
}



void setupMatrices(float position_x,float position_y,float position_z,float lookAt_x,float lookAt_y,float lookAt_z)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,RENDER_WIDTH/RENDER_HEIGHT,10,600.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(position_x,position_y,position_z,lookAt_x,lookAt_y,lookAt_z,0,1,0);
}


// This update only change the position of the light.
//int elapsedTimeCounter = 0;
void update(void)
{
	
	p_light[0] = light_mvnt * cos(glutGet(GLUT_ELAPSED_TIME)/1000.0);
	p_light[2] = light_mvnt * sin(glutGet(GLUT_ELAPSED_TIME)/1000.0);
	
	//p_light[0] = light_mvnt * cos(3652/1000.0);
	//p_light[2] = light_mvnt * sin(3652/1000.0);
}




// Our Rendering Is Done Here
int timer = 0;
int temp_time = 0;
void renderScene(void){

	//update();

	
	glViewport(0,0,RENDER_WIDTH,RENDER_HEIGHT);
	
	//Enabling color write (previously disabled for light POV z-buffer rendering)
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); 
	

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
	//timer++;
	//printf("timer: %d | %d\n", time,timer);

	// Clear previous frame values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Using the shadow shader
	glUseProgramObjectARB(shadowShaderId);

	//setup uniform value here
	
	// setupMatrices(p_camera[0],p_camera[1],p_camera[2],l_camera[0],l_camera[1],l_camera[2]);
	
	//glCullFace(GL_BACK);
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
	

	
	glutSwapBuffers();
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
		glPushMatrix();
			glTranslatef(x, y, -1);
			glutSolidCube(size);
		glPopMatrix();
	}
	else { // walking space
		glColor3f(0.0f, 100.0f, 0.0f);
		glPushMatrix();
			glTranslatef(x, y, -2);
			glutSolidCube(size);
		glPopMatrix();
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
	GLfloat player_mat_ambient_diffuse[] = { 1.0, 0.9, 1.0, 1.0 };
	GLfloat player_mat_specular[] = { 1.0, 0.9, 1.0, 1.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, player_mat_ambient_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, player_mat_specular);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glutSolidSphere(size, 50, 50);
	glPopMatrix();
}

void create_trophy(float size, float sq_size, float s_x, float s_y) {
	float x = s_x + sq_size * trophy[1];
	float y = s_y - sq_size * trophy[0];
	glColor3f(50.0f, 0.0f, 100.0f);
	GLfloat trophy_mat_ambient_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat trophy_mat_specular[] = { 1.0, 0.0, 0.0, 1.0 };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, trophy_mat_ambient_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, trophy_mat_specular);
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


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	makemaze();
	glutInitWindowPosition(100,100);
	glutInitWindowSize(RENDER_WIDTH,RENDER_HEIGHT);
	glutCreateWindow("EPL 426 Maze");
	
	// This call will grab openGL extension function pointers.
	// This is not necessary for macosx and linux
#ifdef _WIN32
	getOpenGLFunctionPointers();
#endif

	loadShadowShader();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 1.0f, 1.0f, 0.5f);			   // bluish background (CHANGED)
	
	//glEnable(GL_CULL_FACE);
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	
	
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);							// Register The Keyboard Handler
	glutSpecialFunc(special_keys);						// Register Special Keys Handler
	
	glutMainLoop();
}