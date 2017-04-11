
#ifndef _MATRIX_3_3_H_
#define _MATRIX_3_3_H_

class Matrix3x3
{//todo: alignment
public:
	FORCE_INLINE Matrix3x3 ( void );
	FORCE_INLINE Matrix3x3 ( const Matrix3x3& );
	FORCE_INLINE Matrix3x3 ( const ftype* );

	// Setters - Scaling
	FORCE_INLINE void setScale ( const ftype = 0, const ftype = 0, const ftype = 0 );
	FORCE_INLINE void setScale ( const Vector3d& );

	// Setters - Rotation
	FORCE_INLINE void setRotation ( const ftype = 0, const ftype = 0, const ftype = 0 );
	FORCE_INLINE void setRotation ( const Vector3d& );
	FORCE_INLINE void setRotation ( const Quaternion& );
	FORCE_INLINE void setRotation ( const Matrix3x3& );

	FORCE_INLINE void setRotationZYX ( const ftype = 0, const ftype = 0, const ftype = 0 );

	// Getters - Rotation
	FORCE_INLINE Vector3d	getEulerAngles ( void ) const;
	FORCE_INLINE Quaternion	getQuaternion ( void ) const;

	// Getters - Scale
	FORCE_INLINE Vector3d	getScaling ( void ) const;

	// Operations
	FORCE_INLINE Matrix3x3 transpose ( void ) const;
	FORCE_INLINE Matrix3x3 inverse ( void ) const;
	FORCE_INLINE ftype det ( void ) const;
	
	FORCE_INLINE Matrix2x2 submatrix ( int, int ) const;

	FORCE_INLINE Matrix3x3 LerpTo( Matrix3x3 const&, ftype const ) const;
	FORCE_INLINE void Lerp ( Matrix3x3 const&, ftype const ); 

	// Multiplication
	FORCE_INLINE Matrix3x3 operator* ( Matrix3x3 const& ) const;
	FORCE_INLINE Matrix3x3 operator*=( Matrix3x3 const& );
	FORCE_INLINE Matrix3x3 operator* ( ftype const ) const;

	FORCE_INLINE Matrix3x3 operator+ ( Matrix3x3 const& ) const;
	FORCE_INLINE Matrix3x3 operator+=( Matrix3x3 const& );

	FORCE_INLINE Vector3d operator* ( Vector3d const& ) const;

	// Transpose
	FORCE_INLINE Matrix3x3 operator! ( void ) const;

	// Accessor
	FORCE_INLINE const ftype* operator[] ( int ) const;
	// Editor
	FORCE_INLINE ftype* operator[] ( int );

	//Equal comparison overload
	FORCE_INLINE bool operator== (Matrix3x3 const& right) const;

	//Not equal comparison overload
	FORCE_INLINE bool operator!= (Matrix3x3 const& right) const;

	// Check for valid values
	FORCE_INLINE bool isOk ( void );
public:
	ftype pData [9];
};

#endif//_MATRIX_3_3_H_