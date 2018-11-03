//
// Oriented Bounding Box
//
// Stored as a matrix( without scaling ) and Extents( x, y, z )
//
// Sourced from: http://www.3dkingdoms.com/weekly/weekly.php?a=21

#ifndef _BOUNDING_BOX_H_
#define _BOUNDING_BOX_H_

#include <algorithm>
#include "core/types/float.h"
#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"
#include "Matrix/CMatrix.h"

namespace core
{
	namespace math
	{
		class BoundingBox
		{
		public:
			BoundingBox() {}
			BoundingBox( const Matrix4x4 &M, const Vector3d &Extent ) 
				{ Set( M, Extent );	}
			// BL = Low values corner point, BH = High values corner point
			BoundingBox( const Matrix4x4 &M, const Vector3d &BL, const Vector3d &BH ) 
				{ Set( M, BL, BH );	}
	
			void Set( const Matrix4x4 &M, const Vector3d &Extent )
			{
			 m_M = M;
			 m_Extent = Extent;
			}	
			void Set( const Matrix4x4 &M, const Vector3d &BL, const Vector3d &BH )
			{
			 m_M = M;
			 m_M.translate( (BH + BL) * 0.5f );
			 m_Extent = (BH - BL) / 2.0f;
			}

			BoundingBox Expand ( const BoundingBox& BB );
	
			Vector3d GetSize() const 
				{ return m_Extent * 2.0f; }
			Vector3d GetCenterPoint() const
				{ return m_M.getTranslation(); }
				//{ return m_M.GetTranslate(); }		
	
			bool IsPointInBox( const Vector3d& P ) const;
			bool IsBoxInBox( BoundingBox &BBox ) const;
			bool IsSphereInBox( const Vector3d& P, float fRadius ) const;
			bool IsLineInBox( const Vector3d& L1, const Vector3d& L2 ) const;
			bool BoxOutsidePlane( const Vector3d &Norm, const Vector3d &P ) const;
	
		private:
			// Data
			Matrix4x4 m_M;
			Vector3d m_Extent;	
		};
	}
}

#endif