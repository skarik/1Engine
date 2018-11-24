
#ifndef _MATRIX_2_2_H_
#define _MATRIX_2_2_H_

class Matrix2x2
{
public:
	FORCE_INLINE Matrix2x2 ( void );
	FORCE_INLINE Matrix2x2 ( const Matrix2x2& );
	FORCE_INLINE Matrix2x2 ( const Real* );

	// Setters - Translation
	FORCE_INLINE bool setTranslation ( const Real = 0, const Real = 0 );
	FORCE_INLINE bool setTranslation ( const Vector2f& );

	// Setters - Scaling
	FORCE_INLINE bool setScale ( const Real = 0, const Real = 0 );
	FORCE_INLINE bool setScale ( const Vector2f& );

	// Setters - Rotation
	FORCE_INLINE bool setRotation ( const Real = 0 );

	// Modders
	FORCE_INLINE void translate ( const Vector2f& );
	FORCE_INLINE void scale ( const Vector2f& );

	// Getters - Translation
	FORCE_INLINE Vector2f	getTranslation ( void ) const;

	// Getters - Rotation
	FORCE_INLINE Real		getEulerAngle ( void ) const;

	// Getters - Scale
	FORCE_INLINE Vector2f	getScaling ( void ) const;

	// Operations
	FORCE_INLINE Matrix2x2 transpose ( void ) const;
	FORCE_INLINE Matrix2x2 inverse ( void ) const;
	FORCE_INLINE Real det ( void ) const;

	FORCE_INLINE Matrix2x2 LerpTo( Matrix2x2 const&, Real const ) const;
	FORCE_INLINE void Lerp ( Matrix2x2 const&, Real const ); 

	// Multiplication
	FORCE_INLINE Matrix2x2 operator* ( Matrix2x2 const& ) const;
	FORCE_INLINE Matrix2x2 operator*=( Matrix2x2 const& );
	FORCE_INLINE Matrix2x2 operator* ( Real const ) const;

	FORCE_INLINE Matrix2x2 operator+ ( Matrix2x2 const& ) const;
	FORCE_INLINE Matrix2x2 operator+=( Matrix2x2 const& );

	FORCE_INLINE Vector2f operator* ( Vector2f const& ) const;

	// Transpose
	FORCE_INLINE Matrix2x2 operator! ( void ) const;

	// Accessor
	FORCE_INLINE const Real* operator[] ( int ) const;
	// Editor
	FORCE_INLINE Real* operator[] ( int );

	//Equal comparison overload
	FORCE_INLINE bool operator== (Matrix2x2 const& right) const;

	//Not equal comparison overload
	FORCE_INLINE bool operator!= (Matrix2x2 const& right) const;

public:
	Real pData [4];
};

#endif//_MATRIX_2_2_H_