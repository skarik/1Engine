//===============================================================================================//
//
//		class BaseNoise
//
// Abstract base class for a noise generator.
//
//===============================================================================================//
#ifndef CORE_NOISE_BASE_H_
#define CORE_NOISE_BASE_H_

#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Vector4.h"

class BaseNoise
{
public:
	virtual ~BaseNoise ( void ) {}

	//	Get( pos1d ) : Sample the noise on 1D line.
	virtual float			Get (const float position) { return 0.0F; }

	//	Get( pos2d ) : Sample the noise on 2D plane.
	virtual float			Get (const Vector2f& position) { return 0.0F; }
	//	Get ( x, y ) : Sample the noise on 2D plane.
	float					Get (const float x, const float y)
		{ return Get(Vector2f(x, y)); }

	//	Get( pos3d ) : Sample the noise on 3D volume.
	virtual float			Get (const Vector3f& position) { return 0.0F; }
	//	Get( pos3d ) : Sample the noise on 3D volume.
	float					Get (const float x, const float y, const float z)
		{ return Get(Vector3f(x, y, z)); }

	//	Get( pos4d ) : Sample the noise on 4D hypervolume.
	virtual float			Get (const Vector4f& position) { return 0.0F; }
	//	Get( pos4d ) : Sample the noise on 4D hypervolume.
	float					Get (const float x, const float y, const float z, const float w)
		{ return Get(Vector4f(x, y, z, w)); }
};

#endif//CORE_NOISE_BASE_H_