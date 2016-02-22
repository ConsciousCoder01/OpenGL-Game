// Solar Storm By: Chris Galletta

#include "stdafx.h"
#include <iostream>
#include "GL/glut.h"
#include "GL/GL.H"
#include "GL/glaux.h"
#include <math.h>
#include <stdlib.h>// for rnd number class
#include <time.h>

using namespace std;

const int height = 480; // window height
const int width = 720; // window width

void *font = GLUT_BITMAP_TIMES_ROMAN_24;
void *fonts[] =
{
	GLUT_BITMAP_9_BY_15,
	GLUT_BITMAP_8_BY_13,
	GLUT_BITMAP_TIMES_ROMAN_10,
	GLUT_BITMAP_TIMES_ROMAN_24
};

GLubyte ship[] = 
{
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,

        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 

        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
        0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55
};

// draw functions
void displayFirst();// initial display of objects on screen
void init(); // initializes window
void handleButton(int button, int state, int x, int y); // handler for button actions
void drawPlayer(); // draw player
void drawGUI(); // draws score, energy, shields and other useful info
void drawPlayerBullets(); // draw player ship projectiles
void drawBackground();
void drawWarp(); // draw area lines
void drawEnergy(); // draw energy
void drawAliens(); // draw alien enemies
void drawComet(); // draw comet
void drawWormhole(); // draw warped wormhole
void drawStars();
void createExplosion(int x, int y); // create explosion on enemy death
void drawExplosions(); // draw explosion
void checkCollision(); // check for collision
void translate(); // move player
int nextGaussian(); // return 1 or -1
void displayTextLarge(float x, float y, int r, int g, int b, const char *string);
void displayTextSmall(float x, float y, int r, int g, int b, const char *string);
void restartLvl();
void initLevel(); // setup level before it starts

// update functions
void update(int i);
void updateEntities();
void updateEnergyEntities();
void updateBulletEntities();
void updateEnemyEntities();
void updateParticleEntities();

// input functions
void processNormalKeys(unsigned char key, int mx, int my);
void processSpecialKeys(int key, int mx, int my);
void shoot(); // fire projectile

// star functions
void visible(int state);
void init();
void reshape(int w, int h);
void idle();
float cosAngle(float angle);
float sinAngle(float angle);

static float PI = 3.1459; // approx for PI
static float radius = 75; // radius of circle
static float INC = (PI/30); // angle measure for circle definition
static GLfloat deltax = 380.0; // change in x
static GLfloat deltay = 458.0; // change in y
static GLfloat deltaz = .001;// change in z
static GLfloat playerX = 380.0; // player charcter position x
static GLfloat playerY = 458.0; // player charcter position y

bool hasGameStarted = false;

bool animGroundColor = false;

// Player variables
GLint health = 20;
int shipEnergy = 20;

int amtBullets = 0; // total amount of pellets a player can eat in the level
int amtAliens = 6;
int score = 0;// total score player has accumulated
int curLane = 2;
int ups = 0;

struct Bullet
{
	double x; // x position on screen
	double y; // y position on screen
	int lane; // lane the bullet was fired in
	bool isVisible;// is it visible in the game?
};

struct Alien
{
	double x; // x pos
	double y; // y pos
	int lane; // shooting lane pos
	bool isVisible; // can be seen in level?
};

struct Comet
{
	double x; // x pos
	double y; // y pos
	int lane; // shooting lane pos
	bool isVisible; // can be seen in level?
};

struct Lane
{
	int x; // x pos
	int y; // y pos
};

struct Energy
{
	double x; // x pos
	double y; // y pos
	int lane; // shooting lane pos
};

struct RGB
{
	float r, g, b;
};

struct Particle
{
	double x; // x pos
	double y; // y pos
	int xa; // x amt to inc/dec
	int ya; // y amt to inc/dec
	int life; // lifetime in level
	int time;
	int color; 
	bool isVisible;
};

Bullet bulletEntities[35]; // store pellets and their positions in level
Lane lanes[5];
Lane oppLanes[5];
Particle particleEntities[16];
Energy energyEntities[10];
Comet cometEntities[12];
Alien alienEntities[6];
RGB rgb[5];

// star variables

#define MAXSTARS 150
#define MAXPOS 10000
#define MAXWARP 2
#define MAXANGLES 6000

typedef struct Star 
{
  GLint type;
  float x[2], y[2], z[2];
  float offsetX, offsetY, offsetR, rotation;
} Star;

enum
{
  STREAK = 0, // stretched
  NORM = 1	  // dot
};

GLenum doubleBuffer;
GLint windW = width, windH = height;

GLint starCount = MAXSTARS / 2;
float speed = 1.0;
GLint nitro = 0;
Star stars[MAXSTARS];
float sinTable[MAXANGLES];

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);// inits GLUT and needs to be called before any GLUT functions
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);// init glut lib files
	glutInitWindowPosition(0, 0);// init size of viewing window
	glutCreateWindow("Solar Storm");// positions window at upper left hand corner
	
	glutVisibilityFunc(visible);
	glutDisplayFunc(displayFirst);// called each time there is a display call back

	glutKeyboardFunc(processNormalKeys); // init normal keyboard keys handler
	glutSpecialFunc(processSpecialKeys); // init special keyboard keys handler
	
	init();// init projection/camera

	initLevel(); // init game/level before start
	glutTimerFunc( 1000/60, update , 1);
	glutMainLoop();//should be last func called in main. Causes program to enter an infinite loop

	return 0;
}

/*******************************************
	Init display func when game first starts
********************************************/
void displayFirst()
{
	glClear(GL_COLOR_BUFFER_BIT);// clears all buffers whose bits are set in mask
	
	drawWarp();

	displayTextLarge(280, 80, 1, 1, 1, "Solar Storm ");

	displayTextLarge(20, 420, 1, 1, 1, "Programmed by: Chris Galletta ");
	displayTextLarge(0, 180, 1, 1, 1, "Press the left and right arrow keys to move. Press the spacebar to shoot. ");
	displayTextLarge(20, 220, 1, 1, 1, "Tip: Collect the green orbs to replenish energy for ship gun.");
	displayTextLarge(40, 260, 1, 1, 1, "Press & hold the arrow keys to begin. ");

	glutSwapBuffers(); // swap framebuffs for drawing current calls
	glFlush(); // force execution of OpenGL functions 
}

/***************************************************
	Draw enemy alien ship entities
****************************************************/
void drawAliens()
{
	for(int i = 0; i < amtAliens; i++)
	{
		if(alienEntities[i].isVisible)
		{
			glBegin(GL_POLYGON);
				glVertex2i(alienEntities[i].x, alienEntities[i].y);
				glVertex2i(alienEntities[i].x + 5, alienEntities[i].y + 5);
				glVertex2i(alienEntities[i].x + 10, alienEntities[i].y);
			glEnd();

			glColor3f(0.0, 0.0, 1.0);
			glBegin(GL_POLYGON);
				glVertex2i(alienEntities[i].x - 6, alienEntities[i].y - 5);
				glVertex2i(alienEntities[i].x, alienEntities[i].y);
				glVertex2i(alienEntities[i].x + 4, alienEntities[i].y - 5);
			glEnd();

			glBegin(GL_POLYGON);
				glVertex2i(alienEntities[i].x + 4, alienEntities[i].y - 5);
				glVertex2i( alienEntities[i].x + 9, alienEntities[i].y);
				glVertex2i(alienEntities[i].x + 14,  alienEntities[i].y - 5);
			glEnd();
		
			glBegin(GL_POLYGON);
				glVertex2i(alienEntities[i].x - 12, alienEntities[i].y - 11);
				glVertex2i(alienEntities[i].x - 6, alienEntities[i].y - 6);
				glVertex2i(alienEntities[i].x - 2, alienEntities[i].y - 11);
			glEnd();

			glBegin(GL_POLYGON);
				glVertex2i(alienEntities[i].x + 8, alienEntities[i].y - 11);
				glVertex2i(alienEntities[i].x + 14, alienEntities[i].y - 6);
				glVertex2i(alienEntities[i].x + 18, alienEntities[i].y - 11);
			glEnd();

		}
	}
}

/***************************************************
	Display players score and other information
****************************************************/
void drawGUI()
{
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POLYGON);
		glVertex2i(22, 0);
		glVertex2i(22, 10);
		glVertex2i(22 + health * 4, 10);
		glVertex2i(22 + health * 4, 0);
	glEnd();

	displayTextSmall(25, 23, 1, 1, 1, "Shields");

	glColor3f(0.0,1.0,0.0);
	glBegin(GL_POLYGON);
		glVertex2i(582, 0);
		glVertex2i(582, 10);
		glVertex2i(582 + shipEnergy * 4, 10);
		glVertex2i(582 + shipEnergy * 4, 0);
	glEnd();

	displayTextSmall(590, 18, 1, 1, 1, "Energy");

	// Display current player score to screen
	displayTextLarge(20, 480, 1, 1, 1, "Score: ");
	char scoreStr[15];
	sprintf_s(scoreStr, "%d", score);// cast score to new char array variable
	displayTextLarge(88, 480, 1, 1, 1, scoreStr);

	if(health <= 0)
	{
		displayTextLarge(220, 20, 1, 1, 1, "Game Over!..");
		displayTextLarge(220, 60, 1, 1, 1, "Press E to restart level");
	}
}

/***************************************************
	Draw flashing cool blue warp
****************************************************/
void drawWarp()
{
	if(ups < 28)
	{
		animGroundColor = false;
	}
	else if(ups > 28)
	{
		animGroundColor = true;
	}
	glColor3f(0,0,.5);
// Vertical line drawing

	glColor3f(0,0,.75);

	glLineWidth(.5);
// Player movement lines
	
	glBegin(GL_POLYGON);// L
		glVertex2i(275, 240);
		glVertex2i(220, 380);
		glVertex2i(221, 380);
		glVertex2i(276, 240);
	glEnd();

	glBegin(GL_POLYGON);// M
		glVertex2i(350, 240);
		glVertex2i(350, 380);
		glVertex2i(351, 380);
		glVertex2i(351, 240);
	glEnd();

	glBegin(GL_POLYGON);//  R
		glVertex2i(430, 240);
		glVertex2i(475, 380);
		glVertex2i(476, 380);
		glVertex2i(431, 240);
	glEnd();

	drawWormhole();

	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);// far L
		glVertex2i(5, 380);
		glVertex2i(276, 240);
	glEnd();

	glLineWidth(.50);
	glBegin(GL_LINES);// far R
		glVertex2i(430, 240);
		glVertex2i(716, 380);
	glEnd();

	// top
	glLineWidth(.75);
	glBegin(GL_LINES);// far R
		glVertex2i(430, 240);
		glVertex2i(714, 100);
	glEnd();

		glLineWidth(.75);
	// Player movement lines
	glBegin(GL_LINES);// far L
		glVertex2i(5, 100);
		glVertex2i(276, 240);
	glEnd();
	
}

/****************************************************************
	Draws blue line space area to create a pseudo 3d perspective
*****************************************************************/
void drawWormhole()
{	
	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0, 0, .5);
	}
	glBegin(GL_POLYGON);
		glVertex2i(241, 222);
		glVertex2i(466, 222);
		glVertex2i(466, 223);
		glVertex2i(241, 223);
	glEnd();
	
	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0,0,.35);
	}
	glBegin(GL_POLYGON);
		glVertex2i(202, 202);
		glVertex2i(505, 202);
		glVertex2i(505, 203);
		glVertex2i(202, 203);
	glEnd();
	
	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0,0,.5);
	}

	glBegin(GL_POLYGON);
		glVertex2i(163, 180);
		glVertex2i(550, 180);
		glVertex2i(550, 181);
		glVertex2i(163, 181);
	glEnd();
	
	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0, 0, .35);
	}
	glBegin(GL_POLYGON);
		glVertex2i(120, 160);
		glVertex2i(590, 160);
		glVertex2i(590, 161);
		glVertex2i(120, 161);
	glEnd();
	
	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0,0,.5);
	}
	glBegin(GL_POLYGON);
		glVertex2i(85, 140);
		glVertex2i(633, 140);
		glVertex2i(633, 141);
		glVertex2i(85, 141);
	glEnd();
	
	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0,0,.35);
	}
	// Middle
	glBegin(GL_POLYGON);
		glVertex2i(9, 100);
		glVertex2i(714, 100);
		glVertex2i(714, 101);
		glVertex2i(9, 101);
	glEnd();



//***********************************
	// Top part of wormhole warp

	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0, 0, .35);
	}
	// Horizontal lines
	glBegin(GL_POLYGON);
		glVertex2i(275, 240);
		glVertex2i(430, 240);
		glVertex2i(430, 241);
		glVertex2i(275, 241);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0, 0, .5);
	}
	glBegin(GL_POLYGON);
		glVertex2i(238, 260);
		glVertex2i(474, 260);
		glVertex2i(474, 261);
		glVertex2i(238, 261);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0,0,.35);
	}
	glBegin(GL_POLYGON);
		glVertex2i(200, 280);
		glVertex2i(514, 280);
		glVertex2i(514, 281);
		glVertex2i(200, 281);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0,0,.5);
	}

	glBegin(GL_POLYGON);
		glVertex2i(160, 300);
		glVertex2i(552, 300);
		glVertex2i(552, 301);
		glVertex2i(160, 301);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0, 0, .35);
	}
	glBegin(GL_POLYGON);
		glVertex2i(120, 320);
		glVertex2i(593, 320);
		glVertex2i(593, 321);
		glVertex2i(120, 321);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0,0,.5);
	}
	glBegin(GL_POLYGON);
		glVertex2i(80, 340);
		glVertex2i(633, 340);
		glVertex2i(633, 341);
		glVertex2i(80, 341);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.5);
	}
	else
	{
		glColor3f(0,0,.35);
	}
	// Middle
	glBegin(GL_POLYGON);
		glVertex2i(5, 380);
		glVertex2i(715, 380);
		glVertex2i(715, 381);
		glVertex2i(5, 381);
	glEnd();

	if(animGroundColor == true)
	{
		glColor3f(0,0,.35);
	}
	else
	{
		glColor3f(0,0,.5);
	}
	glBegin(GL_POLYGON);
		glVertex2i(0, 440);
		glVertex2i(720, 440);
		glVertex2i(720, 441);
		glVertex2i(0, 441);
	glEnd();	
}

/*******************************************
	Reset all neccessary values in level
********************************************/
void restartLvl()
{
	health = 20;
	shipEnergy = 20;
	score = 0;
	init();
}

/***********************************************
 * Processes input for normal keyboard keys
 key : corresponding keyboard char 
 ***********************************************/
void processNormalKeys(unsigned char key, int mx, int my)
{
	 if(key == 'E')
	 {
		 restartLvl();
	 }

	 switch(key)
	 {
		case 'a' : // left

			if(!hasGameStarted)
			{
				hasGameStarted = true;
			}

 			if(curLane < 5 && curLane > 0)
			{
				curLane -= 1;
			}

			glutIdleFunc(translate);
						
			break;

		case 'd' : // right
			
			if(!hasGameStarted)
			{
				hasGameStarted = true;
			}
			
			if(curLane < 4)
			{
				curLane += 1;
			}

			glutIdleFunc(translate);
				
			break;

		case 'w' : // up

			glutIdleFunc(NULL);
	
			break;

		case 's' : // down

			glutIdleFunc(NULL);
			
			break;

		case ' ' : // spacebar
			 
			cout << "Space";
			shoot();

			break;

		default:

			if(hasGameStarted == false)
			{
				hasGameStarted = true;
			}
			
			glutIdleFunc(NULL);
  
			break;
	}
}

/***********************************************
  Processes input for special keyboard keys
  key: ascii number associated with char  
 ***********************************************/
void processSpecialKeys(int key, int mx, int my)
{
	 switch(key)
	 {
		case GLUT_KEY_LEFT :

			if(!hasGameStarted)
			{
				hasGameStarted = true;
			}

 			if(curLane < 5 && curLane > 0)
			{
				curLane-=1;
			}

			glutIdleFunc(translate);
			
			break;

		case GLUT_KEY_RIGHT :

			if(curLane < 4)
			{
				curLane += 1;
			}

			glutIdleFunc(translate);
			
			break;

		case GLUT_KEY_UP :

			break;

		case GLUT_KEY_DOWN :

			break;

		default: 

			if(hasGameStarted == false)
			{
				hasGameStarted = true;
			}

			glutIdleFunc(NULL);

			break;
	}
}

/*************************************************
	Fire a single projectile forward
**************************************************/
void shoot()
{
	// init bullet x/y positions
	if(amtBullets < 35 && shipEnergy <= 20 && shipEnergy > 0)
	{
		bulletEntities[amtBullets].isVisible = true;	
		bulletEntities[amtBullets].x = deltax + 21; 
		bulletEntities[amtBullets].y = deltay;
		bulletEntities[amtBullets].lane = curLane;
		shipEnergy -= 2;
	
		if(curLane == 0)
		{
			bulletEntities[amtBullets].x +=4; 
		}
		else if(curLane == 3)
		{
			bulletEntities[amtBullets].x +=2; 
			bulletEntities[amtBullets].y +=2; 
		}
		else if(curLane == 4)
		{
			bulletEntities[amtBullets].x +=4; 
			bulletEntities[amtBullets].y -=5; 
		}
					
			amtBullets +=1;// track/count new bullet					
	}
	else
	{
		amtBullets = 0;
	}			
}

/*******************************************
	Initialize GL camera and perspective
********************************************/
void init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 1.0);

	// set up std orthogonal view
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
}

/********************************************
	Main update function which updates entity data in level
	i: state value for glutTimerFunc
*********************************************/
void update(int i)
{
	ups++;

	if(ups > 60)
	{
		ups = 0;
	}

	if(hasGameStarted)
	{
		idle();
		updateEntities();
		checkCollision();
	}
	
	glutTimerFunc(1000/60, update, 1);
}

/*********************************************************
	Triggers updates for all types of entities in level
***********************************************************/
void updateEntities()
{
	updateBulletEntities();
	
	updateEnergyEntities();

	updateEnemyEntities();

	updateParticleEntities();
}

/*************************************************
	Update explosion particles in level
**************************************************/
void updateParticleEntities()
{
	int i;
	for(i = 0; i < 16; i++)
	{
		particleEntities[i].time++;
		if(particleEntities[i].time >= 7400) 
		{
			particleEntities[i].time = 0;
		}

		if(particleEntities[i].time > particleEntities[i].life)
		{
			particleEntities[i].isVisible = false;
		}
		
		particleEntities[i].x += particleEntities[i].xa;
		particleEntities[i].y += particleEntities[i].ya;
	}
}

/***************************************************
	Update Enemy entities in level
***************************************************/
void updateEnemyEntities()
{
	int i;

	if(cometEntities[0].y > 416)
	{
		cometEntities[0].lane = rand() %  5;
		cometEntities[0].x = oppLanes[cometEntities[0].lane].x;
		cometEntities[0].y = oppLanes[cometEntities[0].lane].y;
	}
	else if(cometEntities[0].lane == 0)
	{
		cometEntities[0].x -= 2.8 ;
		cometEntities[0].y += 1.5;
	}
	else if(cometEntities[0].lane == 1)
	{
		cometEntities[0].x -= 0.5 ;
		cometEntities[0].y += 1.0;
	}
	else if(cometEntities[0].lane == 2)
	{
		cometEntities[0].y += 1.0;
	}
	else if(cometEntities[0].lane == 3)
	{
		cometEntities[0].x += 0.50 ;
		cometEntities[0].y += 1.5;
	}
	else if(cometEntities[0].lane == 4)
	{
		cometEntities[0].x += 3.0;
		cometEntities[0].y += 1.5;
	}

	for(i = 0; i < amtAliens; i++)
	{
		if(alienEntities[i].y > 416)
		{
			alienEntities[i].lane = rand() %  5;
			alienEntities[i].x = oppLanes[alienEntities[0].lane].x;
			alienEntities[i].y = oppLanes[alienEntities[0].lane].y;
		}
		else if(alienEntities[0].lane == 0)
		{
			alienEntities[i].x -= 3.0 ;
			alienEntities[i].y +=1.5;
		}
		else if(alienEntities[0].lane == 1)
		{
			alienEntities[i].x -= 0.75 ;
			alienEntities[i].y += 1.5;
		}
		else if(alienEntities[i].lane == 2)
		{
			alienEntities[i].y += 1.0;
		}
		else if(alienEntities[i].lane == 3)
		{
			alienEntities[i].x += 0.50 ;
			alienEntities[i].y += 1.5;
		}
		else if(alienEntities[i].lane == 4)
		{
			alienEntities[i].x += 2.8 ;
			alienEntities[i].y += 1.5;
		}
	}
}

/*******************************************************
	Update energy entities in level
********************************************************/
void updateEnergyEntities()
{
	if(energyEntities[0].y > 416)
	{
		energyEntities[0].lane = rand() %  5;
		energyEntities[0].x = oppLanes[energyEntities[0].lane].x;
		energyEntities[0].y = oppLanes[energyEntities[0].lane].y;
	}
	else if(energyEntities[0].lane == 0)
	{
		energyEntities[0].x-=1.5 ;
		energyEntities[0].y +=.75;
	}
	else if(energyEntities[0].lane == 1)
	{
		energyEntities[0].x-=0.5 ;
		energyEntities[0].y +=1.5;
	}
	else if(energyEntities[0].lane == 2)
	{
		energyEntities[0].y += 1.0;
	}
	else if(energyEntities[0].lane == 3)
	{
		energyEntities[0].x+=0.5 ;
		energyEntities[0].y +=1.5;
	}
	else if(energyEntities[0].lane == 4)
	{
		energyEntities[0].x+=1.5 ;
		energyEntities[0].y +=.75;
	}
}

/*********************************************
	Updates player projectiles in level
*********************************************/
void updateBulletEntities()
{
	for(int i = 0; i < amtBullets; i++)
	{
		if(bulletEntities[i].isVisible)
		{
			if(bulletEntities[i].y <= 260)
			{
				bulletEntities[i].y = 260;
				bulletEntities[i].isVisible = false;
			}

			if(bulletEntities[i].lane == 0)
			{
				bulletEntities[i].y -= 5;
				bulletEntities[i].x += 10;
			}
			else if(bulletEntities[i].lane == 1)
			{
				bulletEntities[i].y -= 5;
				bulletEntities[i].x += 2;
			}
			else if(bulletEntities[i].lane == 2)
			{
				bulletEntities[i].y -= 5;
			}
			else if(bulletEntities[i].lane == 3)
			{
				bulletEntities[i].y -= 5;
				bulletEntities[i].x -= 2;
			}
			else if(bulletEntities[i].lane == 4)
			{
				bulletEntities[i].y -= 5;
				bulletEntities[i].x -= 9.5;
			}
		}
	}
}

/**********************************************************************************************
	Checks collision between various entities in the level between the players bullets and ship. 
***********************************************************************************************/
void checkCollision()
{
	if(energyEntities[0].x <= deltax + 40 && energyEntities[0].x >= deltax - 20 && energyEntities[0].y >= deltay - 20 && energyEntities[0].y <= deltay)
	{
		score += 10;
		if(shipEnergy <= 16)
		{		
			shipEnergy += 4;
		}
		energyEntities[0].lane = rand() %  5;
		energyEntities[0].x = oppLanes[energyEntities[0].lane].x;
		energyEntities[0].y = oppLanes[energyEntities[0].lane].y;
	}

	if(alienEntities[0].x <= deltax + 35 && alienEntities[0].x >= deltax - 50 && alienEntities[0].y >= deltay - 10 && alienEntities[0].y <= deltay )
	{
		if(health > 0){	health -= 5;}

		alienEntities[0].lane = rand() %  5;
		alienEntities[0].x = oppLanes[alienEntities[0].lane].x;
		alienEntities[0].y = oppLanes[alienEntities[0].lane].y;
	}

	if(cometEntities[0].x <= deltax + 35 && cometEntities[0].x >= deltax - 50 && cometEntities[0].y >= deltay - 10 && cometEntities[0].y <= deltay )
	{
		if(health > 0){	health -= 5;}

		cometEntities[0].lane = rand() %  5;
		cometEntities[0].x = oppLanes[cometEntities[0].lane].x;
		cometEntities[0].y = oppLanes[cometEntities[0].lane].y;
	}

	for(int i = 0; i < amtBullets; i++)
	{
		if(bulletEntities[i].isVisible)
		{
			if(cometEntities[0].x <= bulletEntities[i].x + 40 && cometEntities[0].x >= bulletEntities[i].x - 20 && cometEntities[0].y >= bulletEntities[i].y - 20 && cometEntities[0].y <= bulletEntities[i].y)
			{
				score += 100;
				createExplosion(cometEntities[0].x, cometEntities[0].y);
				cometEntities[0].lane = rand() %  5;
				cometEntities[0].x = oppLanes[cometEntities[0].lane].x;
				cometEntities[0].y = oppLanes[cometEntities[0].lane].y;
				bulletEntities[i].isVisible = false;
			}
			else if(alienEntities[0].x <= bulletEntities[i].x + 80 && alienEntities[0].x >= bulletEntities[i].x - 25 && alienEntities[0].y >= bulletEntities[i].y - 40 && alienEntities[0].y <= bulletEntities[i].y)
			{
				score += 120;
				createExplosion(alienEntities[0].x, alienEntities[0].y);
				alienEntities[0].lane = rand() %  5;
				alienEntities[0].x = oppLanes[alienEntities[0].lane].x;
				alienEntities[0].y = oppLanes[alienEntities[0].lane].y;
				bulletEntities[i].isVisible = false;
			}
		}
	}
}

/*************************************************
	Populate particleEntities list on entity death
	x : x position on the screen to be later drawn
	y : y position on the screen to be later drawn
**************************************************/
void createExplosion(int x, int y)
{
	for(int i = 0; i < 16; i++)
	{
		particleEntities[i].x = x;
		particleEntities[i].y = y;
		particleEntities[i].life = 24;
		particleEntities[i].time = 0;
		particleEntities[i].xa = nextGaussian(); // return num -1 or 1
		particleEntities[i].ya = nextGaussian();
		particleEntities[i].isVisible = true;
		particleEntities[i].color = rand() % 5;
	}
}

/*****************************************************
	Draws explosion on entity death
******************************************************/
void drawExplosions()
{
	int i;
	for(i = 0; i < 12; i++)
	{		
		if(particleEntities[i].isVisible)
		{
			glColor3f(rgb[particleEntities[i].color].r, rgb[particleEntities[i].color].g, rgb[particleEntities[i].color].b);
			glBegin(GL_POLYGON);
				glVertex2i(particleEntities[i].x, particleEntities[i].y);
				glVertex2i(particleEntities[i].x + 2, particleEntities[i].y);
				glVertex2i(particleEntities[i].x + 2, particleEntities[i].y + 2);
				glVertex2i(particleEntities[i].x, particleEntities[i].y + 2);
			glEnd();
		}
	}
}

/******************************************
	Draws player character to screen
*******************************************/
void drawPlayer()
{
	glColor3f(1.0, 1.0, 0.0);// set color to same as background(black)
	deltax = lanes[curLane].x;	
	deltay = lanes[curLane].y;

	glEnable(GL_POLYGON_STIPPLE);// enable polygon texture to render stipple pattern
	glPolygonStipple(ship);

	glBegin(GL_POLYGON);
		glVertex2f(30 + deltax,  deltay +5);
		glVertex2f(10 + deltax,  deltay + 5);
		glVertex2f(20 + deltax,  deltay - 12);
	glEnd();

	glBegin(GL_POLYGON);// l
		glVertex2f(15 + deltax,  deltay +13);
		glVertex2f(5 + deltax,  deltay + 13);
		glVertex2f(10 + deltax,  deltay  + 6);
	glEnd();

	glBegin(GL_POLYGON);// r
		glVertex2f(35 + deltax,  deltay +13);
		glVertex2f(25 + deltax,  deltay + 13);
		glVertex2f(30 + deltax,  deltay  + 6);
	glEnd();

	glDisable(GL_POLYGON_STIPPLE);// enable polygon texture to render stipple pattern
}

/**************** 
 Move/Rotate Player and Aliens
*****************/
void translate()
{
	glClear(GL_COLOR_BUFFER_BIT);// clears all buffers whose bits are set in mask

	drawWarp();
	drawPlayerBullets();
	drawEnergy();

	glPushMatrix();
		glTranslatef(cometEntities[0].x, cometEntities[0].y ,0.0);
		if(cometEntities[0].y > 330)
		{
			glScalef(2.0, 2.0, 0.0);
		}
		else if(cometEntities[0].y > 260 && cometEntities[0].y < 330)
		{
			glScalef(1.50, 1.50, 0.0);
		}
		else
		{
			glScalef(0.75, 0.75, 0.0);
		}

		glTranslatef(-cometEntities[0].x, -cometEntities[0].y,0.0);
		drawComet();

	glPopMatrix();


	glPushMatrix();
		glTranslatef(alienEntities[0].x , alienEntities[0].y ,0.0);
		if(alienEntities[0].y > 330)
		{
			glScalef(1.50, 1.50, 0.0);
		}
		else if(alienEntities[0].y > 260 && alienEntities[0].y < 330)
		{
			glScalef(1.20, 1.20, 0.0);
		}
		else
		{
			glScalef(0.75, 0.75, 0.0);
		}

		if(alienEntities[0].lane == 0)
		{
			glRotatef(50, 0.0, 0.0, 1.0);
		}
		else if(alienEntities[0].lane == 1)
		{
			glRotatef(20, 0.0, 0.0, 1.0);
		}
		else if(alienEntities[0].lane == 2)
		{
			// do nothing
		}
		else if(alienEntities[0].lane == 3)
		{
			glRotatef(-20, 0.0, 0.0, 1.0);
		}
		else if (alienEntities[0].lane == 4)
		{
			glRotatef(-50, 0.0, 0.0, 1.0);
		}


		glTranslatef(-alienEntities[0].x, -alienEntities[0].y,0.0);
		drawAliens();

	glPopMatrix();

	drawGUI();
	
	// move player
	if(curLane == 0)
	{
		glPushMatrix();
			glTranslatef(deltax, deltay, 0.0);
			glRotatef(50, 0.0, 0.0, 1.0);
			glTranslatef(-deltax, -deltay, 0.0);
			drawPlayer();
		glPopMatrix();
	}
	else if(curLane == 1)	
	{
		glPushMatrix();
			glTranslatef(deltax, deltay, 0.0);
			glRotatef(20, 0.0, 0.0, 1.0);
			glTranslatef(-deltax, -deltay, 0.0);
			drawPlayer();
		glPopMatrix();
	}
	else if(curLane == 2)
	{
		glPushMatrix();
			glTranslatef(deltax, deltay, 0.0);
			
			glTranslatef(-deltax, -deltay, 0.0);
			drawPlayer();
		glPopMatrix();
	}
	else if(curLane == 3)
	{
		glPushMatrix();
			glTranslatef(deltax, deltay, 0.0);
			glRotatef(-20, 0.0, 0.0, 1.0);
			glTranslatef(-deltax, -deltay, 0.0);
			drawPlayer();
		glPopMatrix();
	}
	else if(curLane == 4)
	{
		glPushMatrix();
			glTranslatef(deltax, deltay, 0.0);
			glRotatef(-50, 0.0, 0.0, 1.0);
			glTranslatef(-deltax, -deltay, 0.0);
			drawPlayer();
		glPopMatrix();
	}

	drawExplosions();

	drawStars();

	glutSwapBuffers();
	glFlush();

}

/************************************************************************
	Displays text to screen
	x: x position on screen to be drawn
	y: y position on screen to be drawn
	r: red color value of text
	g: green color value of text
	b: blue volor value of text
	*string: collection of character that will make up the text
****************************************************************************/
void displayTextLarge( float x, float y, int r, int g, int b, const char *string ) 
{
	int i;
	int j = strlen(string);
 
	glColor3f(r, g, b);// set txt color
	glRasterPos2f( x, y );// set postion to be drawn
	// loop thru each charcter and display
	for(i = 0; i < j; i++ ) 
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, string[i] );
	}
}

/************************************************************************
	Displays text to screen
	x: x position on screen to be drawn
	y: y position on screen to be drawn
	r: red color value of text
	g: green color value of text
	b: blue volor value of text
	*string: collection of character that will make up the text
****************************************************************************/
void displayTextSmall( float x, float y, int r, int g, int b, const char *string ) 
{
	int i;
	int j = strlen(string);
 
	glColor3f(r, g, b);// set txt color
	glRasterPos2f( x, y );// set postion to be drawn
	// loop thru each charcter and display
	for(i = 0; i < j; i++ ) 
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_10, string[i] );
	}
}

/**********************************************
	Draw player projectiles
***********************************************/
void drawPlayerBullets()
{
	for(int i = 0; i < amtBullets; i++)
	{
		//init pebble x/y positions
	
		glColor3f(1.0, 1.0, 1.0);// set color to white

		
		if(bulletEntities[i].isVisible)
		{
			glBegin(GL_POLYGON);
				for(double t = 0; t <= 2*PI; t += INC)
				{
					glVertex2i(bulletEntities[i].x + 3 * cos(t), bulletEntities[i].y + 3 * sin(t));
					glColor4f(1.0, 1.0, 1.0, .25);// set shading color to white
				}
			glEnd();
		}
	}
}

/*********************************
	Draw comet entity
**********************************/
void drawComet()
{
	glColor3f(1.0, 1.0, 1.0);
	
	glBegin(GL_POLYGON);
		for(double t = 0; t <= 2*PI; t += INC)
		{			
			glVertex2i(cometEntities[0].x + 5 * cos(t), cometEntities[0].y + 5 * sin(t));

			glColor4f(1.0, 0.0, 0.0, .25);// set shading color to white
		}
	glEnd();
}

/***********************************
	Draw energy entity
************************************/
void drawEnergy()
{
	glColor3f(0.0, 1.0, 0.1);
		
	glBegin(GL_POLYGON);
		for(double t = 0; t <= 2*PI; t += INC)
		{
			glVertex2i(energyEntities[0].x + 4 * cos(t), energyEntities[0].y + 4 * sin(t));
			glColor4f(0, 1, 0, .5);// set shading color to white
		}
	glEnd();
}

/****************************************
	Create new star based on type
	i: star index in array
	d: max position star can move
****************************************/
void newStar(GLint i, GLint d)
{
	if (rand() % 4 == 0) 
	{
		stars[i].type = NORM;
	} 
	else
	{
		stars[i].type = STREAK;
	}

	stars[i].x[0] = (float) (rand() % MAXPOS - MAXPOS / 2);
	stars[i].y[0] = (float) (rand() % MAXPOS - MAXPOS / 2);
	stars[i].z[0] = (float) (rand() % MAXPOS + d);
	stars[i].x[1] = stars[i].x[0];
	stars[i].y[1] = stars[i].y[0];
	stars[i].z[1] = stars[i].z[0];

	stars[i].offsetX = 0.0;
	stars[i].offsetY = 0.0;
	stars[i].offsetR = 0.0;	
}

/*********************************************
	Move/update star positions
*********************************************/
void moveStars()
{
	float offset;
	GLint i;

	offset = speed * 60.0;

	for (i = 0; i < starCount; i++)
	{
		stars[i].x[1] = stars[i].x[0];
		stars[i].y[1] = stars[i].y[0];
		stars[i].z[1] = stars[i].z[0];
		stars[i].x[0] += stars[i].offsetX;
		stars[i].y[0] += stars[i].offsetY;
		stars[i].z[0] -= offset;
		stars[i].rotation += stars[i].offsetR;
    
		if (stars[i].rotation > MAXANGLES)
		{
			stars[i].rotation = 0.0;
		}
	}
}

/*****************************
	Find absolute star position
	i: star index in array
*****************************/
GLenum starPoint(GLint i)
{
	float x0, y0;

	x0 = stars[i].x[0] * windW / stars[i].z[0];
	y0 = stars[i].y[0] * windH / stars[i].z[0];
	x0 += windW / 2.0;
	y0 += windH / 2.0;

	if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH)
	{
		return GL_TRUE;
	} 
	else 
	{
		return GL_FALSE;
	}
}

/****************************************
	Draw start to screen
	i: star index in array
*****************************************/
void drawStar(GLint i)
{
	float x0, y0, x1, y1, width;
	x0 = stars[i].x[0] * windW / stars[i].z[0];
	y0 = stars[i].y[0] * windH / stars[i].z[0];
	
	x0 += windW / 2.0;
	y0 += windH / 2.0;

	if (x0 >= 0.0 && x0 < windW && y0 >= 0.0 && y0 < windH) 
	{
		if (stars[i].type == STREAK) 
		{
			x1 = stars[i].x[1] * windW / stars[i].z[1];
			y1 = stars[i].y[1] * windH / stars[i].z[1];
			x1 += windW / 2.0;
			y1 += windH / 2.0;

			glLineWidth(MAXPOS / 100.0 / stars[i].z[0] + 1.0);
			glColor3f(1.0,1.0,1.0);

			if (fabs(x0 - x1) < 1.0 && fabs(y0 - y1) < 1.0)
			{
				glBegin(GL_POINTS);
				glVertex2f(x0, y0);
				glEnd();
			}	
			else
			{
				glBegin(GL_LINES);
					glVertex2f(x0, y0);
					glVertex2f(x1, y1);
				glEnd();
			}
		} 
		else 
		{
			width = MAXPOS / 10.0 / stars[i].z[0] + 1.0;
			glColor3f(1.0, 1.0, 0.0);
				
			glBegin(GL_POLYGON);
				for (i = 0; i < 8; i++) 
				{
					float x = x0 + width * cosAngle((float) i * MAXANGLES / 8.0);
					float y = y0 + width * sinAngle((float) i * MAXANGLES / 8.0);
					glVertex2f(x, y);
				};
			glEnd();
		}
	}
}

/****************************************
	Update/create stars based on speed
****************************************/
void updateStars()
{
	GLint n;

	for (n = 0; n < starCount; n++) 
	{
		if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP))
		{
			if (starPoint(n) == GL_FALSE)
			{
				newStar(n, MAXPOS);
			}
		} 
		else 
		{
			newStar(n, MAXPOS);
		}
	}
}

/*****************************
	Draw stars to screen
******************************/
void drawStars()
{
	GLint n;

    for (n = 0; n < starCount; n++) 
    {
		if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP)) 
		{
			drawStar(n);
		}
	 }
}

/**********************************************************
	Update and move stars during idle callback
**********************************************************/
void idle()
{
	moveStars();
	updateStars();

	if (nitro > 0)
	{
		speed = (float) (nitro / 10) + 1.0;
		if (speed > MAXWARP) 
		{
			speed = MAXWARP;
		}
		if (++nitro > MAXWARP * 10)
		{
			nitro = -nitro;
		}
  } 
  else if (nitro < 0) 
  {
		nitro++;
		speed = (float) (-nitro / 10) + 1.0;
		if (speed > MAXWARP) 
		{
			speed = MAXWARP;
		}
  }
}

/*********************************************
	Set idle func based on state
	state: int value on whether glut is visible
***********************************************/
void visible(int state)
{
	if (state == GLUT_VISIBLE)
	{
		glutIdleFunc(idle);
	}
	else 
	{
		glutIdleFunc(NULL);
	}
}

/************************************************************
	Initializes the lanes,positions and entities in level before the game starts 
************************************************************/
void initLevel()
{
	float angle;
    GLint n;

	lanes[0].x = 0;
	lanes[0].y = 363;
	lanes[1].x = 198;
	lanes[1].y = 380;
	lanes[2].x = 330;
	lanes[2].y = 380;
	lanes[3].x = 458;
	lanes[3].y = 385;
	lanes[4].x = 700;
	lanes[4].y = 395;	


	oppLanes[0].x = 280;
	oppLanes[0].y = 240;
	oppLanes[1].x = 280;
	oppLanes[1].y = 240;
	oppLanes[2].x = 353;
	oppLanes[2].y = 240;
	oppLanes[3].x = 430;
	oppLanes[3].y = 240;
	oppLanes[4].x = 430;
	oppLanes[4].y = 240;

	alienEntities[0].lane = 1;
	alienEntities[0].isVisible = true;

	if(alienEntities[0].lane == 0 || alienEntities[0].lane == 1)
	{
		alienEntities[0].x = oppLanes[1].x;
		alienEntities[0].y = 240;
	}
	else if(alienEntities[0].lane == 2)
	{
		alienEntities[0].x = oppLanes[2].x;
		alienEntities[0].y = oppLanes[2].y;
	}


	cometEntities[0].lane = 2;
	if(cometEntities[0].lane == 0 || cometEntities[0].lane == 1)
	{
		cometEntities[0].x = oppLanes[1].x;
		cometEntities[0].y = 240;
	}
	else if(cometEntities[0].lane == 2)
	{
		cometEntities[0].x = oppLanes[2].x;
		cometEntities[0].y = oppLanes[2].y;
	}
	else if(cometEntities[0].lane == 3 || cometEntities[0].lane == 4)
	{
		cometEntities[0].x = oppLanes[4].x;
		cometEntities[0].y = oppLanes[4].y;
	}

	energyEntities[0].lane = 3;
	if(energyEntities[0].lane == 0 || energyEntities[0].lane == 1)
	{
		energyEntities[0].x = oppLanes[1].x;
		energyEntities[0].y = 240;
	}
	else if(energyEntities[0].lane == 2)
	{
		energyEntities[0].x = oppLanes[2].x;
		energyEntities[0].y = oppLanes[2].y;
	}
	else if(energyEntities[0].lane == 3 || energyEntities[0].lane == 4)
	{
		energyEntities[0].x = oppLanes[4].x;
		energyEntities[0].y = oppLanes[4].y;
	}

	// init random color values for explosion particle
	rgb [0].r = 1.0;
	rgb [0].g = 0.0;
	rgb [0].b = 0.0;

	// purple
	rgb [1].r = 1.0;
	rgb [1].g = 0.0;
	rgb [1].b = 1.0;

	rgb [2].r = 0.50;
	rgb [2].g = 0.0;
	rgb [2].b = 0.50;

	rgb [3].r = 1.0;
	rgb [3].g = 0.50;
	rgb [3].b = 0.0;

	// Yellow
	rgb [4].r = 1.0;
	rgb [4].g = 1.0;
	rgb [4].b = 0.0;

	rgb [5].r = 1.0;
	rgb [5].g = 1.0;
	rgb [5].b = 1.0;

	// init stars
	srand((unsigned int) time(NULL));

	 for (n = 0; n < MAXSTARS; n++) 
	 {
		newStar(n, 100);
	 }

	angle = 0.0;
	for (n = 0; n <= MAXANGLES; n++) 
	{
		sinTable[n] = sin(angle);
		angle += PI / (MAXANGLES / 2.0);
	}

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glDisable(GL_DITHER);
}

/*******************************************
	Returns a random integer thats either 1 or -1
********************************************/
int nextGaussian()
{
	int r ;
	r =  -1 + rand() % (2 - -2); //  LO + rand() % (HI - LO);
	
	return r;
	
}

/*****************
	Get Sin angle
*****************/
float sinAngle(float angle)
{
  return (sinTable[(GLint) angle]);
}

/******************
	Get Cos angle
*******************/
float cosAngle(float angle)
{
  return (sinTable[((GLint) angle + (MAXANGLES / 4)) % MAXANGLES]);
}
