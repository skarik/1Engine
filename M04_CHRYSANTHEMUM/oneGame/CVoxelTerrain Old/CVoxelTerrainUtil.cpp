

// Includes
#include "CVoxelTerrain.h"

#include "CDebugDrawer.h"

// ===GETTING BOUNDING BOX FOR TARGET AREA===
CCubic		CVoxelTerrain::GetAreaBoundingBox ( Vector3d const& pos, ftype area_size )
{
	Vector3d cubepos;
	cubepos.x = floor( (pos.x+0.0001f)/area_size ) * area_size;
	cubepos.y = floor( (pos.y+0.0001f)/area_size ) * area_size;
	cubepos.z = floor( (pos.z+0.0001f)/area_size ) * area_size;

	CCubic result ( cubepos, Vector3d( area_size,area_size,area_size ) );
	return result;
}
CCubic		CVoxelTerrain::CreateBoundingBox ( Vector3d const& pos, ftype area_size )
{
	CCubic result ( pos, Vector3d( area_size,area_size,area_size ) );
	return result;
}

// ===RAYTRACING===
// Raytracer function
bool	CVoxelTerrain::Raycast ( Ray const& ray, ftype max_dist, BlockInfo * outHitBlockInfo, RaycastHit * outHitResult )
{
	// Draw debug raytrace line
	//DebugD::DrawLine( ray.pos, ray.pos+ray.dir*max_dist );

	// Save the info so it's not being moved all over the place
	rcst_ray			= ray;
	rcst_maxdist		= max_dist;
	rcst_pOutBlockInfo	= outHitBlockInfo;
	rcst_pOutHitInfo	= outHitResult;
	rcst_bMadeCollision = false;

	// Set initial data
	rcst_pOutHitInfo->distance = -1;
	rcst_mindist = -1;

	// Keep track of current distance to not fuck things up
	//ftype currentDist = 0;
	rcst_curdist = 0;

	// Start in the boob the ray is in
	CBoob* startBoob = GetBoobAtPosition( ray.pos );//- Vector3d( 2.0f,2.0f,2.0f )
	Raycast32( startBoob );
	// Do a raycast for each part

	// Return if hit
	return rcst_bMadeCollision;
}
// Checking collision with the 32 sized area
void	CVoxelTerrain::Raycast32 ( CBoob * pBoob )
{
	if ( pBoob != NULL )
	{
		CCubic	boundingCube	= GetAreaBoundingBox( pBoob->position, 64.0f );
		Line	collisionLine;
		collisionLine.start		= rcst_ray.pos + rcst_ray.dir*rcst_curdist;
		collisionLine.end		= rcst_ray.pos + rcst_ray.dir*rcst_maxdist;
		bool	containsLine	= boundingCube.LineCollides( collisionLine );

		//DebugD::DrawLine( collisionLine.start+ Vector3d( 0,0.2f,0.2f ), collisionLine.end ); 
		//boundingCube.DrawDebug();

		if ( containsLine )
		{
			/*DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(-1,0,0),boundingCube.position+boundingCube.size*0.5f+Vector3d(1,0,0) );
			DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(0,-1,0),boundingCube.position+boundingCube.size*0.5f+Vector3d(0,1,0) );
			DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(0,0,-1),boundingCube.position+boundingCube.size*0.5f+Vector3d(0,0,1) );*/

			rcst_pCurrentBoob = pBoob;
			for ( char i = 0; i < 8; i += 1 )
			{
				//Raycast16( pBoob, &(pBoob->data[i]) );
				Raycast16( i );
			}
		}

		if (( !rcst_bMadeCollision )&&( containsLine ))
		{
			// Move to the next areas
			if ( rcst_ray.dir.x > 0 )
				Raycast32( pBoob->front );
			if ( rcst_ray.dir.y > 0 )
				Raycast32( pBoob->left );
			if ( rcst_ray.dir.z > 0 )
				Raycast32( pBoob->top );
			if ( rcst_ray.dir.x < 0 )
				Raycast32( pBoob->back );
			if ( rcst_ray.dir.y < 0 )
				Raycast32( pBoob->right );
			if ( rcst_ray.dir.z < 0 )
				Raycast32( pBoob->bottom );
		}

	}
}
// Checking collision with the 16 sized area
void	CVoxelTerrain::Raycast16 ( char index )
{
	Vector3d targetPos = rcst_pCurrentBoob->position - Vector3d( 32,32,32 );
	targetPos.x += (index%2)	* 32.0f;
	targetPos.y += ((index/2)%2)* 32.0f;
	targetPos.z += ((index/4))	* 32.0f;

	//CCubic	boundingCube	= GetAreaBoundingBox( targetPos, 32.0f );
	CCubic	boundingCube	= CreateBoundingBox( targetPos, 32.0f );
	Line	collisionLine;
	collisionLine.start		= rcst_ray.pos + rcst_ray.dir*rcst_curdist;
	collisionLine.end		= rcst_ray.pos + rcst_ray.dir*rcst_maxdist;
	bool	containsLine	= boundingCube.LineCollides( collisionLine );

	if ( containsLine )
	{
		//Ray debugRay;
		//debugRay.dir = rcst_ray.dir-Vector3d( 0,0,1 );
		//debugRay.pos = rcst_ray.pos;
		//DebugD::DrawRay(debugRay);
		//DebugD::DrawLine( collisionLine.start, collisionLine.end + Vector3d( 0,0,10 ) ); 
		/*DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(-1,0,0),boundingCube.position+boundingCube.size*0.5f+Vector3d(1,0,0) );
		DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(0,-1,0),boundingCube.position+boundingCube.size*0.5f+Vector3d(0,1,0) );
		DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(0,0,-1),boundingCube.position+boundingCube.size*0.5f+Vector3d(0,0,1) );*/
		//boundingCube.DrawDebug();

		rcst_pCurrentSubblock = &( rcst_pCurrentBoob->data[index] );
		for ( char i = 0; i < 8; i += 1 )
		{
			rcst_vCurSubpos = targetPos;
			rcst_vCurSubpos.x += (i%2)	* 16.0f;
			rcst_vCurSubpos.y += ((i/2)%2)* 16.0f;
			rcst_vCurSubpos.z += ((i/4))	* 16.0f;

			Raycast08( i );
		}
	}
}
// Checking collision with the 8 sized area
void	CVoxelTerrain::Raycast08 ( char index )
{
	Vector3d blockPos;

	for ( short i = 0; i < 512; i += 1 )
	{
		if ( (rcst_pCurrentSubblock->data[index].data[i]) != 0 )
		{
			blockPos = rcst_vCurSubpos;
			blockPos.x += (i%8)		* 2.0f;
			blockPos.y += ((i/8)%8)	* 2.0f;
			blockPos.z += ((i/64))	* 2.0f;
			Raycast01( blockPos );
		}
	}
}
// Checking collision with a single block
void	CVoxelTerrain::Raycast01 ( Vector3d const& pos )
{
	//CCubic	boundingCube	= GetAreaBoundingBox( pos, 2.0f );
	CCubic	boundingCube	= CreateBoundingBox( pos-Vector3d(0.01f,0.01f,0.01f), 2.02f );
	Vector3d vHitPos;
	Line	collisionLine;
	collisionLine.start		= rcst_ray.pos + rcst_ray.dir*rcst_curdist;
	collisionLine.end		= rcst_ray.pos + rcst_ray.dir*rcst_maxdist;

	//if ( boundingCube.LineCollides( collisionLine ) )
	if ( boundingCube.LineGetCollision( collisionLine, vHitPos ) )
	{
		//vHitPos = pos;

		//boundingCube.DrawDebug();

		/*DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(-1,0,0),boundingCube.position+boundingCube.size*0.5f+Vector3d(1,0,0) );
		DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(0,-1,0),boundingCube.position+boundingCube.size*0.5f+Vector3d(0,1,0) );
		DebugD::DrawLine( boundingCube.position+boundingCube.size*0.5f+Vector3d(0,0,-1),boundingCube.position+boundingCube.size*0.5f+Vector3d(0,0,1) );*/

		rcst_bMadeCollision = true;

		ftype sqrDist = (vHitPos-rcst_ray.pos).sqrMagnitude();
		if ( ( rcst_mindist < 0 )||( sqrDist < sqr( rcst_mindist ) ) )
		{
			rcst_mindist = sqrt( sqrDist );
			rcst_pOutHitInfo->distance = rcst_mindist;
			rcst_pOutHitInfo->hitPos = vHitPos;
			/*rcst_pOutHitInfo->hitNormal = Vector3d(
				ftype((rcst_ray.pos.x<pos.x) ? -1 : 1),
				ftype((rcst_ray.pos.y<pos.y) ? -1 : 1),
				ftype((rcst_ray.pos.z<pos.z) ? -1 : 1)
				);*/
			rcst_pOutHitInfo->hitNormal = Vector3d( 0,0,0 );
			if ( vHitPos.x < pos.x )
				rcst_pOutHitInfo->hitNormal.x = -1;
			else if ( vHitPos.x > pos.x+2.0 )
				rcst_pOutHitInfo->hitNormal.x = +1;
			else if ( vHitPos.y < pos.y )
				rcst_pOutHitInfo->hitNormal.y = -1;
			else if ( vHitPos.y > pos.y+2.0 )
				rcst_pOutHitInfo->hitNormal.y = +1;
			else if ( vHitPos.z < pos.z )
				rcst_pOutHitInfo->hitNormal.z = -1;
			else if ( vHitPos.z > pos.z+2.0 )
				rcst_pOutHitInfo->hitNormal.z = +1;

			if ( rcst_pOutBlockInfo != NULL )
			{
				GetBlockInfoAtPosition( pos+Vector3d(1,1,1),*rcst_pOutBlockInfo );
			}
		}

		Ray debugRay;
		debugRay.dir = rcst_pOutHitInfo->hitNormal * 2.0f;
		debugRay.pos = rcst_pOutHitInfo->hitPos;
		DebugD::DrawRay(debugRay);
		DebugD::DrawLine( rcst_pOutHitInfo->hitPos, rcst_pOutHitInfo->hitPos+Vector3d( 0.5f,0,0 ) );
		DebugD::DrawLine( rcst_pOutHitInfo->hitPos, rcst_pOutHitInfo->hitPos+Vector3d( -0.5f,0,0 ) );
		DebugD::DrawLine( rcst_pOutHitInfo->hitPos, rcst_pOutHitInfo->hitPos+Vector3d( 0,0.5f,0 ) );
		DebugD::DrawLine( rcst_pOutHitInfo->hitPos, rcst_pOutHitInfo->hitPos+Vector3d( 0,-0.5f,0 ) );
		DebugD::DrawLine( rcst_pOutHitInfo->hitPos, rcst_pOutHitInfo->hitPos+Vector3d( 0,0,0.5f ) );
		DebugD::DrawLine( rcst_pOutHitInfo->hitPos, rcst_pOutHitInfo->hitPos+Vector3d( 0,0,-0.5f ) );
	}
}

// ===BOXCOLLIDING===
// BoxCollider function
bool	CVoxelTerrain::BoxCollides ( CCubic const& bbox )
{
	// Set current collider info
	boxc_bMadeCollision = false;
	
	// Start in the boob the box is in
	CBoob* startBoob = GetBoobAtPosition( bbox.center() - Vector3d( 2,2,2 ) );

	// Check each side of the cube
	boxc_line.start = bbox.position;
	boxc_line.end = bbox.position + Vector3d( bbox.size.x,0,0 );
	BoxCollide32( startBoob );
	boxc_line.end = bbox.position + Vector3d( 0,bbox.size.y,0 );
	BoxCollide32( startBoob );
	boxc_line.end = bbox.position + Vector3d( 0,0,bbox.size.z );
	BoxCollide32( startBoob );

	boxc_line.start = bbox.position + bbox.size;
	boxc_line.end = bbox.position + Vector3d( bbox.size.x,bbox.size.y,0 );
	BoxCollide32( startBoob );
	boxc_line.end = bbox.position + Vector3d( bbox.size.x,0,bbox.size.z );
	BoxCollide32( startBoob );
	boxc_line.end = bbox.position + Vector3d( 0,bbox.size.y,bbox.size.z );
	BoxCollide32( startBoob );

	boxc_line.start = bbox.position + Vector3d( 0,0,bbox.size.z );
	boxc_line.end = boxc_line.start + Vector3d( bbox.size.x,0,0 );
	BoxCollide32( startBoob );
	boxc_line.end = boxc_line.start + Vector3d( 0,bbox.size.y,0 );
	BoxCollide32( startBoob );

	boxc_line.start = bbox.position + Vector3d( bbox.size.x,bbox.size.y,0 );
	boxc_line.end = boxc_line.start - Vector3d( bbox.size.x,0,0 );
	BoxCollide32( startBoob );
	boxc_line.end = boxc_line.start - Vector3d( 0,bbox.size.y,0 );
	BoxCollide32( startBoob );

	boxc_line.start = bbox.position + Vector3d( bbox.size.x,0,0 );
	boxc_line.end = boxc_line.start + Vector3d( 0,0,bbox.size.z );
	BoxCollide32( startBoob );

	boxc_line.start = bbox.position + Vector3d( bbox.size.y,0,0 );
	boxc_line.end = boxc_line.start + Vector3d( 0,0,bbox.size.z );
	BoxCollide32( startBoob );

	// Return if hit
	return boxc_bMadeCollision;
}

// Checking collision with the 32 sized area
void	CVoxelTerrain::BoxCollide32 ( CBoob * pBoob )
{
	if (( pBoob != NULL )&&( !boxc_bMadeCollision ))
	{
		CCubic	boundingCube	= GetAreaBoundingBox( pBoob->position, 64.0f );
		//Line	collisionLine	= boxc_line;
		rcst_ray.pos = boxc_line.start;
		rcst_ray.dir = boxc_line.end-boxc_line.start;
		rcst_maxdist = rcst_ray.dir.magnitude();
		bool	containsLine	= boundingCube.LineCollides( boxc_line );

		if ( containsLine )
		{
			rcst_pCurrentBoob = pBoob;
			for ( char i = 0; i < 8; i += 1 )
			{
				if ( !boxc_bMadeCollision )
					BoxCollide16( i );
			}
		}

		if (( !boxc_bMadeCollision )&&( containsLine ))
		{
			// Move to the next areas
			if ( rcst_ray.dir.x > 0 )
				BoxCollide32( pBoob->front );
			if ( rcst_ray.dir.y > 0 )
				BoxCollide32( pBoob->left );
			if ( rcst_ray.dir.z > 0 )
				BoxCollide32( pBoob->top );
			if ( rcst_ray.dir.x < 0 )
				BoxCollide32( pBoob->back );
			if ( rcst_ray.dir.y < 0 )
				BoxCollide32( pBoob->right );
			if ( rcst_ray.dir.z < 0 )
				BoxCollide32( pBoob->bottom );
		}
	}
}
// Checking collision with the 16 sized area
void	CVoxelTerrain::BoxCollide16 ( char index )
{
	Vector3d targetPos = rcst_pCurrentBoob->position - Vector3d( 32,32,32 );
	targetPos.x += (index%2)	* 32.0f;
	targetPos.y += ((index/2)%2)* 32.0f;
	targetPos.z += ((index/4))	* 32.0f;

	CCubic	boundingCube	= CreateBoundingBox( targetPos, 32.0f );
	bool	containsLine	= boundingCube.LineCollides( boxc_line );

	if ( containsLine )
	{
		rcst_pCurrentSubblock = &( rcst_pCurrentBoob->data[index] );
		for ( char i = 0; i < 8; i += 1 )
		{
			if ( !boxc_bMadeCollision )
			{
				rcst_vCurSubpos = targetPos;
				rcst_vCurSubpos.x += (i%2)	* 16.0f;
				rcst_vCurSubpos.y += ((i/2)%2)* 16.0f;
				rcst_vCurSubpos.z += ((i/4))	* 16.0f;
	
				BoxCollide08( i );
			}
		}
	}
}
// Checking collision with the 8 sized area
void	CVoxelTerrain::BoxCollide08 ( char index )
{
	Vector3d blockPos;

	for ( short i = 0; i < 512; i += 1 )
	{
		if ( (rcst_pCurrentSubblock->data[index].data[i]) != 0 )
		{
			if ( !boxc_bMadeCollision )
			{
				blockPos = rcst_vCurSubpos;
				blockPos.x += (i%8)		* 2.0f;
				blockPos.y += ((i/8)%8)	* 2.0f;
				blockPos.z += ((i/64))	* 2.0f;
				BoxCollide01( blockPos );
			}
		}
	}
}
// Checking collision with a single block
void	CVoxelTerrain::BoxCollide01 ( Vector3d const& pos )
{
	//CCubic	boundingCube	= GetAreaBoundingBox( pos, 2.0f );
	//CCubic	boundingCube	= CreateBoundingBox( pos-Vector3d(0.01f,0.01f,0.01f), 2.02f );
	CCubic	boundingCube	= CreateBoundingBox( pos, 2.0f );
	/*Vector3d vHitPos;
	Line	collisionLine;
	collisionLine.start		= rcst_ray.pos + rcst_ray.dir*rcst_curdist;
	collisionLine.end		= rcst_ray.pos + rcst_ray.dir*rcst_maxdist;*/

	//
	//if ( boundingCube.LineGetCollision( collisionLine, vHitPos ) )
	if ( boundingCube.LineCollides( boxc_line ) )
	{
		boxc_bMadeCollision = true;
	}
}
