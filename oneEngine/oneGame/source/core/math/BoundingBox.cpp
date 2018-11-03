// --------------------------
//
// Oriented Bounding Box Class
//
// --------------------------
#include "BoundingBox.h"

using namespace core::math;

// Expand ( BoundingBox& )
// Combines the hulls of two bounding boxes by creating another bounding box that encompasses both of them
// Note that this only really works proper if both bounding boxes have only been transformed through a translation.
BoundingBox BoundingBox::Expand ( const BoundingBox& BB )
{
	Vector3d minPosA, maxPosA;
	Vector3d minPosB, maxPosB;
	Vector3d curCenter;
	
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

//
// Check if a point is in this bounding box
//
bool BoundingBox::IsPointInBox(const Vector3d &InP) const
{
	// Rotate the point into the box's coordinates
	Vector3d P = m_M.inverse() * InP;
	
	// Now just use an axis-aligned check
	if ( fabs(P.x) < m_Extent.x && fabs(P.y) < m_Extent.y && fabs(P.z) < m_Extent.z ) 
		return true;
		
	return false;
}

//
// Check if a sphere overlaps any part of this bounding box
//
bool BoundingBox::IsSphereInBox( const Vector3d &InP, float fRadius) const
{
	float fDist;
	float fDistSq = 0;
	//Vector3d P = m_M.InvertSimple() * InP;
	Vector3d P = m_M.inverse() * InP;
	
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

//
// Check if the bounding box is completely behind a plane( defined by a normal and a point )
//
bool BoundingBox::BoxOutsidePlane( const Vector3d &InNorm, const Vector3d &InP ) const
{
	// Plane Normal in Box Space
	//Vector3d Norm = InNorm.RotByMatrix( m_M.InvertSimple().mf ); // RotByMatrix only uses rotation portion of matrix
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( m_M.getEulerAngles() );
	Vector3d Norm = rotMatrix.inverse() * InNorm;

	Norm = Vector3d( fabs( Norm.x ), fabs( Norm.y ), fabs( Norm.z ) );

	//cout << GetCenterPoint();
	
	float Extent = Norm.dot( m_Extent ); // Box Extent along the plane normal
	float Distance = InNorm.dot( GetCenterPoint() - InP ); // Distance from Box Center to the Plane

	// If Box Centerpoint is behind the plane further than its extent, the Box is outside the plane
	if ( Distance < -Extent )
		return true;
	return false;
}

//
// Does the Line (L1, L2) intersect the Box?
//
bool BoundingBox::IsLineInBox( const Vector3d& L1, const Vector3d& L2 ) const
{	
	// Put line in box space
	Matrix4x4 MInv = m_M.inverse();
	Vector3d LB1 = MInv * L1;
	Vector3d LB2 = MInv * L2;

	// Get line midpoint and extent
	Vector3d LMid = (LB1 + LB2) * 0.5f; 
	Vector3d L = (LB1 - LMid);
	Vector3d LExt = Vector3d( fabs(L.x), fabs(L.y), fabs(L.z) );

	// Use Separating Axis Test
	// Separation vector from box center to line center is LMid, since the line is in box space
	if ( fabs( LMid.x ) > m_Extent.x + LExt.x ) return false;
	if ( fabs( LMid.y ) > m_Extent.y + LExt.y ) return false;
	if ( fabs( LMid.z ) > m_Extent.z + LExt.z ) return false;
	// Crossproducts of line and each axis
	if ( fabs( LMid.y * L.z - LMid.z * L.y)  >  (m_Extent.y * LExt.z + m_Extent.z * LExt.y) ) return false;
	if ( fabs( LMid.x * L.z - LMid.z * L.x)  >  (m_Extent.x * LExt.z + m_Extent.z * LExt.x) ) return false;
	if ( fabs( LMid.x * L.y - LMid.y * L.x)  >  (m_Extent.x * LExt.y + m_Extent.y * LExt.x) ) return false;
	// No separating axis, the line intersects
	return true;
}

//
// Returns a 3x3 rotation matrix as vectors
//
inline void Get3x3Rot( const Matrix4x4& matrix, Vector3d* outRotation )
{
	outRotation[0] = Vector3d( matrix.pData[0], matrix.pData[1], matrix.pData[2] );
	outRotation[1] = Vector3d( matrix.pData[4], matrix.pData[5], matrix.pData[6] );
	outRotation[2] = Vector3d( matrix.pData[8], matrix.pData[9], matrix.pData[10] );
}

//
// Check if any part of a box is inside any part of another box
// Uses the separating axis test.
//
bool BoundingBox::IsBoxInBox( BoundingBox &BBox ) const
{
	Vector3d SizeA = m_Extent;
	Vector3d SizeB = BBox.m_Extent;
	Vector3d RotA[3], RotB[3];	
	Get3x3Rot( m_M, RotA );
	Get3x3Rot( BBox.m_M, RotB );

    float R[3][3];  // Rotation from B to A
    float AR[3][3]; // absolute values of R matrix, to use with box extents
    float ExtentA, ExtentB, Separation;
    int i, k;

    // Calculate B to A rotation matrix
    for( i = 0; i < 3; i++ )
	{
        for( k = 0; k < 3; k++ )
		{
			R[i][k] = RotA[i].dot(RotB[k]); 
			AR[i][k] = fabs(R[i][k]);
        }
	}
            
    // Vector separating the centers of Box B and of Box A	
    Vector3d vSepWS = BBox.GetCenterPoint() - GetCenterPoint();
    // Rotated into Box A's coordinates
    Vector3d vSepA( vSepWS.dot(RotA[0]), vSepWS.dot(RotA[1]), vSepWS.dot(RotA[2]) );            

     // Test if any of A's basis vectors separate the box
	for( i = 0; i < 3; i++ )
	{
		ExtentA = SizeA[i];
		ExtentB = SizeB.dot( Vector3d( AR[i][0], AR[i][1], AR[i][2] ) );
		Separation = fabs( vSepA[i] );

		if( Separation > ExtentA + ExtentB ) return false;
	}

	// Test if any of B's basis vectors separate the box
	for( k = 0; k < 3; k++ )
	{
		ExtentA = SizeA.dot( Vector3d( AR[0][k], AR[1][k], AR[2][k] ) );
		ExtentB = SizeB[k];
		Separation = fabs( vSepA.dot( Vector3d(R[0][k],R[1][k],R[2][k]) ) );

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