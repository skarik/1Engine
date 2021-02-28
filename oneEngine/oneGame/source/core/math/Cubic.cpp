
// Includes
#include "Cubic.h"

using namespace core::math;

// Default empty constructor
Cubic::Cubic ( void )
{
	position = Vector3f( 0,0,0 );
	size = Vector3f( 1,1,1 );
}
// Constructor taking two vectors
Cubic::Cubic ( Vector3f const& vBottomBackRightCorner, Vector3f const& vSize )
{
	position = vBottomBackRightCorner;
	size = vSize;
}
// Creation taking min and max pos
Cubic Cubic::FromPosition ( Vector3f vMinPos, Vector3f vMaxPos )
{
	if ( vMinPos.x > vMaxPos.x ) {
		Real temp = vMinPos.x;
		vMinPos.x  = vMaxPos.x;
		vMaxPos.x  = temp;
	}
	if ( vMinPos.y > vMaxPos.y ) {
		Real temp = vMinPos.y;
		vMinPos.y  = vMaxPos.y;
		vMaxPos.y  = temp;
	}
	if ( vMinPos.z > vMaxPos.z ) {
		Real temp = vMinPos.z;
		vMinPos.z  = vMaxPos.z;
		vMaxPos.z  = temp;
	}
	return Cubic( vMinPos, (vMaxPos-vMinPos) );
}

// Looks at the position and size, and moves their values around so that
// the position refers to the bottom back right corner of the Cubic.
void Cubic::Realign( void )
{
	if ( size.x < 0 ) {
		position.x += size.x;
		size.x = -size.x;
	}
	if ( size.y < 0 ) {
		position.y += size.y;
		size.y = -size.y;
	}
	if ( size.z < 0 ) {
		position.z += size.z;
		size.z = -size.z;
	}
}

// Get center
Vector3f Cubic::center ( void ) const
{
	return ( position + (size*0.5f) );
}

// Returns a frustum
Frustum Cubic::GetFrustum ( Vector3f const& casterPosition )
{
	// 0,0,0,bottom,right,backward
	// First gotta choose what to generate the frustum with.
	// This shouldn't be too hard. Based on the closest vertex to the camera, choose the other 6 vertices to use.
	Vector3f places [8] = {
		position,
		position + Vector3f( size.x,0,0 ),
		position + Vector3f( 0,size.y,0 ),
		position + Vector3f( size.x,size.y,0 ),
		position + Vector3f( 0,0,size.z ),
		position + Vector3f( size.x,0,size.z ),
		position + Vector3f( 0,size.y,size.z ),
		position + Vector3f( size.x,size.y,size.z )
	};
	const static int cube_lookup [8][6] = {
		{3,1,5,4,6,2},
		{2,3,7,5,4,0},
		{1,0,4,6,7,3},
		{0,2,6,7,5,1},
		{0,1,5,7,6,2},
		{1,3,7,6,4,0},
		{2,0,4,5,7,3},
		{3,2,6,4,5,1}
	};
	// So gotta find closests vertex to camera
	Real mindist = -1.0;
	Real currentdist;
	int sm_vertex = -1;
	for ( int i = 0; i < 8; i += 1 )
	{
		// Get current dist
		currentdist = ( places[i]-casterPosition ).sqrMagnitude();
		// I SUCK DICKS, DISREGARD
		if (( mindist < 0.0 )||( mindist > currentdist ))
		{
			mindist = currentdist;
			sm_vertex = i;
		}
	}
	
	Frustum result;
	for ( int i = 0; i < 5; i += 1 )
	{
		result.plane[i].ConstructFromPoints( casterPosition, places[cube_lookup[sm_vertex][i]],places[cube_lookup[sm_vertex][i+1]] );
	}
	result.plane[5].ConstructFromPoints( casterPosition, places[cube_lookup[sm_vertex][5]],places[cube_lookup[sm_vertex][0]] );

	return result;
}

// Check if point is inside the box
bool Cubic::PointIsInside ( Vector3f const& point )
{
	if ( point.x < position.x ) return false;
	if ( point.y < position.y ) return false;
	if ( point.z < position.z ) return false;
	if ( point.x > position.x+size.x ) return false;
	if ( point.y > position.y+size.y ) return false;
	if ( point.z > position.z+size.z ) return false;
	return true;
}
/*#include "../RrDebugDrawer.h"
void Cubic::DrawDebug ( void )
{
	debug::Drawer->DrawLine( position+Vector3f( 0,0,0 ), position+Vector3f( size.x,0,0 ) );
	debug::Drawer->DrawLine( position+Vector3f( size.x,0,0 ), position+Vector3f( size.x,size.y,0 ) );
	debug::Drawer->DrawLine( position+Vector3f( size.x,size.y,0 ), position+Vector3f( 0,size.y,0 ) );
	debug::Drawer->DrawLine( position+Vector3f( 0,size.y,0 ), position+Vector3f( 0,0,0 ) );
	debug::Drawer->DrawLine( position+Vector3f( 0,0,size.z ), position+Vector3f( size.x,0,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( size.x,0,size.z ), position+Vector3f( size.x,size.y,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( size.x,size.y,size.z ), position+Vector3f( 0,size.y,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( 0,size.y,size.z ), position+Vector3f( 0,0,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( 0,0,0 ), position+Vector3f( 0,0,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( size.x,0,0 ), position+Vector3f( size.x,0,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( size.x,size.y,0 ), position+Vector3f( size.x,size.y,size.z ) );
	debug::Drawer->DrawLine( position+Vector3f( 0,size.y,0 ), position+Vector3f( 0,size.y,size.z ) );
}*/

// Check if box intersects with box
bool Cubic::CubicCollides ( const Cubic & cube )
{
	Vector3f cubePosition = (position+size*0.5f)-(cube.position+cube.size*0.5f);
	Vector3f cubeExtents = (size + cube.size)*0.5f;
	
	if ( fabs( cubePosition.x ) > cubeExtents.x ) return false;
	if ( fabs( cubePosition.y ) > cubeExtents.y ) return false;
	if ( fabs( cubePosition.z ) > cubeExtents.z ) return false;
	
	return true;
}

// Check if line intersects with the box
bool Cubic::LineCollides ( Line const& line )
{
	/*Vector3f position2 = position+size;

	if (line.end.x < position.x  && line.start.x < position.x) return false;
	if (line.end.x > position2.x && line.start.x > position2.x) return false;
	if (line.end.y < position.y  && line.start.y < position.y) return false;
	if (line.end.y > position2.y && line.start.y > position2.y) return false;
	if (line.end.z < position.z  && line.start.z < position.z) return false;
	if (line.end.z > position2.z && line.start.z > position2.z) return false;

	if ( PointIsInside( line.start ) )
		return true;

	return false;*/

	Vector3f cubeCenter = position+size*Real(0.5);
	Vector3f cubeExtents = size*Real(0.5);

	Vector3f lineMid = ((line.midpoint())-cubeCenter);
	Vector3f lineDir = ((line.start-cubeCenter) - lineMid);
	Vector3f lineExt = Vector3f( fabs(lineDir.x), fabs(lineDir.y), fabs(lineDir.z) );

	// Use Separating Axis Test
	// Separation vector from box center to line center is LMid, since the line is in box space
	if ( fabs( lineMid.x ) > cubeExtents.x + lineExt.x ) return false;
	if ( fabs( lineMid.y ) > cubeExtents.y + lineExt.y ) return false;
	if ( fabs( lineMid.z ) > cubeExtents.z + lineExt.z ) return false;
	// Crossproducts of line and each axis
	if ( fabs( lineMid.y * lineDir.z - lineMid.z * lineDir.y)  >  (cubeExtents.y * lineExt.z + cubeExtents.z * lineExt.y) ) return false;
	if ( fabs( lineMid.x * lineDir.z - lineMid.z * lineDir.x)  >  (cubeExtents.x * lineExt.z + cubeExtents.z * lineExt.x) ) return false;
	if ( fabs( lineMid.x * lineDir.y - lineMid.y * lineDir.x)  >  (cubeExtents.x * lineExt.y + cubeExtents.y * lineExt.x) ) return false;
	// No separating axis, the line intersects
	return true;

}

//http://www.3dkingdoms.com/weekly/weekly.php?a=3
// Check if line intersects with the box. Get the first intersection point
/*bool CCubic::LineGetCollision ( Line const& line, Vector3f & outPos )
{
	Vector3f position2 = position+size;

	if (line.end.x < position.x  && line.start.x < position.x) return false;
	if (line.end.x > position2.x && line.start.x > position2.x) return false;
	if (line.end.y < position.y  && line.start.y < position.y) return false;
	if (line.end.y > position2.y && line.start.y > position2.y) return false;
	if (line.end.z < position.z  && line.start.z < position.z) return false;
	if (line.end.z > position2.z && line.start.z > position2.z) return false;

	if ( PointIsInside( line.start ) )
	{
		outPos = line.start;
		return true;
	}

	if (   (GetIntersection( line.start.x-position.x, line.end.x-position.x, line.start, line.end, outPos ) && InBox( outPos, position, position2, 1 ))
		|| (GetIntersection( line.start.y-position.y, line.end.y-position.y, line.start, line.end, outPos ) && InBox( outPos, position, position2, 2 )) 
		|| (GetIntersection( line.start.z-position.z, line.end.z-position.z, line.start, line.end, outPos ) && InBox( outPos, position, position2, 3 )) 
		|| (GetIntersection( line.start.x-position2.x, line.end.x-position2.x, line.start, line.end, outPos ) && InBox( outPos, position, position2, 1 )) 
		|| (GetIntersection( line.start.y-position2.y, line.end.y-position2.y, line.start, line.end, outPos ) && InBox( outPos, position, position2, 2 )) 
		|| (GetIntersection( line.start.z-position2.z, line.end.z-position2.z, line.start, line.end, outPos ) && InBox( outPos, position, position2, 3 )))
		return true;

	return false;
}

// Helper functions for collision
bool inline CCubic::GetIntersection( Real fDst1, Real fDst2, Vector3f P1, Vector3f P2, Vector3f & Hit )
{
	if ( (fDst1 * fDst2) >= 0.0f) return false;
	//if ( fDst1 == fDst2 ) return false; 
	if ( fabs( fDst1-fDst2 ) < 0.0001f ) return false; 
	Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
	return true;
}
bool inline CCubic::InBox( Vector3f Hit, Vector3f B1, Vector3f B2, const char Axis )
{
	if ( Axis==1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return true;
	if ( Axis==2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return true;
	if ( Axis==3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return true;
	return false;
}*/

//http://www.gamedev.net/topic/505066-finding-intersect-of-line-segment-and-aabb/
// ray-aabb test along one axis
bool rayAABBIntersect1D(float start, float dir, float min, float max, float& enter, float& exit)
{
    // ray parallel to direction
    if(fabs(dir) < 1.0E-8) 
        return (start >= min && start <= max);

    // intersection params
    float t0, t1;
    t0 = (min - start) / dir;
    t1 = (max - start) / dir;

    // sort intersections
    if(t0 > t1) std::swap(t0, t1);

    // check if intervals are disjoint
    if(t0 > exit || t1 < enter) 
        return false;

    // reduce interval
    if(t0 > enter) enter = t0;
    if(t1 < exit) exit = t1;
    return true;
}
bool rayAABBIntersect(Vector3f start, Vector3f dir, Vector3f min, Vector3f max, Vector3f& penter, Vector3f& pexit)
{
    float enter = 0.0f, exit = 1.0f;

    if(!rayAABBIntersect1D(start.x, dir.x, min.x, max.x, enter, exit))
        return false;

    if(!rayAABBIntersect1D(start.y, dir.y, min.y, max.y, enter, exit))
        return false;
    
    if(!rayAABBIntersect1D(start.z, dir.z, min.z, max.z, enter, exit))
        return false;

    penter = start + dir * enter;
    pexit  = start + dir * exit;
    return true;
}
bool Cubic::LineGetCollision ( Line const& line, Vector3f & outPos )
{
	Vector3f bleh;
	return rayAABBIntersect( line.start, line.end-line.start, position, position+size, outPos, bleh );
}