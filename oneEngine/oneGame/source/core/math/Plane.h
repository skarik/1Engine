
#ifndef _C_PLANE_
#define _C_PLANE_

// Includes
#include "core/types/float.h"
#include "Vector3d.h"

// Class Def
namespace Maths
{
	class Plane
	{
	public:
		Vector3d n;
		Real d;

	public:
		// Constructors
		Plane ( void );
		Plane ( Vector3d * );
		void ConstructFromPoints ( Vector3d * );
		void ConstructFromPoints ( Vector3d const&, Vector3d const&, Vector3d const& );
	};
}

#endif