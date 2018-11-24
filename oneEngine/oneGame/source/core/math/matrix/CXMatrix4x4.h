
#ifndef _C_X_MATRIX_4X4_H_
#define _C_X_MATRIX_4X4_H_

//#include "../Vector2f.h"
#include "../Vector3d.h"
//#include "../Vector4d.h"
//#include "../Quaternion.h"
#include "CMatrix.h"

class CXMatrix4x4 
{

public:
	Vector3d GetT ( void );
	Vector3d GetR ( void );
	Vector3d GetS ( void );
	Vector3d GetQ ( void );

private:
	Vector3d t;
	Vector3d r;
	Vector3d s;
	//Quaternion q;
	Matrix4x4 m;

};

#endif