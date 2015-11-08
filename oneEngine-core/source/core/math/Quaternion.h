#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "core/types/types.h"
#include "Vector3d.h"
#include <cmath>
#include <iostream>

// Forward declarataions
class Matrix4x4;
class Matrix3x3;

class Quaternion
{
	friend std::ostream& operator<< (std::ostream& out, Quaternion const & quat);		//Out stream overload

public:
	// Constructors
	FORCE_INLINE Quaternion( ); //Default constructor.
	FORCE_INLINE Quaternion( Quaternion const& old ); // Copy constructor.
	FORCE_INLINE Quaternion( Real const& new_x, Real const& new_y, Real const& new_z, Real const& new_w ); // Inpute conversion consturctor.
	FORCE_INLINE Quaternion( const Vector3d& euler_angles ); // Euler angles input constructor
	FORCE_INLINE Quaternion( const Matrix3x3& rotator );
	FORCE_INLINE Quaternion( const Matrix4x4& rotation );

	// Operations
	//Quaternion Invert(Quaternion const& qa) const; //Note: This also gives you the conjugate of a quarternion
	//ftype Magnitude(Quaternion const& qa) const; // returns a ftype magnitude of the quarternion.
	//void Normalize(Quaternion const& qa); // Normalizes the quaternion.
	FORCE_INLINE void Invert ( void );
	FORCE_INLINE Real Magnitude( void ) const;
	FORCE_INLINE void Normalize ( void );
	FORCE_INLINE Quaternion Multiply(Quaternion const& qa, Quaternion const& qb); // multiplies two quarternions together.

	// Dot product
	FORCE_INLINE Real Dot( Quaternion const& qa ) const;

	// Linear Interpolation (only valid for small differences in angle)
	FORCE_INLINE Quaternion Lerp ( Quaternion const& qa, const Real t );
	// Spherical Interpolation (works for large angle differences, but is much slower than lerp)
	FORCE_INLINE Quaternion Slerp ( Quaternion const& qa, const Real t ); 

	// Sets the quat to describing a rotation around an axis
	FORCE_INLINE void AxisAngle ( const Vector3d& axis, const Real angle );
	FORCE_INLINE static Quaternion CreateAxisAngle ( const Vector3d& axis, const Real angle );
	// Sets the quat to describe a rotation from one vector to another
	FORCE_INLINE void RotationTo ( const Vector3d& vfrom, const Vector3d& vto );
	FORCE_INLINE static Quaternion CreateRotationTo ( const Vector3d& vfrom, const Vector3d& vto );
	// Sets the quat to describing the given Euler XYZ rotation
	FORCE_INLINE void SetEulerAngles ( const Vector3d& euler_angles );
	FORCE_INLINE static Quaternion CreateFromEuler ( const Vector3d& euler_angles );
	// Sets the quat to describing the given rotation matrix
	//void SetFromRotator ( const Matrix3x3& rotator );
	// Swaps the basis of the quaternion (you should never need to use this)
	Quaternion& SwitchBasis ( void );

	// Rotates a vector
	FORCE_INLINE Vector3d operator* ( Vector3d const& right ) const;	
	// Rotates against another quat
	FORCE_INLINE Quaternion operator* ( Quaternion const& right ) const;
	// Inverts the quaternion
	FORCE_INLINE Quaternion operator! ( void ) const;

	// Scales the quaternion
	FORCE_INLINE const Quaternion operator* ( const float scale ) const;

	// Gets euler angles from the quaternion
	Vector3d GetEulerAngles ( void ) const;

	//Equal comparison overload
	FORCE_INLINE bool operator== (Quaternion const& right) const;

	//Not equal comparison overload
	FORCE_INLINE bool operator!= (Quaternion const& right) const;

	// Returns the private values for direct read
	FORCE_INLINE const Real X ( void ) { return x; };
	FORCE_INLINE const Real Y ( void ) { return y; };
	FORCE_INLINE const Real Z ( void ) { return z; };
	FORCE_INLINE const Real W ( void ) { return w; };

public:
	Real x;
	Real y;
	Real z;
	Real w;

};


//===============================================================================================//
// Quaternion implementation
//===============================================================================================//
#include "core/math/Matrix/CMatrix.h"

// Constructors
FORCE_INLINE Quaternion::Quaternion()
	: x(0.0), y(0.0), z(0.0), w(1.0) //Default constructor.
{
	;
}
FORCE_INLINE Quaternion::Quaternion(Quaternion const& old)
	: x(old.x), y(old.y), z(old.z), w(old.w) // Copy class.
{
	;
}
FORCE_INLINE Quaternion::Quaternion(Real const& new_x, Real const& new_y, Real const& new_z, Real const& new_w)
	: x(new_x), y(new_y), z(new_z), w(new_w) // Inpute conversion consturctor.
{
	;
}
FORCE_INLINE Quaternion::Quaternion( const Vector3d& euler_angles ) // Euler angles constructor
{
	SetEulerAngles( euler_angles );
}
FORCE_INLINE Quaternion::Quaternion( const Matrix3x3& rotator ) // 3x3 rotator
{
	*this = rotator.getQuaternion();
}
FORCE_INLINE Quaternion::Quaternion( const Matrix4x4& rotation ) // 4x4 rotation
{
	*this = rotation.getQuaternion();
}

FORCE_INLINE void Quaternion::Invert ( void )
{
	x = -x;
	y = -y;
	z = -z;
}

FORCE_INLINE Real Quaternion::Magnitude( void ) const // returns a ftype magnitude of the quarternion.
{
	return sqrt( sqr(w) + sqr(x) + sqr(y) + sqr(z) );
}
FORCE_INLINE void Quaternion::Normalize( void ) // Normalizes the quaternion.
{
	Real magnitude = 1/Magnitude();
	x *= magnitude;
	y *= magnitude;
	z *= magnitude;
	w *= magnitude;
}

FORCE_INLINE Quaternion Quaternion::Multiply(Quaternion const& qa, Quaternion const& qb) // multiplies two quarternions together.
{
	Quaternion qr;

	/*qr.w = qa.w*qb.w - qa.x*qb.x - qa.y*qb.y - qa.z*qb.z;
	qr.x = qa.w*qb.x + qa.x*qb.w + qa.y*qb.z - qa.z*qb.y;
	qr.y = qa.w*qb.y + qa.y*qb.w + qa.z*qb.x - qa.x*qb.z;
	qr.z = qa.w*qb.z + qa.z*qb.w + qa.x*qb.y - qa.y*qb.x;
	*/

	qr.w = qa.w*qb.w - qa.x*qb.x - qa.y*qb.y - qa.z*qb.z;
	qr.x = qa.w*qb.x + qa.x*qb.w - qa.y*qb.z + qa.z*qb.y;
	qr.y = qa.w*qb.y + qa.x*qb.z + qa.y*qb.w - qa.z*qb.x;
	qr.z = qa.w*qb.z - qa.x*qb.y + qa.y*qb.x + qa.z*qb.w;

	return qr;
}
FORCE_INLINE Quaternion Quaternion::operator* ( Quaternion const& right ) const
{
	//return this->Multiply(*this,right);
	Quaternion qr;

	qr.w = this->w*right.w - this->x*right.x - this->y*right.y - this->z*right.z;
	qr.x = this->w*right.x + this->x*right.w + this->y*right.z - this->z*right.y;
	qr.y = this->w*right.y + this->y*right.w + this->z*right.x - this->x*right.z;
	qr.z = this->w*right.z + this->z*right.w + this->x*right.y - this->y*right.x;

	return qr;
}
FORCE_INLINE Quaternion Quaternion::operator! ( void ) const
{
	Quaternion quat( *this );
	quat.Invert();
	return quat;
	//return this->Invert(*this);
}
FORCE_INLINE const Quaternion Quaternion::operator* ( const float scale ) const
{
	return Quaternion( x*scale, y*scale, z*scale, w*scale );
}

// Dot product
FORCE_INLINE Real Quaternion::Dot( Quaternion const& qa ) const
{
	return x * qa.x + y * qa.y + z * qa.z + w * qa.w;
}

// Sets the quat to describing a rotation around an axis
FORCE_INLINE void Quaternion::AxisAngle ( const Vector3d& axis, const Real angle )
{
	Vector3d Axis = axis.normal();
	Real sourceAngle = (Real) degtorad(angle) / 2.0f;
    Real sin_a = sin( sourceAngle );
    x    = Axis.x * sin_a;
    y    = Axis.y * sin_a;
    z    = Axis.z * sin_a;
    w    = cos( sourceAngle );

	Normalize();
}
FORCE_INLINE Quaternion Quaternion::CreateAxisAngle ( const Vector3d& axis, const Real angle )
{
	Quaternion q;
	q.AxisAngle( axis, angle );
	return q;
}


// Sets the quat to describe a rotation from one vector to another
FORCE_INLINE void Quaternion::RotationTo ( const Vector3d& vfrom, const Vector3d& vto )
{
	// Based on Stan Melax's article in Game Programming Gems
    Quaternion q;

    // Copy, since cannot modify local
    Vector3d v0 = vfrom;
    Vector3d v1 = vto;
    v0.normalize();
    v1.normalize();

    Real d = v0.dot(v1);
    // If dot == 1, vectors are the same
    if (d >= 1.0f)
    {
        //return Quaternion();
		q = Quaternion();
    }
    else if (d < (1e-6f - 1.0f))
    {
        // Generate an axis
		Vector3d axis = Vector3d::forward.cross(vfrom);
        if (axis.sqrMagnitude() < 0.0001f) // pick another if colinear
			axis = Vector3d::left.cross(vfrom);
        axis.normalize();
		q.AxisAngle( axis, radtodeg(PI) );
    }
    else
    {
        Real s = sqrt( (1+d)*2 );
		Real invs = 1 / s;

		Vector3d c = v0.cross(v1);

        q.x = c.x * invs;
        q.y = c.y * invs;
        q.z = c.z * invs;
        q.w = s * 0.5f;
    }

	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;

	Normalize();
}

FORCE_INLINE Quaternion Quaternion::CreateRotationTo ( const Vector3d& vfrom, const Vector3d& vto )
{
	Quaternion temp;
	temp.RotationTo( vfrom, vto );
	return temp;
}

// Sets the quat to describing the given Euler XYZ rotation
FORCE_INLINE void Quaternion::SetEulerAngles ( const Vector3d& euler_angles )
{
	{
		/*(Vector3d vx ( 1, 0, 0 ), vy ( 0, 1, 0 ), vz ( 0, 0, 1 );
		Quaternion qx, qy, qz, qt;
		qx.AxisAngle( vx, euler_angles.x );
		qy.AxisAngle( vy, euler_angles.y );
		qz.AxisAngle( vz, euler_angles.z );
		qt = qz * qy * qx;
		(*this) = qt;*/
	}

	{
		Real rotx = (Real)degtorad(euler_angles.x);
		Real roty = (Real)degtorad(euler_angles.y);
		Real rotz = (Real)degtorad(euler_angles.z);
		Quaternion qx ( sin(rotx/2),0,0,	cos(rotx/2) );
		Quaternion qy ( 0,sin(roty/2),0,	cos(roty/2) );
		Quaternion qz ( 0,0,sin(rotz/2),	cos(rotz/2) );
		//Quaternion qt = qx;
		//qt = qy*qt;
		//qt = qz*qt;
		//Quaternion qt = qx * qy * qz;
		Quaternion qt = qz * qy * qx;
		(*this) = qt;
	}

	{
		// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
		// and multiply those together.
		// the calculation below does the same, just shorter
 
		/*float p = (ftype) degtorad( euler_angles.x ) * 0.5f;
		float y = (ftype) degtorad( euler_angles.y ) * 0.5f;
		float r = (ftype) degtorad( euler_angles.z ) * 0.5f;
 
		float sinp = sin(p);
		float siny = sin(y);
		float sinr = sin(r);
		float cosp = cos(p);
		float cosy = cos(y);
		float cosr = cos(r);
 
		this->x = sinr * cosp * cosy - cosr * sinp * siny;
		this->y = cosr * sinp * cosy + sinr * cosp * siny;
		this->z = cosr * cosp * siny - sinr * sinp * cosy;
		this->w = cosr * cosp * cosy + sinr * sinp * siny;
 
		Normalize();*/
	}
}

// Rotates a vector
FORCE_INLINE Vector3d Quaternion::operator* ( Vector3d const& right ) const
{
	Quaternion qr = *this;
	Quaternion qri= !(*this);
	Quaternion v ( right.x,right.y,right.z,0 ); 
	Quaternion qf = qr*v*qri;
	return Vector3d( qf.x, qf.y, qf.z );
}

// Linear Interpolation (only valid for small differences in angle)
FORCE_INLINE Quaternion Quaternion::Lerp ( Quaternion const& qa, const Real t )
{
	if (t <= 0)
	{
		return (*this);
	}
	else if(t >= 1)
	{
		return qa;
	}
	else
	{
		return Quaternion(
			( qa.x - this->x )*t + this->x,
			( qa.y - this->y )*t + this->y,
			( qa.z - this->z )*t + this->z,
			( qa.w - this->w )*t + this->w
			);
	}
}

// Spherical Interpolation (works for large angle differences, but is much slower than lerp)
FORCE_INLINE Quaternion Quaternion::Slerp ( Quaternion const& qa, const Real t )
{
	if (t <= 0)
	{
		return (*this);
	}
	else if(t >= 1)
	{
		return qa;
	}
	else
	{
		Quaternion q3;
		Real dot = this->Dot( qa );
		
		if (dot < 0)
		{
			dot = -dot;
			q3 = Quaternion( -qa.x, -qa.y, -qa.z, -qa.w );
		}
		else
		{
			q3 = qa;
		}

		float angle = acosf(dot);
		if ( !VALID_FLOAT(angle) ) {
			return (*this);
		}

		Real a = sinf(angle*(1-t));
		Real b = sinf(angle*t);
		Real c = sinf(angle);
		if ( c < FTYPE_PRECISION ) {
			return (*this);
		}

		return Quaternion(
			( x*a + q3.x*b )/c,
			( y*a + q3.y*b )/c,
			( z*a + q3.z*b )/c,
			( w*a + q3.w*b )/c
			);
	}
}

//Equal comparison overload
FORCE_INLINE bool Quaternion::operator== (Quaternion const& right) const
{
	if (fabs(right.x - x) > FTYPE_PRECISION)
		return false;
	if (fabs(right.y - y) > FTYPE_PRECISION)
		return false;
	if (fabs(right.z - z) > FTYPE_PRECISION)
		return false;
	if (fabs(right.w - w) > FTYPE_PRECISION)
		return false;
	return true;
}

//Not equal comparison overload
FORCE_INLINE bool Quaternion::operator!= (Quaternion const& right) const
{
	return !((*this) == right);
}

#endif//_QUATERNION_H_