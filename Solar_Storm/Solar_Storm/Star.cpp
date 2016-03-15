#include "stdafx.h"
#include <stdlib.h>// for rnd number class
#include "GL/glut.h"
#include "GL/GL.H"
#include <time.h>
#include <math.h> // for fabs function


// star constants
#define MAXSTARS 75
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

GLint starCount = MAXSTARS / 2;
float speed = 1.0;
GLint nitro = 0;
Star stars[MAXSTARS];
float sinTable[MAXANGLES];

static float PI = 3.1459; // approx for PI

const int height = 480; // window height
const int width = 720; // window width
GLint windW = width, windH = height;

/**************************
<-- Get Sin angle -->
**************************/
float SinAngle(float angle)
{
	return (sinTable[(GLint)angle]);
}

/*************************
<-- Get Cos angle -->
*************************/
float CosAngle(float angle)
{
	return (sinTable[((GLint)angle + (MAXANGLES / 4)) % MAXANGLES]);
}

/****************************************
<-- Create new star based on type -->
i: star index in array
d: max position star can move
****************************************/
void NewStar(GLint i, GLint d)
{
	if (rand() % 4 == 0)
	{
		stars[i].type = NORM;
	}
	else
	{
		stars[i].type = STREAK;
	}

	stars[i].x[0] = (float)(rand() % MAXPOS - MAXPOS / 2);
	stars[i].y[0] = (float)(rand() % MAXPOS - MAXPOS / 2);
	stars[i].z[0] = (float)(rand() % MAXPOS + d);
	stars[i].x[1] = stars[i].x[0];
	stars[i].y[1] = stars[i].y[0];
	stars[i].z[1] = stars[i].z[0];

	stars[i].offsetX = 0.0;
	stars[i].offsetY = 0.0;
	stars[i].offsetR = 0.0;
}

/*********************************************
<-- Move/update star positions -->
*********************************************/
void MoveStars()
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

/***************************************
<-- Find absolute star position -->
i: star index in array
***************************************/
GLenum StarPoint(GLint i)
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
<-- Draw start to screen -->
i: star index in array
*****************************************/
void DrawStar(GLint i)
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
			glColor3f(1.0, 1.0, 1.0);

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
				float x = x0 + width * CosAngle((float)i * MAXANGLES / 8.0);
				float y = y0 + width * SinAngle((float)i * MAXANGLES / 8.0);
				glVertex2f(x, y);
			};
			glEnd();
		}
	}
}

/**********************************************
<-- Update/create stars based on speed -->
**********************************************/
void UpdateStars()
{
	GLint n;

	for (n = 0; n < starCount; n++)
	{
		if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP))
		{
			if (StarPoint(n) == GL_FALSE)
			{
				NewStar(n, MAXPOS);
			}
		}
		else
		{
			NewStar(n, MAXPOS);
		}
	}
}

/*********************************
<-- Draw stars to screen -->
*********************************/
void DrawStars()
{
	GLint n;

	for (n = 0; n < starCount; n++)
	{
		if (stars[n].z[0] > speed || (stars[n].z[0] > 0.0 && speed < MAXWARP))
		{
			DrawStar(n);
		}
	}
}

/**********************************************************
<-- Update and move stars during idle callback -->
**********************************************************/
void Idle()
{
	MoveStars();
	UpdateStars();

	if (nitro > 0)
	{
		speed = (float)(nitro / 10) + 1.0;
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
		speed = (float)(-nitro / 10) + 1.0;
		if (speed > MAXWARP)
		{
			speed = MAXWARP;
		}
	}
}

/*********************************************
<-- Set idle func based on state -->
state: int value on whether glut is visible
***********************************************/
void Visible(int state)
{
	if (state == GLUT_VISIBLE)
	{
		glutIdleFunc(Idle);
	}
	else
	{
		glutIdleFunc(nullptr);
	}
}

void StarInit()
{
	// init stars
	srand((unsigned int)time(NULL));

	for (int n = 0; n < MAXSTARS; n++)
	{
		NewStar(n, 100);
	}

	float angle = 0.0;
	for (int n = 0; n <= MAXANGLES; n++)
	{
		sinTable[n] = sin(angle);
		angle += PI / (MAXANGLES / 2.0);
	}
}

/*********************************************************
<-- Returns a random integer thats either 1 or -1 -->
*********************************************************/
int NextGaussian()
{
	int r;
	r = -1 + rand() % (2 - -2); //  LO + rand() % (HI - LO);

	return r;

}
