

#ifndef _STRUCT_RAY_H_
#define _STRUCT_RAY_H_

// Includes
#include "Vector3.h"

// Struct Definition
struct Ray
{
public:
	Ray ( void ) : pos(0,0,0), dir(0,0,0) {;}
	Ray ( const Vector3f & npos, const Vector3f & ndir ) : pos(npos), dir(ndir) {;}
public:
	Vector3f pos;
	Vector3f dir;
};

#endif