// CCubic is a 3d primitive defined by its bottom back right corner and the distance from this corner.
// It has various collision detection functions beyond this.

#ifndef _C_CUBIC_
#define _C_CUBIC_

// Includes
#include "Vector3d.h"
#include "Line.h"
#include "Frustum.h"

// Class Definition
namespace Maths
{
	class Cubic
	{
	public:
		// Bottom back right corner
		Vector3d position;
		// Size of object, going forward x, left y, and up z
		Vector3d size;

	public:
		Cubic ( void );
		Cubic ( Vector3d const& vPosition, Vector3d const& vSize );
		//CCubic ( Vector3d const& vMinPos, Vector3d const& vMaxPos );
		static Cubic FromPosition ( Vector3d vMinPos, Vector3d vMaxPos );

		// Looks at the position and size, and moves their values around so that
		// the position refers to the bottom back right corner of the Cubic.
		void Realign( void );

		Vector3d center ( void ) const;
	public:
		void DrawDebug ( void );
	public:
		Frustum GetFrustum ( Vector3d const& casterPosition );
		bool PointIsInside ( Vector3d const& );
		bool LineCollides ( Line const& );
		bool LineGetCollision ( Line const&, Vector3d & );
	
		bool CubicCollides ( const Cubic & );
	private:
		// Helper functions for collision
		bool inline GetIntersection( ftype fDst1, ftype fDst2, Vector3d P1, Vector3d P2, Vector3d & Hit );
		bool inline InBox( Vector3d Hit, Vector3d B1, Vector3d B2, const char Axis );
	};
}

#endif