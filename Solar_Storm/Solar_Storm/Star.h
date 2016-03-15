#include "stdafx.h"
#include <stdlib.h>// for rnd number class
#include "GL/glut.h"
#include "GL/GL.H"
#include <time.h>
#include <math.h> // for fabs function

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
/* ^^ these are the include guards */

int NextGaussian(); // reutn 1 or -1

// <-- star functions -->
void StarInit();
void DrawStars();
void UpdateStars();
void Visible(int state);
void Idle();
void MoveStars();
void DrawStar(GLint i);
GLenum StarPoint(GLint i);
float CosAngle(float angle);
float SinAngle(float angle);

#endif
