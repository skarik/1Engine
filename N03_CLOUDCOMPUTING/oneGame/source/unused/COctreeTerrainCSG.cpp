
#include "COctreeTerrain.h"
#include "COctreeRenderer.h"
#include "Math.h"

bool COctreeTerrain::CSG_SetBlock ( const Vector3d& nCenter, const ushort blockType )
{
	Terrain::terra_b block;
	SampleBlock( nCenter, block.raw );
	{
		block.block = blockType;
		block.normal_x_w = Terrain::_depth_unbias( 0.5f );
		block.normal_y_w = Terrain::_depth_unbias( 0.5f );
		block.normal_z_w = Terrain::_depth_unbias( 0.5f );
		block.normal_x_y = Terrain::_normal_unbias( 0 );
		block.normal_x_z = Terrain::_normal_unbias( 0 );
		block.normal_y_x = Terrain::_normal_unbias( 0 );
		block.normal_y_z = Terrain::_normal_unbias( 0 );
		block.normal_z_y = Terrain::_normal_unbias( 0 );
		block.normal_z_y = Terrain::_normal_unbias( 0 );
		block.smooth_normal = 0;
	}
	SetDataAtPositionMinimal( block.raw, nCenter );


	SampleBlock( nCenter - Vector3d( blockSize,0,0 ), block.raw );
	{
		block.normal_x_w = Terrain::_depth_unbias( 0.5f );
		block.normal_x_y = Terrain::_normal_unbias( 0 );
		block.normal_x_z = Terrain::_normal_unbias( 0 );
	}
	SetDataAtPositionMinimal( block.raw, nCenter - Vector3d( blockSize,0,0 ) );

	SampleBlock( nCenter - Vector3d( 0,blockSize,0 ), block.raw );
	{
		block.normal_y_w = Terrain::_depth_unbias( 0.5f );
		block.normal_y_x = Terrain::_normal_unbias( 0 );
		block.normal_y_z = Terrain::_normal_unbias( 0 );
	}
	SetDataAtPositionMinimal( block.raw, nCenter - Vector3d( 0,blockSize,0 ) );

	SampleBlock( nCenter - Vector3d( 0,0,blockSize ), block.raw );
	{
		block.normal_z_w = Terrain::_depth_unbias( 0.5f );
		block.normal_z_x = Terrain::_normal_unbias( 0 );
		block.normal_z_y = Terrain::_normal_unbias( 0 );
	}
	SetDataAtPositionMinimal( block.raw, nCenter - Vector3d( 0,0,blockSize ) );

	Sectors_Request_NewMesh( nCenter );

	return true;
}

void COctreeTerrain::CSG_AddBlock ( const Vector3d& nCenter, const Vector3d& nHalfExtents )
{
/*	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d( 0,0,m_treeStartResolution*blockSize*0.5*0.5 );
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	Terrain::terra_b tSampler;

	tTracker = tTrackerSource;
	while ( !SeekToPosition( tTracker, nCenter ) ) {}
	tSampler.raw = tTracker.sector->links[tTracker.bindex];

	tSampler.normal_w = 1;
	tSampler.block = EB_STONE;

	SetDataAtPositionMinimal( tSampler.raw, nCenter );

	for ( Real x = 0; x < nHalfExtents.x+blockSize*2; x += blockSize )
	{
		for ( Real y = 0; y < nHalfExtents.y+blockSize*2; y += blockSize )
		{
			for ( Real z = 0; z < nHalfExtents.z+blockSize*2; z += blockSize )
			{
				//
				for ( int dirX = -1; dirX <= 1; dirX += 2 )
				{
					for ( int dirY = -1; dirY <= 1; dirY += 2 )
					{
						for ( int dirZ = -1; dirZ <= 1; dirZ += 2 )
						{
							//
							tTracker = tTrackerSource;

							Vector3d tTarget = nCenter + Vector3d(x*dirX,y*dirY,z*dirZ);
							while ( !SeekToPosition( tTracker, tTarget ) ) {}
							tSampler.raw = tTracker.sector->links[tTracker.bindex];

							if ( !m_renderer->BlocktypeOpaque(tSampler.block) )
							{
								tSampler.normal_x = 7;
								tSampler.normal_y = 7;
								tSampler.normal_z = 7;

								bool useX = false, useY = false, useZ = false;
								int edgeCount = 0;
								if ( x > nHalfExtents.x ) edgeCount++;
								if ( y > nHalfExtents.y ) edgeCount++;
								if ( z > nHalfExtents.z ) edgeCount++;
								if ( edgeCount >= 2 )
								{
									if ( dirX*dirY > 0 ) {
										if ( dirX*dirZ > 0 ) {
											useZ = true;
										}
										else {
											useX = true;
										}
									}
									else {
										if ( dirY*dirZ > 0 ) {
											useZ = true;
										}
										else {
											useY = true;
										}
									}
								}
								else {
									if ( x > y && x > z ) {
										useX = true;
									}
									else if ( y > z ) {
										useY = true;
									}
									else {
										useZ = true;
									}
								}

								if ( useX ) {
									tSampler.normal_x = 7 + 6*dirX;
								}
								else if ( useY ) {
									tSampler.normal_y = 7 + 6*dirY;
								}
								else if ( useZ ) { 
									tSampler.normal_z = 7 + 6*dirZ;
								}

								if ( x < nHalfExtents.x + blockSize && y < nHalfExtents.y + blockSize && z < nHalfExtents.z + blockSize )
								{
									tSampler.block = EB_STONE;
									tSampler.normal_w = 0;
								}
								SetDataAtPositionMinimal( tSampler.raw, tTarget );
							}
							//
						}
					}
				}
				//
			}
		}
	}*/
}

void COctreeTerrain::CSG_RemoveBlock ( const Vector3d& nCenter, const Vector3d& nHalfExtents )
{
/*	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d( 0,0,m_treeStartResolution*blockSize*0.5*0.5 );
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	Terrain::terra_b tSampler;

	tTracker = tTrackerSource;
	while ( !SeekToPosition( tTracker, nCenter ) ) {}
	tSampler.raw = tTracker.sector->links[tTracker.bindex];

	tSampler.normal_x = 0; // mark as recreate normals
	tSampler.normal_w = 0;
	tSampler.block = EB_NONE;

	SetDataAtPositionMinimal( tSampler.raw, nCenter );

	for ( Real x = 0; x < nHalfExtents.x+blockSize; x += blockSize )
	{
		for ( Real y = 0; y < nHalfExtents.y+blockSize; y += blockSize )
		{
			for ( Real z = 0; z < nHalfExtents.z+blockSize; z += blockSize )
			{
				//
				for ( int dirX = -1; dirX <= 1; dirX += 2 )
				{
					for ( int dirY = -1; dirY <= 1; dirY += 2 )
					{
						for ( int dirZ = -1; dirZ <= 1; dirZ += 2 )
						{
							//
							tTracker = tTrackerSource;

							Vector3d tTarget = nCenter + Vector3d(x*dirX,y*dirY,z*dirZ);
							while ( !SeekToPosition( tTracker, tTarget ) ) {}
							tSampler.raw = tTracker.sector->links[tTracker.bindex];

							tSampler.normal_x = 7;
							tSampler.normal_y = 7;
							tSampler.normal_z = 7;

							if ( x > y && x > z )
								tSampler.normal_x = 7 - 6*dirX;
							else if ( y > z )
								tSampler.normal_y = 7 - 6*dirY;
							else
								tSampler.normal_z = 7 - 6*dirZ;

							tSampler.normal_w = 0;
							if ( x >= nHalfExtents.x ) {
								if ( dirX > 0 )
									tSampler.normal_w = (1-abs(fmod( tTarget.x, blockSize ) / blockSize))*7;
								else
									tSampler.normal_w = (abs(fmod( tTarget.x, blockSize ) / blockSize))*7;
								tSampler.normal_w = 7-tSampler.normal_w;
							}
							if ( y >= nHalfExtents.y ) {
								if ( dirY > 0 )
									tSampler.normal_w = (1-abs(fmod( tTarget.y, blockSize ) / blockSize))*7;
								else
									tSampler.normal_w = (abs(fmod( tTarget.y, blockSize ) / blockSize))*7;
								tSampler.normal_w = 7-tSampler.normal_w;
							}
							if ( z >= nHalfExtents.z ) {
								if ( dirZ > 0 )
									tSampler.normal_w = (1-abs(fmod( tTarget.z, blockSize ) / blockSize))*7;
								else
									tSampler.normal_w = (abs(fmod( tTarget.z, blockSize ) / blockSize))*7;
								tSampler.normal_w = 7-tSampler.normal_w;
							}
							
							tSampler.block = EB_NONE;

							SetDataAtPositionMinimal( tSampler.raw, tTarget );
							//
						}
					}
				}
				//
			}
		}
	}*/
}


void COctreeTerrain::CSG_RemoveSphere ( const Vector3d& nCenter, const Real& nRadius )
{
/*	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d( 0,0,m_treeStartResolution*blockSize*0.5*0.5 );
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	Terrain::terra_b tSampler;

	tTracker = tTrackerSource;
	while ( !SeekToPosition( tTracker, nCenter ) ) {}
	tSampler.raw = tTracker.sector->links[tTracker.bindex];

	tSampler.normal_x = 0; // mark as recreate normals
	tSampler.normal_w = 0;
	tSampler.block = EB_NONE;

	SetDataAtPositionMinimal( tSampler.raw, nCenter );

	for ( Real x = 0; x < nRadius+blockSize; x += blockSize )
	{
		for ( Real y = 0; y < nRadius+blockSize; y += blockSize )
		{
			for ( Real z = 0; z < nRadius+blockSize; z += blockSize )
			{
				Real sqrRadius = Vector3d(x,y,z).sqrMagnitude();
				if ( sqrRadius < sqr(nRadius+blockSize) )
				{
					//
					for ( int dirX = -1; dirX <= 1; dirX += 2 )
					{
						for ( int dirY = -1; dirY <= 1; dirY += 2 )
						{
							for ( int dirZ = -1; dirZ <= 1; dirZ += 2 )
							{
								//
								tTracker = tTrackerSource;

								Vector3d tTarget = nCenter + Vector3d(x*dirX,y*dirY,z*dirZ);
								while ( !SeekToPosition( tTracker, tTarget ) ) {}
								tSampler.raw = tTracker.sector->links[tTracker.bindex];

								Vector3d currentNormal (-x*dirX,-y*dirY,-z*dirZ);
								currentNormal.normalize();
								tSampler.normal_x = ((int)((currentNormal.x+1)*6))+1;
								tSampler.normal_y = ((int)((currentNormal.y+1)*6))+1;
								tSampler.normal_z = ((int)((currentNormal.z+1)*6))+1;
								//tSampler.normal_x = 0;

								tSampler.normal_w = 0;
								if ( sqrRadius >= sqr(nRadius) )
								{
									// Need to get distance from grid point to sphere outside
									Vector3d tOffsetDistance;
									tOffsetDistance.x = abs(fmod(nCenter.x,blockSize));
									tOffsetDistance.y = abs(fmod(nCenter.y,blockSize));
									tOffsetDistance.z = abs(fmod(nCenter.z,blockSize));
								//	tTarget.x -= fmod(
								//	tSampler.normal_w = ((sqrt(sqrRadius)-nRadius)/blockSize)*7;

									//tSampler.normal_w = (tOffsetDistance.magnitude()/blockSize)*7;
									//tSampler.normal_w = ((abs(tOffsetDistance.x)+abs(tOffsetDistance.y)+abs(tOffsetDistance.z))/(blockSize*3))*7;

									tSampler.normal_w = (((sqrt(sqrRadius)-nRadius)+tOffsetDistance.dot(Vector3d(x,y,z).normal()))/blockSize)*15;
									tSampler.normal_w = std::min<unsigned>( tSampler.normal_w, 15 );
									//tSampler.normal_w = 7-tSampler.normal_w;
								}
								else {
									tSampler.block = EB_NONE;
								}
							

								SetDataAtPositionMinimal( tSampler.raw, tTarget );
								//
							}
						}
					}
					//
				}
			}
		}
	}*/
}