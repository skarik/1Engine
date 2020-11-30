// CCubic is a 3d primitive defined by its bottom back right corner and the distance from this corner.
// It has various collision detection functions beyond this.

#ifndef CORE_MATH_CUBIC_H_
#define CORE_MATH_CUBIC_H_

// Includes
#include "Vector3.h"
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
			Vector3f position;
			// Size of object, going forward x, left y, and up z
			Vector3f size;

		public:
			Cubic ( void );
			Cubic ( Vector3f const& vPosition, Vector3f const& vSize );

			//		FromPosition ( pos1, pos2 )
			// Creates a new Cubic from the input positions
			static Cubic FromPosition ( Vector3f vMinPos, Vector3f vMaxPos );

			//	ConstructCenterExtents ( center, extent ) : Creates cubic from given positions
			static Cubic ConstructCenterExtents ( const Vector3f& center, const Vector3f& extent )
			{
				return FromPosition(center - extent, center + extent);
			}

			//		Realign ()
			// Looks at the position and size, and moves their values around so that
			// the position refers to the bottom back right corner of the Cubic.
			void Realign ( void );

			Vector3f center ( void ) const;
		public:
			void DrawDebug ( void );
		public:
			Frustum GetFrustum ( Vector3f const& casterPosition );
			bool PointIsInside ( Vector3f const& );
			bool LineCollides ( Line const& );
			bool LineGetCollision ( Line const&, Vector3f & );
	
			bool CubicCollides ( const Cubic & );
		private:
			// Helper functions for collision
			bool inline GetIntersection( Real fDst1, Real fDst2, Vector3f P1, Vector3f P2, Vector3f & Hit );
			bool inline InBox( Vector3f Hit, Vector3f B1, Vector3f B2, const char Axis );
		};
	}
}
#endif//CORE_MATH_CUBIC_H_