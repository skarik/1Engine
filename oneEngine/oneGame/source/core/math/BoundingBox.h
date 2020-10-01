//===============================================================================================//
//
// Oriented Bounding Box
//
// Stored as a matrix( without scaling ) and Extents( x, y, z )
// Sourced from: http://www.3dkingdoms.com/weekly/weekly.php?a=21
//
//===============================================================================================//
#ifndef CORE_MATH_BOUNDING_BOX_H_
#define CORE_MATH_BOUNDING_BOX_H_

#include <algorithm>
#include "core/types/float.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix/CMatrix.h"
#include "Plane.h"
#include "Ray.h"
#include "Math.h"
#include "vect3d_template.h"

namespace core
{
	namespace math
	{
		class BoundingBox
		{
		public:
			BoundingBox()
				{}
			BoundingBox( const Matrix4x4& M, const Vector3f& Extent ) 
				{ Set( M, Extent );	}
			BoundingBox( const Matrix4x4& M, const Vector3f& BL, const Vector3f& BH ) 
				{ Set( M, BL, BH );	}
	
			FORCE_INLINE void Set( const Matrix4x4& M, const Vector3f& Extent )
			{
				m_M = M;
				m_Extent = Vector3f(std::abs(Extent.x), std::abs(Extent.y), std::abs(Extent.z));

				m_MInverse = m_M.inverse();
				//m_Extent.w = std::sqrt(m_Extent.x * m_Extent.x + m_Extent.y * m_Extent.y + m_Extent.z * m_Extent.z);
			}

			FORCE_INLINE void Set( const Matrix4x4& M, const Vector3f& BL, const Vector3f& BH )
			{
				// Need to ensure each component is properly sorted:
				Vector3f l_actualLower = Vector3f(std::min(BL.x, BH.x), std::min(BL.y, BH.y), std::min(BL.z, BH.z));
				Vector3f l_actualHigher = Vector3f(std::max(BL.x, BH.x), std::max(BL.y, BH.y), std::max(BL.z, BH.z));

				m_M = M;
				m_M.translate( (l_actualHigher + l_actualLower) * 0.5f ); // Move by midpoint
				m_Extent = (l_actualHigher - l_actualLower) / 2.0f; // Properly set extents

				m_MInverse = m_M.inverse();
				//m_Extent.w = std::sqrt(m_Extent.x * m_Extent.x + m_Extent.y * m_Extent.y + m_Extent.z * m_Extent.z);
			}

			FORCE_INLINE void SetRotation ( const Matrix3x3& rotator )
			{
				m_M.setRotation(rotator);
				m_MInverse.setRotation(rotator.inverse());
			}

			//	Expand ( BoundingBox& )
			// Combines the hulls of two bounding boxes by creating another bounding box that encompasses both of them
			// Note that this only really works properly if both bounding boxes have only been transformed through a translation.
			FORCE_INLINE BoundingBox Expand ( const BoundingBox& BB )
			{
				Vector3f minPosA, maxPosA;
				Vector3f minPosB, maxPosB;
				Vector3f curCenter;

				// Calculate min and max pos for first bbox
				curCenter = GetCenterPoint();
				minPosA = (-m_Extent)+curCenter;
				maxPosA = ( m_Extent)+curCenter;
				// Calculate min and max pos for second bbox
				curCenter = BB.GetCenterPoint();
				minPosB = (-BB.m_Extent)+curCenter;
				maxPosB = ( BB.m_Extent)+curCenter;

				// Grab the largest area
				minPosA.x = std::min<Real>( minPosA.x, minPosB.x );
				minPosA.y = std::min<Real>( minPosA.y, minPosB.y );
				minPosA.z = std::min<Real>( minPosA.z, minPosB.z );
				maxPosA.x = std::max<Real>( maxPosA.x, maxPosB.x );
				maxPosA.y = std::max<Real>( maxPosA.y, maxPosB.y );
				maxPosA.z = std::max<Real>( maxPosA.z, maxPosB.z );

				// Return new bounding box
				return BoundingBox( Matrix4x4(), minPosA, maxPosA );
			}
	
			FORCE_INLINE Vector3f GetSize() const 
				{ return m_Extent * 2.0f; }

			FORCE_INLINE Vector3f GetCenterPoint() const
				{ return m_M.getTranslation(); }
	
			//	IsPointInBox( point ) : Check if a point is in this bounding box
			FORCE_INLINE bool IsPointInBox( const Vector3f& InP ) const
			{
				// Rotate the point into the box's coordinates
				Vector3f P = m_MInverse * InP;

				// Now just use an axis-aligned check
				if ( fabs(P.x) < m_Extent.x && fabs(P.y) < m_Extent.y && fabs(P.z) < m_Extent.z ) 
					return true;

				return false;
			}

			// Check if any part of a box is inside any part of another box
			// Uses the separating axis test.
			FORCE_INLINE bool IsBoxInBox( BoundingBox &BBox ) const
			{
				Vector3f SizeA = m_Extent;
				Vector3f SizeB = BBox.m_Extent;
				Matrix3x3 matxA = m_M.getRotator();
				Matrix3x3 matxB = BBox.m_M.getRotator();
				Vector3f* RotA[3] = { (Vector3f*)matxA[0], (Vector3f*)matxA[1], (Vector3f*)matxA[2] };
				Vector3f* RotB[3] = { (Vector3f*)matxB[0], (Vector3f*)matxB[1], (Vector3f*)matxB[2] };

				Real R[3][3];  // Rotation from B to A
				Real AR[3][3]; // absolute values of R matrix, to use with box extents
				Real ExtentA, ExtentB, Separation;
				int i, k;

				// Calculate B to A rotation matrix
				for( i = 0; i < 3; i++ )
				{
					for( k = 0; k < 3; k++ )
					{
						R[i][k] = RotA[i]->dot(*RotB[k]); 
						AR[i][k] = fabs(R[i][k]);
					}
				}

				// Vector separating the centers of Box B and of Box A	
				Vector3f vSepWS = BBox.GetCenterPoint() - GetCenterPoint();
				// Rotated into Box A's coordinates
				Vector3f vSepA( vSepWS.dot(*RotA[0]), vSepWS.dot(*RotA[1]), vSepWS.dot(*RotA[2]) );            

				// Test if any of A's basis vectors separate the box
				for( i = 0; i < 3; i++ )
				{
					ExtentA = SizeA[i];
					ExtentB = SizeB.dot( Vector3f( AR[i][0], AR[i][1], AR[i][2] ) );
					Separation = fabs( vSepA[i] );

					if( Separation > ExtentA + ExtentB ) return false;
				}

				// Test if any of B's basis vectors separate the box
				for( k = 0; k < 3; k++ )
				{
					ExtentA = SizeA.dot( Vector3f( AR[0][k], AR[1][k], AR[2][k] ) );
					ExtentB = SizeB[k];
					Separation = fabs( vSepA.dot( Vector3f(R[0][k],R[1][k],R[2][k]) ) );

					if( Separation > ExtentA + ExtentB ) return false;
				}

				// Now test Cross Products of each basis vector combination ( A[i], B[k] )
				for( i=0 ; i<3 ; i++ )
				{
					for( k=0 ; k<3 ; k++ )
					{
						int i1 = (i+1)%3, i2 = (i+2)%3;
						int k1 = (k+1)%3, k2 = (k+2)%3;
						ExtentA = SizeA[i1] * AR[i2][k]  +  SizeA[i2] * AR[i1][k];
						ExtentB = SizeB[k1] * AR[i][k2]  +  SizeB[k2] * AR[i][k1];
						Separation = fabs( vSepA[i2] * R[i1][k]  -  vSepA[i1] * R[i2][k] );
						if( Separation > ExtentA + ExtentB )
							return false;
					}
				}

				// No separating axis found, the boxes overlap	
				return true;
			}

			//	IsSphereInBox( center, radius ) : Check if a sphere overlaps any part of this bounding box
			FORCE_INLINE bool IsSphereInBox( const Vector3f& InP, float fRadius ) const
			{
				Real fDist;
				Real fDistSq = 0;
				Vector3f P = m_MInverse * InP;

				// Add distance squared from sphere centerpoint to box for each axis
				for ( int i = 0; i < 3; i++ )
				{
					if ( fabs(P[i]) > m_Extent[i] )
					{
						fDist = fabs(P[i]) - m_Extent[i];
						fDistSq += fDist*fDist;
					}
				}
				return ( fDistSq <= fRadius*fRadius );
			}

			//	IsLineInBox( lineStart, lineEnd ) : Does the Line (L1, L2) intersect the Box?
			FORCE_INLINE bool IsLineInBox( const Vector3f& L1, const Vector3f& L2 ) const
			{	
				// Put line in box space
				Matrix4x4 MInv = m_MInverse;
				Vector3f LB1 = MInv * L1;
				Vector3f LB2 = MInv * L2;

				// Get line midpoint and extent
				Vector3f LMid = (LB1 + LB2) * 0.5f; 
				Vector3f L = (LB1 - LMid);
				Vector3f LExt = Vector3f( fabs(L.x), fabs(L.y), fabs(L.z) );

				// Use Separating Axis Test
				// Separation vector from box center to line center is LMid, since the line is in box space
				if ( std::abs( LMid.x ) > m_Extent.x + LExt.x ) return false;
				if ( std::abs( LMid.y ) > m_Extent.y + LExt.y ) return false;
				if ( std::abs( LMid.z ) > m_Extent.z + LExt.z ) return false;
				// Crossproducts of line and each axis
				if ( std::abs( LMid.y * L.z - LMid.z * L.y)  >  (m_Extent.y * LExt.z + m_Extent.z * LExt.y) ) return false;
				if ( std::abs( LMid.x * L.z - LMid.z * L.x)  >  (m_Extent.x * LExt.z + m_Extent.z * LExt.x) ) return false;
				if ( std::abs( LMid.x * L.y - LMid.y * L.x)  >  (m_Extent.x * LExt.y + m_Extent.y * LExt.x) ) return false;

				// No separating axis, the line intersects
				return true;
			}

			//	Raycast( ray ) : Checks if ray hits the box
			FORCE_INLINE bool Raycast( const Ray& ray ) const
			{
				Real unused_distance;
				return Raycast(ray, unused_distance);
			}

			//	Raycast( ray, out_distance ) : Checks if ray hits the box, sets distance to the box if so.
			FORCE_INLINE bool Raycast( const Ray& ray, Real& out_distance ) const
			{
				/*Real tmin, tmax, tymin, tymax, tzmin, tzmax;
				Vector3f bounds [2] = {-m_Extent, m_Extent};
				Vector3f ray_inverse_dir = Vector3f(1 / ray.dir.x, 1 / ray.dir.y, 1 / ray.dir.z);

				int sign[3] = {::math::sgn(ray.dir.x), ::math::sgn(ray.dir.y), ::math::sgn(ray.dir.z)};

				tmin = (bounds[sign[0]].x - ray.pos.x) * ray_inverse_dir.x;
				tmax = (bounds[1-sign[0]].x - ray.pos.x) * ray_inverse_dir.x;
				tymin = (bounds[sign[1]].y - ray.pos.y) * ray_inverse_dir.y;
				tymax = (bounds[1-sign[1]].y - ray.pos.y) * ray_inverse_dir.y;

				if ((tmin > tymax) || (tymin > tmax))
					return false;
				if (tymin > tmin)
					tmin = tymin;
				if (tymax < tmax)
					tmax = tymax;

				tzmin = (bounds[sign[2]].z - ray.pos.z) * ray_inverse_dir.z;
				tzmax = (bounds[1-sign[2]].z - ray.pos.z) * ray_inverse_dir.z;

				if ((tmin > tzmax) || (tzmin > tmax))
					return false;
				if (tzmin > tmin)
					tmin = tzmin;
				if (tzmax < tmax)
					tmax = tzmax;

				return true;*/

				// Implementation borrowed from http://www.jcgt.org/published/0007/03/04/paper-lowres.pdf

				Ray l_ray = Ray(m_MInverse * ray.pos, m_MInverse.getRotator() * ray.dir);
				Vector3i l_sign = Vector3i(::math::sgn(l_ray.dir.x), ::math::sgn(l_ray.dir.y), ::math::sgn(l_ray.dir.z));

				// Distance to plane
				Vector3f l_d = l_ray.pos - Vector3f((Real32)l_sign.x, (Real32)l_sign.y, (Real32)l_sign.z).mulComponents(m_Extent);
				l_d = l_d.divComponents(l_ray.dir);

				// Test all axes at once
				bool l_test [3] = {
					(l_d.x > 0.0) && (std::abs(l_ray.pos.y + l_ray.dir.y * l_d.x) < m_Extent.y) && (std::abs(l_ray.pos.z + l_ray.dir.z * l_d.x) < m_Extent.z),
					(l_d.y > 0.0) && (std::abs(l_ray.pos.z + l_ray.dir.z * l_d.y) < m_Extent.z) && (std::abs(l_ray.pos.x + l_ray.dir.x * l_d.y) < m_Extent.x),
					(l_d.z > 0.0) && (std::abs(l_ray.pos.x + l_ray.dir.x * l_d.z) < m_Extent.x) && (std::abs(l_ray.pos.y + l_ray.dir.y * l_d.z) < m_Extent.y)
				};

				l_sign[0] = l_test[0] ? l_sign[0] : 0;
				l_sign[1] = l_test[1] ? l_sign[1] : 0;
				l_sign[2] = l_test[2] ? l_sign[2] : 0;

				// Get distance
				out_distance = (l_sign.x != 0) ? l_d.x : ((l_sign.y != 0) ? l_d.y : l_d.z);
				//out_normal = m_M.getRotator() * Vector3f(l_sign.x, l_sign.y, l_sign.z);

				// Return if hit.
				return (l_sign.x != 0) || (l_sign.y != 0) || (l_sign.z != 0);
			}

			//	BoxOutsidePlane( plane ) : Check if the bounding box is completely behind a plane
			FORCE_INLINE bool BoxOutsidePlane( const core::math::Plane& plane ) const
			{
				return BoxOutsidePlane(plane.n, Vector3f(0, 0, -plane.d) );
			}

			//	BoxOutsidePlane( normal, point ) : Check if the bounding box is completely behind a plane
			FORCE_INLINE bool BoxOutsidePlane( const Vector3f &InNorm, const Vector3f &InP ) const
			{
				// Plane Normal in Box Space
				//Vector3f Norm = InNorm.RotByMatrix( m_M.InvertSimple().mf ); // RotByMatrix only uses rotation portion of matrix

				//Matrix4x4 rotMatrix;
				//rotMatrix.setRotation( m_M.getEulerAngles() );
				//Vector3f Norm = rotMatrix.inverse() * InNorm;

				Matrix3x3 rotMatrixInverse = m_MInverse.getRotator();
				Vector3f Norm = rotMatrixInverse * InNorm;

				Norm = Vector3f( fabs( Norm.x ), fabs( Norm.y ), fabs( Norm.z ) );

				//cout << GetCenterPoint();

				Real Extent = Norm.dot( m_Extent ); // Box Extent along the plane normal
				Real Distance = InNorm.dot( GetCenterPoint() - InP ); // Distance from Box Center to the Plane

																	   // If Box Centerpoint is behind the plane further than its extent, the Box is outside the plane
				if ( Distance < -Extent )
					return true;
				return false;
			}
	
		public:
			// Extents of the BBox. Stores: X Y Z Unused.
			Vector4f		m_Extent;
			Matrix4x4		m_M;
			Matrix4x4		m_MInverse;
		};
	}
}

#endif//CORE_MATH_BOUNDING_BOX_H_