
#ifndef _C_X_MATRIX_4X4_H_
#define _C_X_MATRIX_4X4_H_

//#include "../Vector2.h"
#include "../Vector3.h"
//#include "../Vector4.h"
//#include "../Quaternion.h"
#include "CMatrix.h"

class CXMatrix4x4 
{

public:
	Vector3f GetT ( void );
	Vector3f GetR ( void );
	Vector3f GetS ( void );
	Vector3f GetQ ( void );

private:
	Vector3f t;
	Vector3f r;
	Vector3f s;
	//Quaternion q;
	Matrix4x4 m;

};

#endif