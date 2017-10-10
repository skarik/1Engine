// CCubic is a 3d primitive defined by its bottom back right corner and the distance from this corner.
// It has various collision detection functions beyond this.

#ifndef CORE_MATH_CUBIC_H_
#define CORE_MATH_CUBIC_H_

// Includes
#include "Vector3d.h"
#include "Line.h"
#include "Frustum.h"

namespace core
{
	namespace math
	{
		// Class Definition
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

			//		FromPosition ( pos1, pos2 )
			// Creates a new Cubic from the input positions
			static Cubic FromPosition ( Vector3d vMinPos, Vector3d vMaxPos );

			//		Realign ()
			// Looks at the position and size, and moves their values around so that
			// the position refers to the bottom back right corner of the Cubic.
			void Realign ( void );

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
			bool inline GetIntersection( Real fDst1, Real fDst2, Vector3d P1, Vector3d P2, Vector3d & Hit );
			bool inline InBox( Vector3d Hit, Vector3d B1, Vector3d B2, const char Axis );
		};
	}
}
#endif//CORE_MATH_CUBIC_H_