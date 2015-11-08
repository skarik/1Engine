// The matrix class used for transforms
//------------------------------------
// Please note that the matrix is defined in the same manner that OpenGL uses matrices!
// All matrices are column order. This changes multiplication order. For example, if you 
// want to rotate something, then translate it, the multiplication order is:
//  result = translationMatrix * rotationMatrix;
// Wheras in with a row order matrix, the order would be:
//  result = rotationMatrix * translationMatrix;
// This can cause extreme confusion, since the popular notation is the latter definition.
// If you want to get a row order matrix, there is always the transpose() function
//  transpose(rotationMatrixColumnOrder * translationMatrixColumnOrder) = ( translationMatrixRowOrder*rotationMatrixRowOrder )
// Please note I could be switching around row order and column order. If so, please fix this.
//------------------------------------

#ifndef C_MATRIX_H
#define C_MATRIX_H

// Includes
#include <cmath>
#include <climits>
#include <cstddef>
#include <cfloat>
#include <cassert>

//#include "floattype.h"
#include "core/types/types.h"

#include "../Vector2d.h"
#include "../Vector3d.h"
#include "../Vector4d.h"
#include "../Quaternion.h"

class Vector2d;
class Vector3d;
class Vector4d;
class Quaternion;
class Matrix2x2;
class Matrix3x3;
class Matrix4x4;

#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Matrix2x2.hpp"
#include "Matrix3x3.hpp"
#include "Matrix4x4.hpp"
/*
// Class Definition
template <int W, int H>
class CMatrix
{
public:
	// Constructor
	CMatrix ( void );
	//CMatrix ( CMatrix const& ); // Copy constructor not needed!
	CMatrix ( ftype * );

	// Setters - Translation
	bool setTranslation ( ftype const = 0, ftype const = 0, ftype const = 0 );
	bool setTranslation ( ftype * );
	bool setTranslation ( Vector2d const& );
	bool setTranslation ( Vector3d const& );

	// Setters - Scaling
	bool setScale ( ftype const = 0, ftype const = 0, ftype const = 0 );
	bool setScale ( ftype * );
	bool setScale ( Vector2d const& );
	bool setScale ( Vector3d const& );

	// Setters - Rotation
	bool setRotation ( ftype const );
	bool setRotation ( ftype * );
	bool setRotation ( ftype const = 0, ftype const = 0, ftype const = 0 );
	bool setRotation ( Vector3d const& );
	bool setRotation ( Quaternion const& );
	bool setRotation ( CMatrix<3,3> const& );

	// Getters - Translation
	Vector3d getTranslation ( void ) const;

	// Getters - Rotation
	Vector3d getEulerAngles ( void ) const;
	Quaternion getQuaternion ( void ) const;
	CMatrix<3,3> getRotator ( void ) const;

	// Getters - Scale
	Vector3d getScaling ( void );

	// Modders - Translation
	void translate ( Vector3d const& );

	// Operations
	CMatrix<H,W> transpose ( void );
	CMatrix<W,H> inverse ( void );
	ftype det ( void );
	
	CMatrix<W-1,H-1> submatrix ( int, int ) const;

	CMatrix<W,H> lerp( CMatrix<W,H> const&, ftype const ) const;
	void Lerp ( CMatrix<W,H> const&, ftype const ); 

	// Multiplication
	template <int C>
	CMatrix<C,H> operator* ( CMatrix<C,W> const& ) const;
	//CMatrix<W,H> operator* ( CMatrix<W,H> const& );
	CMatrix<W,H> operator*=( CMatrix<W,H> const& );
	CMatrix<W,H> operator* ( ftype const ) const;

	CMatrix<W,H> operator+ ( CMatrix<W,H> const& ) const;
	CMatrix<W,H> operator+= ( CMatrix<W,H> const& );

	Vector3d operator* ( Vector3d const& ) const;
	Vector4d operator* ( Vector4d const& ) const;

	// Transpose
	CMatrix<H,W> operator! ( void ) const;

	// Accessor
	const ftype* operator[] ( int ) const;
	// Editor
	ftype* operator[] ( int );

public:
	// Data
	ftype pData [W*H];
};

// Typedefs
typedef CMatrix<4,4> Matrix4x4;
typedef CMatrix<2,2> Matrix2x2;
typedef CMatrix<3,3> Matrix3x3;

// Function definitions
#include "CMatrixFunctions.hpp"
#include "CMatrixFunctions2.hpp"
*/
#endif