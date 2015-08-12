#ifndef SOURCEMATHLIB_H
#define SOURCEMATHLIB_H
#include "hooks.h"

#ifndef M_PI
	#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

#ifndef DEG2RAD
	#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
	#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

// MOVEMENT INFO
enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};


void SinCos(float x, float* s, float* c);
//-----------------------------------------------------------------------------
// Euler QAngle -> Basis Vectors
//-----------------------------------------------------------------------------
void AngleVectors (const Vector& angles, Vector *forward);
void AngleVectors (const Vector& angles, Vector *forward, Vector *right, Vector *up);
void VectorAngles( const float *forward, float *angles );

#endif