

// Includes
#include "Raycaster.h"
//#include "CVoxelTerrain.h"
//#include "COctreeTerrain.h"
#include "engine/physics/CPhysicsEngine.h"

// Global instance
CRaycaster Raycaster;

// Static values
//BlockTrackInfo CRaycaster::m_blockInfo;
physMaterial* CRaycaster::m_lastHitMaterial = NULL;

//#include "CDebugDrawer.h"
// Raytracer
bool	CRaycaster::Raycast	( const Ray & ray, ftype max_dist, RaycastHit * pOutHitInfo, /*BlockTrackInfo * pOutBlockInfo,*/ r_bitmask collisionFilter, void* mismatch )
{
	//bool hit = false;

	if ( (collisionFilter == (1|2|4)) || (collisionFilter == 0x00) )
	{
		collisionFilter = Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31);
	}

	static RaycastHit dummyInfo;
	if ( pOutHitInfo == NULL ) {
		pOutHitInfo = &dummyInfo;
	}

	/*for ( unsigned short i = 0; i < CVoxelTerrain::terrainList.size(); i += 1 )
		if ( CVoxelTerrain::terrainList[i]->Raycast( ray,max_dist,pOutBlockInfo,pOutHitInfo ) )
			hit = true;*/
	PhysicsEngine::Raycast( ray, max_dist, pOutHitInfo, collisionFilter, mismatch );
//	if ( pOutHitInfo->hit ) {
//		if ( COctreeTerrain::GetActive() ) {
//			//COctreeTerrain::GetActive()->SampleBlock( (pOutHitInfo->hitPos)-(pOutHitInfo->hitNormal), (pOutBlockInfo->block.raw) );
//			COctreeTerrain::GetActive()->SampleBlockExpensive( (pOutHitInfo->hitPos)-(pOutHitInfo->hitNormal*0.99f) /*- (Vector3d(1,1,1)*COctreeTerrain::blockSize)*/, pOutBlockInfo );
//		}
//		else if (( !CVoxelTerrain::terrainList.empty() )&&( CVoxelTerrain::terrainList[0] != NULL )) {
//			//CVoxelTerrain::terrainList[0]->GetBlockInfoAtPosition( (pOutHitInfo->hitPos)-(pOutHitInfo->hitNormal), *pOutBlockInfo );
//		}
//	}
	/*
#ifdef _ENGINE_DEBUG
	Ray debugRay;
	debugRay.dir = pOutHitInfo->hitNormal * 1.0f;
	debugRay.pos = pOutHitInfo->hitPos;
	Debug::Drawer->DrawRay(debugRay);
	Debug::Drawer->DrawLine( pOutHitInfo->hitPos, pOutHitInfo->hitPos+Vector3d( 0.25f,0,0 ) );
	Debug::Drawer->DrawLine( pOutHitInfo->hitPos, pOutHitInfo->hitPos+Vector3d( -0.25f,0,0 ) );
	Debug::Drawer->DrawLine( pOutHitInfo->hitPos, pOutHitInfo->hitPos+Vector3d( 0,0.25f,0 ) );
	Debug::Drawer->DrawLine( pOutHitInfo->hitPos, pOutHitInfo->hitPos+Vector3d( 0,-0.25f,0 ) );
	Debug::Drawer->DrawLine( pOutHitInfo->hitPos, pOutHitInfo->hitPos+Vector3d( 0,0,0.25f ) );
	Debug::Drawer->DrawLine( pOutHitInfo->hitPos, pOutHitInfo->hitPos+Vector3d( 0,0,-0.25f ) );
#endif
	*/
	return pOutHitInfo->hit;
}
/*bool	CRaycaster::Raycast	( const Ray & ray, ftype max_dist, RaycastHit * pOutHitInfo, r_bitmask collisionFilter, void* mismatch )
{
	return Raycast( ray,max_dist,pOutHitInfo,&m_blockInfo,collisionFilter,mismatch );
}*/

bool CRaycaster::Linecast ( const Ray & ray, ftype max_dist, physShape* pShape, RaycastHit* pOutHitInfo, const int hitInfoArrayCount, r_bitmask collisionFilter, void* mismatch )
{
	if ( (collisionFilter == (1|2|4)) || (collisionFilter == 0x00) )
	{
		collisionFilter = Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31);
	}

	int sendCount = hitInfoArrayCount;

	static RaycastHit dummyInfo;
	if ( pOutHitInfo == NULL ) {
		pOutHitInfo = &dummyInfo;
		sendCount = 1;
	}

	PhysicsEngine::Linearcast( ray, max_dist, pShape, pOutHitInfo, sendCount, collisionFilter, mismatch );

	return pOutHitInfo->hit;
}


// Box collider
/*bool	CRaycaster::BoxCollides	( Maths::Cubic const& box, r_bitmask collisionFilter )
{
	for ( unsigned short i = 0; i < CVoxelTerrain::terrainList.size(); i += 1 )
		if ( CVoxelTerrain::terrainList[i]->BoxCollides( box ) )
			return true;
	return false;
}
*/
// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
//http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
void Barycentric ( const Vector3d& p, const Vector3d& a, const Vector3d& b, const Vector3d& c, ftype &u, ftype &v, ftype &w)
{
    Vector3d v0 = b - a, v1 = c - a, v2 = p - a;
    ftype d00 = v0.dot(v0);
    ftype d01 = v0.dot(v1);
    ftype d11 = v1.dot(v1);
    ftype d20 = v2.dot(v0);
    ftype d21 = v2.dot(v1);
    ftype invDenom = 1.0f / (d00 * d11 - d01 * d01);
    v = (d11 * d20 - d01 * d21) * invDenom;
    w = (d00 * d21 - d01 * d20) * invDenom;
    u = 1.0f - v - w;
}

// Basic mesh raytracer
int CRaycaster::Raycast ( const Ray & ray, ftype max_dist, const CModelData* model, Vector3d& out_point, Vector3d& out_bary )
{
	Maths::Plane triPlane;
	CModelVertex* v0;
	CModelVertex* v1;
	CModelVertex* v2;
	ftype ray_incidence;
	ftype param_t;
	Vector3d incident_point;
	Vector3d bry_coords;

	ftype min_param_t = max_dist;
	int closestTri = -1;
	
	// Need to loop through all the triangles of the mesh
	for ( uint tri = 0; tri < model->triangleNum; ++tri )
	{
		// Grab triangle verts
		v0 = &(model->vertices[model->triangles[tri].vert[0]]);
		v1 = &(model->vertices[model->triangles[tri].vert[1]]);
		v2 = &(model->vertices[model->triangles[tri].vert[2]]);

		// Get the triangle information into a plane
		triPlane.ConstructFromPoints(
			Vector3d( v0->x, v0->y, v0->z ),
			Vector3d( v1->x, v1->y, v1->z ),
			Vector3d( v2->x, v2->y, v2->z ) );

		// Calculate ray incidence
		ray_incidence = triPlane.n.dot( ray.dir );

		// Skip this triangle if facing the wrong way
		if ( ray_incidence > 0 ) {
			continue;
		}

		// Calculate the intersection point
		param_t = ( ray.pos.dot( triPlane.n ) + triPlane.d ) / - ray.dir.dot( triPlane.n );
		incident_point = ray.pos + ray.dir * param_t;

		// Get barycentric coordinates
		Barycentric( incident_point, Vector3d( v0->x, v0->y, v0->z ), Vector3d( v1->x, v1->y, v1->z ), Vector3d( v2->x, v2->y, v2->z ), bry_coords.x, bry_coords.y, bry_coords.z );

		// Check for out of bounds
		if (( bry_coords.x < 0 )||( bry_coords.x > 1 )||( bry_coords.y < 0 )||( bry_coords.y > 1 )||( bry_coords.z < 0 )||( bry_coords.z > 1 )) {
			continue;
		}

		// Check for range, and if this one is closer, set value
		if ( min_param_t > param_t ) {
			min_param_t = param_t;
			closestTri = tri;

			out_point = incident_point;
			out_bary = bry_coords;
		}
	}

	// No collision, return invalid index
	return closestTri;
}

// Basic mesh raytracer
int CRaycaster::RaycastExpensiveMiss ( const Ray & ray, ftype max_dist, const CModelData* model, Vector3d& out_point, Vector3d& out_bary )
{
	Maths::Plane triPlane;
	CModelVertex* v0;
	CModelVertex* v1;
	CModelVertex* v2;
	ftype ray_incidence;
	ftype param_t;
	Vector3d incident_point;
	Vector3d bry_coords;
	Vector3d dif_bary_coords;
	Vector3d min_dif_bary_coords = Vector3d(max_dist,max_dist,max_dist);

	ftype min_param_t = max_dist;
	int closestTri = -1;
	
	// Need to loop through all the triangles of the mesh
	for ( uint tri = 0; tri < model->triangleNum; ++tri )
	{
		// Grab triangle verts
		v0 = &(model->vertices[model->triangles[tri].vert[0]]);
		v1 = &(model->vertices[model->triangles[tri].vert[1]]);
		v2 = &(model->vertices[model->triangles[tri].vert[2]]);

		// Get the triangle information into a plane
		triPlane.ConstructFromPoints(
			Vector3d( v0->x, v0->y, v0->z ),
			Vector3d( v1->x, v1->y, v1->z ),
			Vector3d( v2->x, v2->y, v2->z ) );

		// Calculate ray incidence
		ray_incidence = triPlane.n.dot( ray.dir );

		// Skip this triangle if facing the wrong way
		if ( ray_incidence > 0 ) {
			continue;
		}

		// Calculate the intersection point
		param_t = ( ray.pos.dot( triPlane.n ) + triPlane.d ) / - ray.dir.dot( triPlane.n );
		incident_point = ray.pos + ray.dir * param_t;

		// Get barycentric coordinates
		Barycentric( incident_point, Vector3d( v0->x, v0->y, v0->z ), Vector3d( v1->x, v1->y, v1->z ), Vector3d( v2->x, v2->y, v2->z ), bry_coords.x, bry_coords.y, bry_coords.z );

		// Check for out of bounds
		/*if (( bry_coords.x < 0 )||( bry_coords.x > 1 )||( bry_coords.y < 0 )||( bry_coords.y > 1 )||( bry_coords.z < 0 )||( bry_coords.z > 1 )) {
			continue;
		}*/
		//dif_bary_coords = Vector3d(0,0,0);
		//dif_bary_coords.x = (bry_coords.x < 0) ? (bry_coords.x) : ( (bry_coords.x > 1) ? (bry_coords.x-1) : 0 );
		//dif_bary_coords.y = (bry_coords.y < 0) ? (bry_coords.y) : ( (bry_coords.y > 1) ? (bry_coords.y-1) : 0 );
		//dif_bary_coords.z = (bry_coords.z < 0) ? (bry_coords.z) : ( (bry_coords.z > 1) ? (bry_coords.z-1) : 0 );
		dif_bary_coords = bry_coords;
		dif_bary_coords.x = (bry_coords.x > 0) ? std::max<ftype>(dif_bary_coords.x-1,0) : dif_bary_coords.x;
		dif_bary_coords.y = (bry_coords.y > 0) ? std::max<ftype>(dif_bary_coords.y-1,0) : dif_bary_coords.y;
		dif_bary_coords.z = (bry_coords.z > 0) ? std::max<ftype>(dif_bary_coords.z-1,0) : dif_bary_coords.z;

		// Check for range, and if this one is closer, set value
		//if ( min_param_t > param_t ) {

		// Check for closer barycentric coordinates
		//if ( (1+dif_bary_coords.sqrMagnitude())*sqr(param_t) < (1+min_dif_bary_coords.sqrMagnitude())*sqr(min_param_t) ) { //doesn't work because of tris near-parallel and close to ray
		if ( dif_bary_coords.sqrMagnitude()+sqr(param_t) < min_dif_bary_coords.sqrMagnitude()+sqr(min_param_t) ) {
			min_param_t = param_t;
			min_dif_bary_coords = dif_bary_coords;
			closestTri = tri;

			out_point = incident_point;
			out_bary = bry_coords;
		}
	}

	// No collision, return invalid index
	return closestTri;
}