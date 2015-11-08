

#ifndef _STRUCT_RAY_H_
#define _STRUCT_RAY_H_

// Includes
#include "Vector3d.h"

// Struct Definition
struct Ray
{
public:
	Ray ( void ) : pos(0,0,0), dir(0,0,0) {;}
	Ray ( const Vector3d & npos, const Vector3d & ndir ) : pos(npos), dir(ndir) {;}
public:
	Vector3d pos;
	Vector3d dir;
};

#endif