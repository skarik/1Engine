
// ======== CWorldGen_Terrain_Architecture =======
// This is the definition for the test building creation

#include "core/math/Math.h"
#include "core/math/Math3d.h"
#include "core/math/noise/SimplexNoise.h"
#include "core/math/random/Random.h"

#include "core/types/ModelData.h"

#include "core-ext/containers/KDTree.h"

#include "renderer/logic/model/CModel.h"

#include "CWorldGen_Terran.h"

#include "after/entities/foliage/CTerraFoliageFactory.h"
#include "after/entities/props/CTerrainPropFactory.h"

#include "after/terrain/edit/csg/SidebufferVolumeEditor.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/types/terrain/BlockType.h"

#include "after/terrain/generation/CWorldGen_Terran_Architecture.h"

using namespace Terrain;
using namespace GeneratorUtilities;

Vector3d GeneratorUtilities::NearestPointOnTri(Vector3d pt, Vector3d a, Vector3d b, Vector3d c)
{
	Vector3d edge1 = b - a;
	Vector3d edge2 = c - a;
	Vector3d edge3 = c - b;
	float edge1Len = edge1.magnitude();
	float edge2Len = edge2.magnitude();
	float edge3Len = edge3.magnitude();
		
	Vector3d ptLineA = pt - a;
	Vector3d ptLineB = pt - b;
	Vector3d ptLineC = pt - c;
	Vector3d xAxis = edge1 / edge1Len;
	Vector3d zAxis = edge1.cross(edge2).normal();
	Vector3d yAxis = zAxis.cross(xAxis);
		
	Vector3d edge1Cross = edge1.cross(ptLineA);
	Vector3d edge2Cross = edge2.cross(-ptLineC);
	Vector3d edge3Cross = edge3.cross(ptLineB);
	bool edge1On = edge1Cross.dot(zAxis) > 0.0f;
	bool edge2On = edge2Cross.dot(zAxis) > 0.0f;
	bool edge3On = edge3Cross.dot(zAxis) > 0.0f;
		
	//	If the point is inside the triangle then return its coordinate.
	if (edge1On && edge2On && edge3On) {
		float xExtent = ptLineA.dot(xAxis);
		float yExtent = ptLineA.dot(yAxis);
		return a + xAxis * xExtent + yAxis * yExtent;
	}
		
	//	Otherwise, the nearest point is somewhere along one of the edges.
	Vector3d edge1Norm = xAxis;
	Vector3d edge2Norm = edge2.normal();
	Vector3d edge3Norm = edge3.normal();
		
	float edge1Ext = Math.Clamp(edge1Norm.dot(ptLineA), 0.0f, edge1Len);
	float edge2Ext = Math.Clamp(edge2Norm.dot(ptLineA), 0.0f, edge2Len);
	float edge3Ext = Math.Clamp(edge3Norm.dot(ptLineB), 0.0f, edge3Len);

	Vector3d edge1Pt = a + edge1Norm*edge1Ext;
	Vector3d edge2Pt = a + edge2Norm*edge2Ext;
	Vector3d edge3Pt = b + edge3Norm*edge3Ext;
		
	float sqDist1 = (pt - edge1Pt).sqrMagnitude();
	float sqDist2 = (pt - edge2Pt).sqrMagnitude();
	float sqDist3 = (pt - edge3Pt).sqrMagnitude();
		
	if (sqDist1 < sqDist2) {
		if (sqDist1 < sqDist3) {
			return edge1Pt;
		} else {
			return edge3Pt;
		}
	} else if (sqDist2 < sqDist3) {
		return edge2Pt;
	} else {
		return edge3Pt;
	}
}

int GeneratorUtilities::GetClosestTriangle ( const CModelData* md, const Vector3d_d& position )
{
	int closestTri = -1;
	for ( uint i = 0; i < md->triangleNum; ++i )
	{

	}
	return closestTri;
}
int GeneratorUtilities::GetClosestVertex ( const CModelData* md, const Vector3d_d& point )
{
	// convert point to local space
    /*float minDistanceSqr = 10000000;
	int nearestVertex = -1;

    // scan all vertices to find nearest
	for ( uint i = 0; i < md->vertexNum; ++i )
    {
		Vector3d_d diff = point-Vector3d_d( md->vertices[i].x, md->vertices[i].y, md->vertices[i].z );
		Real_d distSqr = diff.sqrMagnitude();

        if (distSqr < minDistanceSqr)
        {
            minDistanceSqr = distSqr;
            nearestVertex = i;
        }
    }

    // convert nearest vertex back to world space
    return nearestVertex;*/

	//	First, find the nearest vertex (the nearest point must be on one of the triangles
	//	that uses this vertex if the mesh is convex).
	static KDTree* vertProx = NULL;
	static std::atomic_flag stopflag = {0};
	while ( stopflag.test_and_set() ) {;}
	if ( vertProx == NULL ) {
		std::vector<Vector3d> points;
		points.resize( md->vertexNum );
		for ( uint i = 0; i < md->vertexNum; ++i )
		{
			points[i] = ( Vector3d( md->vertices[i].x, md->vertices[i].y, md->vertices[i].z ) );
		}
		std::cout << "thread lock: --KDTREE INITIALIZED--" << std::endl;
		vertProx = KDTree::MakeFromPoints( points );
		stopflag.clear();
	}
	else {
		stopflag.clear();
	}
	int nearest = vertProx->FindNearest(Vector3d(point.x,point.y,point.z));
	return nearest;
}

// Compute barycentric coordinates (u, v, w) for
// point p with respect to triangle (a, b, c)
//http://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
void GeneratorUtilities::Barycentric_ ( const Vector3d& p, const Vector3d& a, const Vector3d& b, const Vector3d& c, ftype &u, ftype &v, ftype &w)
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

void GeneratorUtilities::GetClosestMeshInfo( const CModelData* md, const Vector3d_d& point, Vector3d& o_pos, Vector3d& o_norm )
{
	// Create the KD tree we use
	/*static KDTree* vertProx = NULL;
	static std::atomic_flag stopflag = {0};
	while ( stopflag.test_and_set() ) {;}
	if ( vertProx == NULL ) {
		vector<Vector3d> points;
		points.resize( md->vertexNum );
		for ( uint i = 0; i < md->vertexNum; ++i )
		{
			points[i] = ( Vector3d( md->vertices[i].x, md->vertices[i].y, md->vertices[i].z ) );
		}
		std::cout << "thread lock: --KDTREE INITIALIZED--" << std::endl;
		vertProx = KDTree::MakeFromPoints( points );
		stopflag.clear();
	}
	else {
		stopflag.clear();
	}

	int nearest = vertProx->FindNearest(Vector3d(point.x,point.y,point.z));*/
	Real minSqDistance = 100000000.0f;
	Vector3d sourcePoint = Vector3d(point.x,point.y,point.z);
	Vector3d minPoint;
	uint targetTri;
	for ( uint tri = 0; tri < md->triangleNum; ++tri )
	{
		Vector3d points [3];
		Vector3d normals [3];
		for ( uint i = 0; i < 3; ++i ) {
			points[i] = Vector3d(
				md->vertices[md->triangles[tri].vert[i]].x,
				md->vertices[md->triangles[tri].vert[i]].y,
				md->vertices[md->triangles[tri].vert[i]].z );
			normals[i] = Vector3d(
				md->vertices[md->triangles[tri].vert[i]].nx,
				md->vertices[md->triangles[tri].vert[i]].ny,
				md->vertices[md->triangles[tri].vert[i]].nz );
		}
		Vector3d possibleNearestPoint = NearestPointOnTri( sourcePoint, points[0],points[1],points[2] );
		Vector3d possibleNearestNormal;
		{
			Vector3d bary_coords;
			Barycentric_( possibleNearestPoint, points[0],points[1],points[2], bary_coords.x,bary_coords.y,bary_coords.z );
			possibleNearestNormal = normals[0]*bary_coords.x + normals[1]*bary_coords.y + normals[2]*bary_coords.z;
		}
		Real possibleMinSqDistance = (sourcePoint-possibleNearestPoint).sqrMagnitude();

		// weight in the normal facing the right direction
		possibleMinSqDistance -= (sourcePoint-possibleNearestPoint).dot( possibleNearestNormal );

		if ( possibleMinSqDistance < minSqDistance ) {
			minPoint = possibleNearestPoint;
			minSqDistance = possibleMinSqDistance;
			targetTri = tri;
		}
	}

	Vector3d points [3];
	Vector3d normals [3];
	for ( uint i = 0; i < 3; ++i ) {
		points[i] = Vector3d(
			md->vertices[md->triangles[targetTri].vert[i]].x,
			md->vertices[md->triangles[targetTri].vert[i]].y,
			md->vertices[md->triangles[targetTri].vert[i]].z );
		normals[i] = Vector3d(
			md->vertices[md->triangles[targetTri].vert[i]].nx,
			md->vertices[md->triangles[targetTri].vert[i]].ny,
			md->vertices[md->triangles[targetTri].vert[i]].nz );
	}
	Vector3d bary_coords;
	Barycentric_( minPoint, points[0],points[1],points[2], bary_coords.x,bary_coords.y,bary_coords.z );

	o_pos = minPoint;
	o_norm = normals[0]*bary_coords.x + normals[1]*bary_coords.y + normals[2]*bary_coords.z;
}


Terrain::terra_b GeneratorUtilities::SampleModelVolume ( const CModelData* md, const Vector3d_d& point )
{
	static Terrain::terra_b* modelVolume = NULL;
	static std::atomic_flag stopflag = {0};
	while ( stopflag.test_and_set() ) {;}
	if ( modelVolume == NULL ) {
		modelVolume = new Terrain::terra_b [32*32*32];

		const Real_d modelScale = 1.0f;

		// Set initial values
		for ( int32_t x = 0; x < 32; ++x )
		{
			for ( int32_t y = 0; y < 32; ++y ) 
			{
				for ( int32_t z = 0; z < 32; ++z )
				{
					int t_indexer = x+y*32+z*32*32;
					modelVolume[t_indexer].raw = 0;
				}
			}
		}

		// Loop through the triangles to set voxels
		for ( int32_t tri = 0; tri < md->triangleNum; ++tri )
		{
			Vector3d verts [3];
			Vector3d normals [3];
			for ( uint i = 0; i < 3; ++i ) {
				verts[i] = Vector3d( md->vertices[md->triangles[tri].vert[i]].x, md->vertices[md->triangles[tri].vert[i]].y, md->vertices[md->triangles[tri].vert[i]].z );
				normals[i] = Vector3d( md->vertices[md->triangles[tri].vert[i]].nx, md->vertices[md->triangles[tri].vert[i]].ny, md->vertices[md->triangles[tri].vert[i]].nz );
			}
			ftype abStep = 1.0f / std::max<ftype>( std::max<ftype>( fabs(verts[0].x-verts[1].x), fabs(verts[0].y-verts[1].y) ), fabs(verts[0].z-verts[1].z) );
			ftype acStep = 1.0f / std::max<ftype>( std::max<ftype>( fabs(verts[0].x-verts[2].x), fabs(verts[0].y-verts[2].y) ), fabs(verts[0].z-verts[2].z) );
			for ( ftype ab = 0; ab < 2; ab += abStep )
			{
				for ( ftype ac = 0; ac < 2; ac += acStep )
				{
					Vector3d targetPosition = (verts[0].lerp( verts[1], ab )).lerp( verts[2], ac );
					Vector3d targetNormal = (normals[0].lerp( normals[1], ab )).lerp( normals[2], ac );
					// Turn targetPosition to integer index
					targetPosition = targetPosition*modelScale + Vector3d( 16,16,16 );
					int32_t subX = targetPosition.x;
					int32_t subY = targetPosition.y;
					int32_t subZ = targetPosition.z;
					if ( subX < 0 || subX >= 32 ) continue;
					if ( subY < 0 || subY >= 32 ) continue;
					if ( subZ < 0 || subZ >= 32 ) continue;

					int32_t t_indexer = subX+subY*32+subZ*32*32;

					modelVolume[t_indexer].block = Terrain::EB_STONEBRICK;

					targetNormal.normalize();
					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( targetNormal.x );
					normal_y = Terrain::_normal_unbias( targetNormal.y );
					normal_z = Terrain::_normal_unbias( targetNormal.z );
					modelVolume[t_indexer].normal_y_x = normal_x;
					modelVolume[t_indexer].normal_z_x = normal_x;
					modelVolume[t_indexer].normal_x_y = normal_y;
					modelVolume[t_indexer].normal_z_y = normal_y;
					modelVolume[t_indexer].normal_x_z = normal_z;
					modelVolume[t_indexer].normal_y_z = normal_z;

					if ( targetNormal.x < 0 ) {
						modelVolume[t_indexer].fluid_level = 1;
					}
					else {
						modelVolume[t_indexer].fluid_level = 2;
					}

					if ( ac > 1 ) {
						break;
					}
				}

				if ( ab > 1 ) {
					break;
				}
			}
		}

		// Set normals now.
		for ( int32_t x = 0; x < 32; ++x )
		{
			for ( int32_t y = 0; y < 32; ++y ) 
			{
				for ( int32_t z = 0; z < 32; ++z )
				{
					int t_indexer = x+y*32+z*32*32;
					/*modelVolume[t_indexer].raw = 0;

					Vector3d_d samplePosition = Vector3d_d(x-16,y-16,z-16);
					const Real_d skip_bias = 0.5f;//COctreeTerrain::blockSizae * 0.25f;

					int closestVert = GetClosestVertex( md, samplePosition );
					if ( closestVert == -1 ) continue;
					Vector3d pos( md->vertices[closestVert].x, md->vertices[closestVert].y, md->vertices[closestVert].z );
					Vector3d norm( md->vertices[closestVert].nx, md->vertices[closestVert].ny, md->vertices[closestVert].nz );
					if ( fabs(samplePosition.x-pos.x) > skip_bias ) continue;
					if ( fabs(samplePosition.y-pos.y) > skip_bias ) continue;
					if ( fabs(samplePosition.z-pos.z) > skip_bias ) continue;*/
					if ( modelVolume[t_indexer].raw == 0 ) continue;

					//modelVolume[t_indexer].block = EB_STONEBRICK;

					Vector3d_d samplePosition = Vector3d_d(x-16,y-16,z-16);
					int closestVert;// = GetClosestVertex( md, samplePosition );

					// Do the 3X sample
					const Real_d bias = 0.5;
					Vector3d_d samplePosX = samplePosition; samplePosX.x += bias;
					Vector3d_d samplePosY = samplePosition; samplePosY.y += bias;
					Vector3d_d samplePosZ = samplePosition; samplePosZ.z += bias;
 
					{	// X direction
						//closestVert = GetClosestVertex( md, samplePosX );
						//Vector3d pos( md->vertices[closestVert].x, md->vertices[closestVert].y, md->vertices[closestVert].z );
						//Vector3d norm( md->vertices[closestVert].nx, md->vertices[closestVert].ny, md->vertices[closestVert].nz );
						Vector3d pos;
						Vector3d norm;
						GetClosestMeshInfo( md, samplePosX, pos, norm );

						//modelVolume[t_indexer].normal_x_x = Terrain::_normal_unbias( norm.x );
						if ( norm.x < 0 ) {
							modelVolume[t_indexer].fluid_level = 1;
						}
						else {
							modelVolume[t_indexer].fluid_level = 2;
						}
						modelVolume[t_indexer].normal_x_y = Terrain::_normal_unbias( norm.y );
						modelVolume[t_indexer].normal_x_z = Terrain::_normal_unbias( norm.z );
						modelVolume[t_indexer].normal_x_w = Terrain::_depth_unbias( (pos.x - samplePosition.x) );
					}

					{	// Y direction
						//closestVert = GetClosestVertex( md, samplePosY );
						//Vector3d pos( md->vertices[closestVert].x, md->vertices[closestVert].y, md->vertices[closestVert].z );
						//Vector3d norm( md->vertices[closestVert].nx, md->vertices[closestVert].ny, md->vertices[closestVert].nz );
						Vector3d pos;
						Vector3d norm;
						GetClosestMeshInfo( md, samplePosY, pos, norm );

						modelVolume[t_indexer].normal_y_x = Terrain::_normal_unbias( norm.x );
						//modelVolume[t_indexer].normal_y_y = Terrain::_normal_unbias( norm.y );
						modelVolume[t_indexer].normal_y_z = Terrain::_normal_unbias( norm.z );
						modelVolume[t_indexer].normal_y_w = Terrain::_depth_unbias( (pos.y - samplePosition.y) );
					}

					{	// Z direction
						//closestVert = GetClosestVertex( md, samplePosZ );
						//Vector3d pos( md->vertices[closestVert].x, md->vertices[closestVert].y, md->vertices[closestVert].z );
						//Vector3d norm( md->vertices[closestVert].nx, md->vertices[closestVert].ny, md->vertices[closestVert].nz );
						Vector3d pos;
						Vector3d norm;
						GetClosestMeshInfo( md, samplePosZ, pos, norm );

						modelVolume[t_indexer].normal_z_x = Terrain::_normal_unbias( norm.x );
						modelVolume[t_indexer].normal_z_y = Terrain::_normal_unbias( norm.y );
						//modelVolume[t_indexer].normal_z_z = Terrain::_normal_unbias( norm.z );
						modelVolume[t_indexer].normal_z_w = Terrain::_depth_unbias( (pos.z - samplePosition.z) );
					}
					
				}
			}
		}

		// Fill in the empty voxels (based on method detailed by Wolfire Games)
		for ( int32_t z = 0; z < 32; ++z ) 
		{
			bool penDown = false;
			for ( int32_t y = 0; y < 32; ++y ) 
			{
				for ( int32_t x = 0; x < 32; ++x )
				{
					int t_indexer = x+y*32+z*32*32;
					if ( !penDown )
					{
						if ( modelVolume[t_indexer].fluid_level == 1 )
						{
							// First check for an endpoint
							bool canWrite = false;
							for ( int32_t xc = x+1; xc < 32; ++xc ) {
								int t_nindexer = xc+y*32+z*32*32;
								if ( modelVolume[t_nindexer].fluid_level == 2 ) {
									canWrite = true;
									break;
								}
								else if ( modelVolume[t_nindexer].fluid_level == 1 ) {
									break; // Hit invalid, do not write
								}
							}
							// If can write then write 
							if ( canWrite ) {
								penDown = true;
							}
						}
						modelVolume[t_indexer].fluid_level = 0;
					}
					else
					{
						// Check for opposite facing normal
						modelVolume[t_indexer].block = Terrain::EB_STONEBRICK;
						if ( modelVolume[t_indexer].fluid_level == 2 )
						{
							penDown = false;
						}
						modelVolume[t_indexer].fluid_level = 0;
					}
				}
			}
		}
		// Reduce voxels for the air values.
		for ( int32_t x = 1; x < 31; ++x )
		{
			for ( int32_t y = 1; y < 31; ++y ) 
			{
				for ( int32_t z = 1; z < 31; ++z )
				{
					int t_pindexer, t_nindexer;
					int t_indexer = x+y*32+z*32*32;
					if ( modelVolume[t_indexer].raw == 0 ) continue;
					
					t_nindexer = (x+1)+y*32+z*32*32;
					if ( modelVolume[t_nindexer].raw != 0 ) continue;
					t_nindexer = x+(y+1)*32+z*32*32;
					if ( modelVolume[t_nindexer].raw != 0 ) continue;
					t_nindexer = x+y*32+(z+1)*32*32;
					if ( modelVolume[t_nindexer].raw != 0 ) continue;

					t_pindexer = (x-1)+y*32+z*32*32;
					if ( modelVolume[t_pindexer].raw == 0 ) {
						modelVolume[t_indexer].block = Terrain::EB_NONE;
						continue;
					}
					t_pindexer = x+(y-1)*32+z*32*32;
					if ( modelVolume[t_pindexer].raw == 0 ) {
						modelVolume[t_indexer].block = Terrain::EB_NONE;
						continue;
					}
					t_pindexer = x+y*32+(z-1)*32*32;
					if ( modelVolume[t_pindexer].raw == 0 ) {
						modelVolume[t_indexer].block = Terrain::EB_NONE;
						continue;
					}
				}
			}
		}
		std::cout << "thread lock: --MODEL VOLUME INITIALIZED--" << std::endl;
		stopflag.clear();
	}
	else {
		stopflag.clear();
	}

	Terrain::terra_b result;
	result.raw = 0;

	const Real_d scale = Terrain::BlockSize;

	int sx = (point.x/scale)+16;
	if ( sx < 0 || sx >= 32 ) return result;
	int sy = (point.y/scale)+16;
	if ( sy < 0 || sy >= 32 ) return result;
	int sz = (point.z/scale)+16;
	if ( sz < 0 || sz >= 32 ) return result;

	result = modelVolume[sx+sy*32+sz*32*32];

	return result;
}

// Generate_Architecture_Test() : Generate test architecture.
void Terrain::CWorldGen_Terran::Generate_Architecture_Test ( const quickAccessData& qd )
{
	// Select model to instance
	CModel* model = (CModel*)(m_model);
	CModelData* md = model->GetModelData(0);

	// Create all the temporary variables
	Terrain::terra_b block;
	Real_d x,y,z;
	int32_t subX, subY, subZ;
	Real stepX, stepY, stepZ;
	// Calculate step size
	stepX = (Real)((qd.pMax.x - qd.pMin.x)/32);
	stepY = (Real)((qd.pMax.y - qd.pMin.y)/32);
	stepZ = (Real)((qd.pMax.z - qd.pMin.z)/32);

	// Loop through sector buffer and set data
	/*for ( subX = 0; subX < 32; ++subX )
	{
		x = subX*stepX + qd.pMin.x + COctreeTerrain::blockSize*0.5;
		for ( subY = 0; subY < 32; ++subY )
		{
			y = subY*stepY + qd.pMin.y + COctreeTerrain::blockSize*0.5;
			for ( subZ = 0; subZ < 32; ++subZ )
			{
				z = subZ*stepZ + qd.pMin.z + COctreeTerrain::blockSize*0.5;

				// Look at model and find closest triangle.
				//int closestTri = GetClosestTriangle( md, Vector3d_d( x,y,z ) );
				Terrain::terra_b newBlock = SampleModelVolume( md, Vector3d_d( x,y,z-44 ) );
				if ( newBlock.raw == 0 ) continue;

				// Get block.
				qd.accessor->GetDataAtSidebufferPosition( block.raw, RangeVector(subX,subY,subZ) );
				
				block.raw = newBlock.raw;

				// Set block.
				qd.accessor->SetDataAtSidebufferPosition( block.raw, RangeVector(subX,subY,subZ) );
			}
		}
	}*/
	/*
	qd.editor->Add_Box( Vector3d_d( 10,10,28 ), Vector3d_d( 4,12,8 ), EB_STONEBRICK );

	qd.editor->Add_Box( Vector3d_d( -2,-20,36 ), Vector3d_d( 4,12+15,16 ), EB_STONEBRICK );
	qd.editor->Add_Box( Vector3d_d( 10,-10,36 ), Vector3d_d( 4,12,16 ), EB_STONEBRICK );

	qd.editor->Add_Box( Vector3d_d( 16,-25,28 ), Vector3d_d( 9,12,8 ), EB_STONEBRICK );

	qd.editor->Add_Box( Vector3d_d( 70,-25,28 ), Vector3d_d( 18,12,8 ), EB_STONEBRICK );
	*/

	struct Box_t
	{
		Vector3d_d position;
		Vector3d_d halfsize;
	};

	static std::atomic_flag lock = {0};
	static std::vector<Real_d> heights;
	static std::vector<Box_t> boxes;

	Vector3d_d centerOffset ( 420, 190, 0 );

	while ( lock.test_and_set() ) {;}
	if ( heights.empty() )
	{
		// Build the heights
		for ( uint i = 0; i < 4; ++i ) {
			//Real_d height = 28 + 9 + i*8;
			Real_d height = 28 + 9 + i*11;
			height += Random.Range( -9.0f, 9.0f );
			heights.push_back( height );
		}

		// Create structure bases
		for ( uint blockNumber = 0; blockNumber < 23; ++blockNumber ) {
			Vector3d_d nextPosition, nextHalfsize;
			Real_d nextHeight;
			
			// Choose position
			nextPosition.x = Random.Range( -170,170 );
			nextPosition.y = Random.Range( -170,170 );
			nextPosition.z = Random.Range( 0,40 );
			nextPosition += centerOffset;

			// Choose size
			nextHalfsize.x = Random.Range( 2,30 );
			nextHalfsize.y = Random.Range( 2,30 );
			nextHalfsize.z = Random.Range( 2,30*Random.Range(0.5,1.0) );

			// Set height
			nextHeight = nextPosition.z + nextHalfsize.z;

			// 75% chance to snap the height to the nearest div
			if ( Random.Range(0,1) < .75f ) {
				// Look for the nearest height
				Real_d nearestHeight = heights[0];
				for ( uint i = 1; i < heights.size(); ++i )
				{
					if ( abs( nextHeight - heights[i] ) < abs( nextHeight - nearestHeight ) ) {
						nearestHeight = heights[i];
					}
				}
				nextHeight = nearestHeight;
			}

			// 50% chance to edit the height
			if ( Random.Range(0,1) < .50f ) {
				// Set up the block to go from the ground to the height
				Real_d groundLevel = 0;
				nextPosition.z = (nextHeight + groundLevel)/2;
				nextHalfsize.z = (nextHeight - groundLevel)/2;
			}
			else {
				// Set up the block to touch the top of the desired height
				nextPosition.z = nextHeight - nextHalfsize.z;
			}

			// Add the block
			{
				Box_t nextBlock;
				nextBlock.position = nextPosition;
				nextBlock.halfsize = nextHalfsize;
				boxes.push_back( nextBlock );
			}

			// 20% chance to add a column in the middle of it
			Real chance = Random.Range(0,1);
			if ( chance < 0.20f )
			{
				Box_t nextBlock;
				nextBlock.halfsize.x = Random.Range( 2,6 );
				nextBlock.halfsize.y = nextBlock.halfsize.x * Random.Range( 0.5,2.0 );
				nextBlock.halfsize.z *= Random.Range( 1.6,3.4 );
				boxes.push_back( nextBlock );
			}
			else if ( chance < 0.70f ) // 50% chance to put columns on every corner
			{
				// Put a column on each corner
				Box_t nextBlock;
				nextBlock.halfsize.x = Random.Range( 2,6 );
				nextBlock.halfsize.y = nextBlock.halfsize.x;
				nextBlock.halfsize.z *= Random.Range( 1.5,2.2 );

				nextBlock.position.x = nextPosition.x + nextHalfsize.x;
				nextBlock.position.y = nextPosition.y + nextHalfsize.y;
				boxes.push_back( nextBlock );
				nextBlock.position.x = nextPosition.x - nextHalfsize.x;
				nextBlock.position.y = nextPosition.y + nextHalfsize.y;
				boxes.push_back( nextBlock );
				nextBlock.position.x = nextPosition.x + nextHalfsize.x;
				nextBlock.position.y = nextPosition.y - nextHalfsize.y;
				boxes.push_back( nextBlock );
				nextBlock.position.x = nextPosition.x - nextHalfsize.x;
				nextBlock.position.y = nextPosition.y - nextHalfsize.y;
				boxes.push_back( nextBlock );
			}

			// 60% chance to add a child
			while ( Random.Range(0,1) < 0.60f )
			{
				// Choose position
				nextPosition.x += Random.Range( -nextHalfsize.x,nextHalfsize.x );
				nextPosition.y += Random.Range( -nextHalfsize.y,nextHalfsize.y );
				nextPosition.z += Random.Range( -nextHalfsize.z,nextHalfsize.z );

				// Choose size
				nextHalfsize.x *= Random.Range( 0.5,1.0 );
				nextHalfsize.y *= Random.Range( 0.5,1.0 );
				nextHalfsize.z *= Random.Range( 0.3,1.0 );

				// Set height
				nextHeight = nextPosition.z + nextHalfsize.z;

				// 75% chance to snap the height to the nearest div
				if ( Random.Range(0,1) < .75f ) {
					// Look for the nearest height
					Real_d nearestHeight = heights[0];
					for ( uint i = 1; i < heights.size(); ++i )
					{
						if ( abs( nextHeight - heights[i] ) < abs( nextHeight - nearestHeight ) ) {
							nearestHeight = heights[i];
						}
					}
					nextHeight = nearestHeight;
				}

				// 50% chance to edit the height
				if ( Random.Range(0,1) < .50f ) {
					// Set up the block to go from the ground to the height
					Real_d groundLevel = 0;
					nextPosition.z = (nextHeight + groundLevel)/2;
					nextHalfsize.z = (nextHeight - groundLevel)/2;
				}
				else {
					// Set up the block to touch the top of the desired height
					nextPosition.z = nextHeight - nextHalfsize.z;
				}

				// Add the block
				Box_t nextBlockB;
				nextBlockB.position = nextPosition;
				nextBlockB.halfsize = nextHalfsize;
				boxes.push_back( nextBlockB );

				// 10% chance to add a column in the middle of it
				Real chance = Random.Range(0,1);
				if ( chance < 0.10f )
				{
					Box_t nextBlock;
					nextBlock.halfsize.x = Random.Range( 2,6 );
					nextBlock.halfsize.y = nextBlock.halfsize.x * Random.Range( 0.5,2.0 );
					nextBlock.halfsize.z *= Random.Range( 1.6,3.4 );
					boxes.push_back( nextBlock );
				}
				else if ( chance < 0.50f ) // 40% chance to put columns on every corner
				{
					// Put a column on each corner
					Box_t nextBlock;
					nextBlock.halfsize.x = Random.Range( 2,6 );
					nextBlock.halfsize.y = nextBlock.halfsize.x;
					nextBlock.halfsize.z *= Random.Range( 1.5,2.2 );

					nextBlock.position.x = nextPosition.x + nextHalfsize.x;
					nextBlock.position.y = nextPosition.y + nextHalfsize.y;
					boxes.push_back( nextBlock );
					nextBlock.position.x = nextPosition.x - nextHalfsize.x;
					nextBlock.position.y = nextPosition.y + nextHalfsize.y;
					boxes.push_back( nextBlock );
					nextBlock.position.x = nextPosition.x + nextHalfsize.x;
					nextBlock.position.y = nextPosition.y - nextHalfsize.y;
					boxes.push_back( nextBlock );
					nextBlock.position.x = nextPosition.x - nextHalfsize.x;
					nextBlock.position.y = nextPosition.y - nextHalfsize.y;
					boxes.push_back( nextBlock );
				}
			} // End children
		}

		// Build structure bridges
		const uint boxCount = boxes.size();
		for ( uint i = 0; i < boxCount; ++i )
		{
			Real_d c_blockHeight;
			c_blockHeight = boxes[i].position.z + boxes[i].halfsize.z;
			// Look and see if this is nearby a step (search for nearest height)
			Real_d c_nearestHeight = heights[0];
			for ( uint i = 1; i < heights.size(); ++i )
			{
				if ( abs( c_blockHeight - heights[i] ) < abs( c_blockHeight - c_nearestHeight ) ) {
					c_nearestHeight = heights[i];
				}
			}

			// Skip if not nearby
			if ( abs( c_blockHeight-c_nearestHeight ) > 2 )
			{
				continue;
			}

			// Look for nearby boxes
			for ( uint j = 0; j < boxCount; ++j )
			{
				// Skip self
				if ( i == j ) {
					continue;
				}
				// Check that it's close enough
				if ( (boxes[i].position - boxes[j].position).sqrMagnitude() > sqr(90) ) {
					continue;
				}
				// Check that it's connected at a right angle
				if ( abs(boxes[i].position.x-boxes[j].position.x) > boxes[i].halfsize.x && abs(boxes[i].position.x-boxes[j].position.x) > boxes[j].halfsize.x ) {
					continue;
				}
				if ( abs(boxes[i].position.y-boxes[j].position.y) > boxes[i].halfsize.y && abs(boxes[i].position.y-boxes[j].position.y) > boxes[j].halfsize.y ) {
					continue;
				}
				// Check its height
				Real_d t_blockHeight;
				t_blockHeight = boxes[j].position.z + boxes[j].halfsize.z;
				/*if ( abs( t_blockHeight - c_blockHeight ) > 3 ) {
					continue;
				}*/
				if ( c_blockHeight - t_blockHeight > 3 ) {
					continue;
				}

				// 75% chance to make a bridge
				if ( Random.Range(0,1) < 0.75f )
				{
					Vector3d_d nextPosition, nextHalfsize;

					nextHalfsize.x = abs(boxes[i].position.x-boxes[j].position.x)/2 * Random.Range(0.5,1) + 2;
					nextHalfsize.y = abs(boxes[i].position.y-boxes[j].position.y)/2 * Random.Range(0.5,1) + 2;
					nextHalfsize.z = Random.Range( 1,2.5 );

					nextPosition = (boxes[i].position + boxes[j].position)/2;
					//nextPosition.z = (t_blockHeight+c_blockHeight)/2 - nextHalfsize.z;
					nextPosition.z = c_blockHeight - nextHalfsize.z;

					// Add the block
					Box_t nextBlock;
					nextBlock.position = nextPosition;
					nextBlock.halfsize = nextHalfsize;
					boxes.push_back( nextBlock );
				}
				
			}
			// Done check
		} // Done bridges

		// Add some small spires
		for ( uint i = 0; i < 18; ++i )
		{
			Vector3d_d nextPosition, nextHalfsize;
			Real_d nextHeight;
			
			// Choose position
			nextPosition.x = Random.Range( -170,170 );
			nextPosition.y = Random.Range( -170,170 );
			nextPosition.z = Random.Range( 0,40 );
			nextPosition += centerOffset;

			// Choose size
			nextHalfsize.x = Random.Range( 2,5 );
			nextHalfsize.y = Random.Range( 2,5 );
			nextHalfsize.z = Random.Range( 30,80 ) * Random.Range( 0.5, 1.5 );

			Box_t nextBlock;
			nextBlock.position = nextPosition;
			nextBlock.halfsize = nextHalfsize;
			boxes.push_back( nextBlock );
		}
		
		// Everything is done, go to generation
	}
	lock.clear();

	// FUCK IT, ADD A CAVE!
	//qd.editor->Sub_Line( Vector3d_d( 0,0,28 ), 
	qd.editor->Mak_Line( Vector3d_d( 0,0,28 ), Vector3d_d( 50,50,-36 ), 5.5, Terrain::EB_NONE );

	for ( uint i = 0; i < boxes.size(); ++i )
	{
		qd.editor->Add_Box( boxes[i].position, boxes[i].halfsize, Terrain::EB_STONEBRICK );
	}


	/*if ( qd.indexer.x == -1 && qd.indexer.y == -1 && qd.indexer.z == 0 )
	{
		sTerraProp newprop = {0};
		newprop.component_index = TerrainProp::GetComponentType( "PropCampfire" );
		newprop.position = Vector3d( 32,57.6, 27 );
		newprop.rotation = Vector3d( 0,0,0 );
		newprop.scaling = Vector3d( 1.3f,1.3f,1.3f );
		newprop.userdata = NULL;
		qd.gamedata->m_component_queue.push_back( newprop );
	}*/
}