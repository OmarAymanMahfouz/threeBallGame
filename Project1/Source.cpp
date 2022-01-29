#define _CRT_SECURE_NO_WARNINGS
#include <windows.h> //exit(0)
#include <gl/glut.h> //drawing, keyboard, mouse
#include <stdlib.h> //srand (Initialize random number generator)
#include <time.h> //timer
#include <algorithm>
#include <iostream> //defines the standard input/output stream objects (cin, cout)

using namespace std;

//Ambient, Diffuse, and Specular are called the three components of a light source (when added together, create the final color of light)
//lighting and materials constant values
//ambient 0 0 0 (no effect)
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; //light that doesn't seem to come from a specific source but is just there (under the desk)
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //directed light coming off a surface and creates shading (brightness)
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //refers to the highlights on reflective objects, like diamonds, billiard balls, and eyes (light reflection)
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

bool gameStart = false;
bool gameover = false;
int score = 0;
int highscore = 0;
//ball rotation
double ballRotate = 0;
//8 road positions (render it)
float roadPosZ = -10, roadPosZ1 = -45, roadPosZ2 = -80, roadPosZ3 = -115, roadPosZ4 = -150, roadPosZ5 = -185, roadPosZ6 = -220, roadPosZ7 = -255;
//game speed (ball movement)
float forwardSpeed = 0.1;
//ball right or left
float tBallX = 0;
//ball speed
float moveSpeed = 0.5;
string Bcolors[3] = { "Red", "Green", "Blue" };
string LastTrapColors[2][3];
bool IsCounted[2];



//random numbers for traps [-10 to 10]
int randomPosX() {
	//srand(unsigned int seed) seeds the random number generator used by the function rand
	//time() returns a time_t value which varies every time and hence the pseudo-random number varies for every program call
	srand((unsigned int)time(0));
	return (rand() % (10 - (-10))) + (-10); //rand() generate random numbers in the given range
}

//drawing the ball
void ball() {
	glColor3d(1.0, 0.0, 0.0);
	glPushMatrix();
	glTranslated(0, 0, 0);
	glScaled(2, 2, 2);
	//glutSolidSphere: Renders a sphere centered at the modeling coordinates origin of the specified radius. The sphere is subdivided around the Z axis into slices and along the Z axis into stacks
	//radius (The radius of the sphere), slices (The number of subdivisions around the Z axis), stacks (The number of subdivisions along the Z axis)
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
}

//traps random positions
float trapPosX = randomPosX();
float trapPosX2 = randomPosX();

//drawing road, lines, road sides
void road() {

	// --Road--
	glColor3d(0.768, 0.768, 0.768);
	glPushMatrix();
	glTranslated(0, 0, 0);
	glScaled(20, 0.3, 40);
	//render a solid cube respectively, the cube is centered at the modeling coordinates origin with sides of length size
	//void glutSolidCube(GLdouble size);
	glutSolidCube(1);
	glPopMatrix();

}

void GenRedBall(int Pos) {
	glColor3d(1.0, 0.0, 0.0);
	glPushMatrix();
	glTranslated(Pos, 2, 0);
	glScaled(2, 2, 2);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
}

void GenGreenBall(int Pos) {
	glColor3d(0.0, 1.0, 0.0);
	glPushMatrix();
	glTranslated(Pos, 2, 0);
	glScaled(2, 2, 2);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
}

void GenBlueBall(int Pos) {
	glColor3d(0.0, 0.0, 1.0);
	glPushMatrix();
	glTranslated(Pos, 2, 0);
	glScaled(2, 2, 2);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();
}


//random trap1 generation
void trap(int x) {
	int Pos[] = { -6, 0, 6 };

	if (LastTrapColors[x][0] == "Red") GenRedBall(Pos[0]);
	else if (LastTrapColors[x][0] == "Green") GenGreenBall(Pos[0]);
	else GenBlueBall(Pos[0]);

	if (LastTrapColors[x][1] == "Red") GenRedBall(Pos[1]);
	else if (LastTrapColors[x][1] == "Green") GenGreenBall(Pos[1]);
	else GenBlueBall(Pos[1]);

	if (LastTrapColors[x][2] == "Red") GenRedBall(Pos[2]);
	else if (LastTrapColors[x][2] == "Green") GenGreenBall(Pos[2]);
	else GenBlueBall(Pos[2]);
}



//gameover cover and conditions
bool gameOver() {
	if (roadPosZ3 > -1) {
		if (LastTrapColors[0][0] == "Red" && tBallX >= -2) return true;
		else if (LastTrapColors[0][1] == "Red" && (tBallX <= -2 || tBallX >= 2)) return true;
		else if (LastTrapColors[0][2] == "Red" && tBallX <= 2) return true;
	}
	else if (roadPosZ7 > -1) {
		if (LastTrapColors[1][0] == "Red" && tBallX >= -2) return true;
		else if (LastTrapColors[1][1] == "Red" && (tBallX <= -2 || tBallX >= 2)) return true;
		else if (LastTrapColors[1][2] == "Red" && tBallX <= 2) return true;
	}
	return false;
}

//Drawing game objects such as road, traps, and ball
void draw() {
	//ball
	glPushMatrix();
	glTranslated(tBallX, 2.5, 5);
	glRotated(ballRotate, 0, 0, 1);
	ball();
	glPopMatrix();

	//8 road blocks and 2 traps
	glPushMatrix();
	glTranslated(0, 0, roadPosZ);
	road();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ1);
	road();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ2);
	road();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ3);
	road();
	trap(0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ4);
	road();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ5);
	road();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ6);
	road();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, roadPosZ7);
	road();
	trap(1);
	glPopMatrix();

	//ball speed on the road (the road is moving not the ball)
	roadPosZ += forwardSpeed;
	roadPosZ1 += forwardSpeed;
	roadPosZ2 += forwardSpeed;
	roadPosZ3 += forwardSpeed;
	roadPosZ4 += forwardSpeed;
	roadPosZ5 += forwardSpeed;
	roadPosZ6 += forwardSpeed;
	roadPosZ7 += forwardSpeed;
	
	// increse Score
	if (roadPosZ3 >= 0 && !IsCounted[0]) {
		score++;
		IsCounted[0] = true;
	}
	else if (roadPosZ7 >= 0 && !IsCounted[1]) {
		score++;
		IsCounted[1] = true;
	}
	
	//appearing of the road blocks again
	if (roadPosZ >= 35)roadPosZ = -245;
	if (roadPosZ1 >= 35)roadPosZ1 = -245;
	if (roadPosZ2 >= 35)roadPosZ2 = -245;
	if (roadPosZ3 >= 35) {
		roadPosZ3 = -245;
		IsCounted[0] = false;
		next_permutation(Bcolors, Bcolors + 3);
		LastTrapColors[0][0] = Bcolors[0], LastTrapColors[0][1] = Bcolors[1], LastTrapColors[0][2] = Bcolors[2];
	}
	if (roadPosZ4 >= 35)roadPosZ4 = -245;
	if (roadPosZ5 >= 35)roadPosZ5 = -245;
	if (roadPosZ6 >= 35)roadPosZ6 = -245;
	if (roadPosZ7 >= 35) {
		roadPosZ7 = -245;
		IsCounted[1] = false;
		next_permutation(Bcolors, Bcolors + 3);
		LastTrapColors[1][0] = Bcolors[0], LastTrapColors[1][1] = Bcolors[1], LastTrapColors[1][2] = Bcolors[2];
	}	


	//increasing of the ball speed (the road is moving not the ball)
	forwardSpeed += 0.0005;
	//max speed
	if (forwardSpeed >= 0.3)forwardSpeed = 0.3;

}

//to draw a string of character(One character at a time) at position x,y,z
void showText(float x, float y, float z, char* str)
{
	//position
	glRasterPos3f(x, y, z);
	//get the string length
	int j = strlen(str);
	//loop for each character in the string to draw it
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]); //renders the character in the named bitmap font
	}
}

//drawing and calculating the score
void Score()
{
	glColor3f(0.5, 0.5, 0.5);			// grey
	//the string and its position
	unsigned char string[] = "Score = ";
	glRasterPos3f(-20, 20, 0);

	//get the string length
	int len = sizeof(string); //7
	//loop for each character in the string to draw it
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	//k --> new pixel, j --> new position, p --> score value
	int k = 0, j = 0, p = score;
	//draw another pixel beside the old pixel
	while (p > 9){
		k = p % 10;
		//next number position
		glRasterPos3f((-13.5 - (j * 0.5)), 20, 0);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + k);
		p /= 10;
		j++;
	}
	//first number position
	glRasterPos3f((-13.5 - (j * 0.5)), 20, 0);
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + p);
}

//drawing and calculating the high score
void HighScore()
{
	glColor3f(0.5, 0.5, 0.5);			// grey
	//the string and its position
	unsigned char string[] = "High Score = ";
	glRasterPos3f(10, 20, 0);

	//get the string length
	int len = sizeof(string);
	//loop for each character in the string to draw it
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
	//k --> new pixel, j --> new position, p --> score value
	int k = 0, j = 0, p = 0;
	//calculating the high score and its difference between the current score
	if (score > highscore) {
		p = score;
		highscore = score;
		auto e = freopen("highScore.in", "w", stdout);
		cout << highscore;
	}
	else {
		p = highscore;
	}
	//draw another pixel beside the old pixel
	do {
		k = p % 10;
		//next number position
		glRasterPos3f((18 - (j * 0.5)), 20, 0);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + k);
		p /= 10;
		j++;
	} while (p != 0);
}

//restarting the whole game (original states)
void restart() {
	ballRotate = 0;
	roadPosZ = -10, roadPosZ1 = -45, roadPosZ2 = -80, roadPosZ3 = -115, roadPosZ4 = -150, roadPosZ5 = -185, roadPosZ6 = -220, roadPosZ7 = -255;
	forwardSpeed = 0.1;
	trapPosX = randomPosX();
	trapPosX2 = randomPosX();
	tBallX = 0;
	moveSpeed = 0.5;
	score = 0;			   //score
	IsCounted[0] = IsCounted[1] = false;
}

//resizing the window
static void resize(int width, int height)
{
	const float ar = (float)width / (float)height;
	glViewport(0, 0, width, height); //Specify the lower left corner of the viewport rectangle (x,y) and Specify the width and height of the viewport
	//ex: [GL_MODELVIEW - GL_PROJECTION (perspective, orthographic) - GL_TEXTURE]
	glMatrixMode(GL_PROJECTION); //camera's lens
	glLoadIdentity(); //replace the current matrix with the identity matrix
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, 1000.0); // = perspective (relative to Z-axis) [accepts only positive values of near and far distances]

	glMatrixMode(GL_MODELVIEW); //camera view 
	glLoadIdentity(); //replace the current matrix with the identity matrix
}

//drawing function
static void RenderScene(void) {
	//clear the depth and buffer to begin a new scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); ////replace the current matrix with the identity matrix

	//camera position (3 eye (position), 3 center (pointing), 3 up vector direction)
	gluLookAt(0.0f, 15.0f, 30.0f,
		0.0f, 10.0f, 0.0f,
		0.0f, 1.0f, 0.0f);

	if (gameStart) {
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glEnable(GL_LIGHTING);

		glPushMatrix();
		glTranslated(0, 0, 0);
		draw(); //road, traps, and ball
		glPopMatrix();
		Score();
		HighScore();

		if (gameOver()) {
			glColor3f(0.5, 0.5, 0.5);
			glRasterPos3f(-3, 10, 0);
			unsigned char string[] = "Game Over!";
			int len = sizeof(string);
			for (int i = 0; i < len; i++) {
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
			}
			glutSwapBuffers();

			Sleep(1000);
			exit(0);
		}

		glutPostRedisplay(); //refresh or redisplay the scene
	}
	else {
		glDisable(GL_LIGHTING);
		glClearColor(0.105, 0.301, 0.4, 0.0);
		glColor3f(1, 0, 1);


	}
	//swaping between buffers
	glutSwapBuffers();
}

//keyboard keys
static void keyboard(unsigned char key, int x, int y)
{

	double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0; //get the number of milliseconds since glutInit called (or first call to glutGet(GLUT_ELAPSED_TIME))
	double ballRotateSpeed = t * 90.0;

	if (key == 27)								//if Esc is pressed
		exit(0);

	//move right and rotate
	if (key == 'd') {
		tBallX += moveSpeed;
		ballRotate = -ballRotateSpeed;
		if (tBallX >= 6)
			tBallX = 6;
	}

	//move left and rotate
	if (key == 'a') {
		tBallX -= moveSpeed;
		ballRotate = ballRotateSpeed;
		if (tBallX <= -6)
			tBallX = -6;
	}

	//restart the game 
	if (key == 'r' || key == 'R') {				//if r or R is pressed
		restart();
	}

	//begin the game
	if (key == 's' || key == 'S') {				//if s or S is pressed
		if (gameStart == false) {                     //Display the menu
			restart();
		}
		gameStart = true;					    //Start the Game
		RenderScene();
	}

	glutPostRedisplay(); //refresh or redisplay the scene
}

//timer function for incrementing the score
void timer(int start)
{
	glutPostRedisplay(); //refresh or redisplay the scene
	glutTimerFunc(100, timer, 0);	//timer callback function
}

//main function
int main(int argc, char* argv[]) {
	LastTrapColors[0][0] = Bcolors[0], LastTrapColors[0][1] = Bcolors[1], LastTrapColors[0][2] = Bcolors[2];
	next_permutation(Bcolors, Bcolors + 3); // this function takes two parameters : (array name, number of items)
	LastTrapColors[1][0] = Bcolors[0], LastTrapColors[1][1] = Bcolors[1], LastTrapColors[1][2] = Bcolors[2];
	auto e = freopen("highScore.in", "r", stdin);
	cin >> highscore;

	glutInit(&argc, argv); //used to initialize the GLUT library
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_RGBA); //display modes
	glutInitWindowPosition(0, 0); //x and y location of the window (for upper left corner)
	glutInitWindowSize(1280, 720); //width and height of the window
	glutCreateWindow("DodgeBall"); //window title
	glutFullScreen(); //fullscreen window
	glutReshapeFunc(resize); //resizing of the window (sets the reshape callback for the current window)
	glutDisplayFunc(RenderScene); //drawing fucntion (sets the display callback for the current window)
	glutTimerFunc(50, timer, 0); //registers a timer callback to be triggered in a specified number of milliseconds
	glutKeyboardFunc(keyboard); //keyboards keys (sets the keyboard callback for the current window)

	glClearColor(1, 1, 1, 1);

	glEnable(GL_DEPTH_TEST); //depth buffer for non-appeareance pixels (If enabled, do depth comparisons and update the depth buffer)
	//glDepthFunc: specify the value used for depth buffer comparisons
	glDepthFunc(GL_LESS); //update the less values in buffer (GL_LESS: Passes if the incoming depth value is less than the stored depth value)

	//If enabled, include light 0 in the evaluation of the lighting equation
	glEnable(GL_LIGHT0);
	//If enabled, normal vectors are normalized to unit length after transformation and before lighting
	glEnable(GL_NORMALIZE);
	//If enabled, have ambient and diffuse material parameters track the current color
	glEnable(GL_COLOR_MATERIAL);
	//If enabled, use the current lighting parameters to compute the vertex color. Otherwise, simply associate the current color with each vertex
	glEnable(GL_LIGHTING);

	//lighting
	//(glLightfv: returns light source parameter values)
	//light: The identifier of a light
	//pname: A light source parameter for light. Must be one of the following: GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_POSITION, ....
	//params: Specifies the value that parameter pname of light source light will be set to
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); //lighting in the dark
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); //brightness
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); //light reflection
	glLightfv(GL_LIGHT0, GL_POSITION, light_position); //light position

	//materials for lighting
	//(glMaterialfv: specifies material parameters for the lighting model)
	//face: The face or faces that are being updated. Must be one of the following: GL_FRONT, GL_BACK, or GL_FRONT and GL_BACK
	//pname: The material parameter of the face or faces being updated. Must be one of the following: GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_SHININESS, ....
	//params: The value to which parameter GL_SHININESS will be set
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	
	//looping the main function
	glutMainLoop(); //start the event loop; never returns
	
	return 0;
}