
// Includes
#include "COctreeTerrain.h"
#include "COctreeRenderer.h"

#include "Math.h"
#include "CDebugDrawer.h"

#include "Zones.h"

void COctreeTerrain::DebugDrawOctree ( void )
{
	for ( int i = 0; i < 4; ++i ) {
		TrackInfo trackInfo;
		trackInfo.sector = m_root->leafs[i];
		trackInfo.subdivision = m_treeStartResolution/2;
		trackInfo.center = Vector3d( ((i%2)-0.5f)*m_treeStartResolution*blockSize*0.5f, ((i/2)-0.5f)*m_treeStartResolution*blockSize*0.5f, 0 );
		trackInfo.bindex = i;
		if ( trackInfo.sector ) {
			DebugDrawData( trackInfo );
		}
		Debug::Drawer->DrawLine( trackInfo.center+Vector3d(0,0,-1), trackInfo.center+Vector3d(0,0,1), Color(1,0,0,1) );
	}
}
void COctreeTerrain::DebugDrawData ( const TrackInfo& n_currentArea )
{
	if ( n_currentArea.subdivision < 4 ) {
		return;
	}
	for ( int i = 0; i < 8; ++i )
	{
		if ( n_currentArea.sector->data[i] )
		{
			Terrain::terra_b block;
			block.block = n_currentArea.sector->info[i].block;
			if ( block.block == EB_NONE )
			{
				Vector3d offset = Vector3d(1,1,1) * blockSize * n_currentArea.subdivision/2 * 0.5f;
				Vector3d center = n_currentArea.center + Vector3d(
						((i%2)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f,
						(((i/2)%2)-0.5f)	*n_currentArea.subdivision*blockSize*0.5f,
						((i/4)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f );
				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, offset.y, -offset.z ),
					center + Vector3d( offset.x, offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, -offset.y, -offset.z ),
					center + Vector3d( offset.x, -offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( -offset.x, offset.y, -offset.z ),
					center + Vector3d( -offset.x, offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( -offset.x, -offset.y, -offset.z ),
					center + Vector3d( -offset.x, -offset.y, offset.z ) );

				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, offset.y, offset.z ),
					center + Vector3d( -offset.x, offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, offset.y, -offset.z ),
					center + Vector3d( -offset.x, offset.y, -offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, -offset.y, offset.z ),
					center + Vector3d( -offset.x, -offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, -offset.y, -offset.z ),
					center + Vector3d( -offset.x, -offset.y, -offset.z ) );

				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, offset.y, offset.z ),
					center + Vector3d( offset.x, -offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( offset.x, offset.y, -offset.z ),
					center + Vector3d( offset.x, -offset.y, -offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( -offset.x, offset.y, offset.z ),
					center + Vector3d( -offset.x, -offset.y, offset.z ) );
				Debug::Drawer->DrawLine(
					center + Vector3d( -offset.x, offset.y, -offset.z ),
					center + Vector3d( -offset.x, -offset.y, -offset.z ) );
			}
		}
		else
		{
			TrackInfo trackInfo;
			trackInfo.sector = n_currentArea.sector->leafs[i];
			if ( trackInfo.sector )
			{
				trackInfo.subdivision = n_currentArea.subdivision/2;
				trackInfo.center = n_currentArea.center + Vector3d(
					((i%2)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f,
					(((i/2)%2)-0.5f)	*n_currentArea.subdivision*blockSize*0.5f,
					((i/4)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f
					);
				trackInfo.bindex = i;
				DebugDrawData( trackInfo );
			}
		}
	}
}

void COctreeTerrain::DebugCreateTestArea ( void )
{
	/*for ( int i = 0; i < 4; ++i ) 
	{
		m_root->data[i] = true;
		m_root->leafs[i] = 0;
		m_root->info[i].raw = 0;
	}*/
	/*
	// Loop through terrain and set data
	Real minX, maxX, minZ, maxZ;
	minX = -(m_treeStartResolution*blockSize*0.5) + blockSize*0.5f;
	maxX = +(m_treeStartResolution*blockSize*0.5) - blockSize*0.5f;
	minZ = -(m_treeStartResolution*blockSize*0.5*0.5) + blockSize*0.5f;
	maxZ = +(m_treeStartResolution*blockSize*0.5*0.5) - blockSize*0.5f;

	for ( Real x = minX; x <= maxX; x += blockSize )
	{
		for ( Real y = minX; y <= maxX; y += blockSize )
		{
			for ( Real z = minZ; z <= maxZ; z += blockSize )
			{
				Terrain::terra_b block;
				block.raw = 0;
				Real elevation = sin(sqrt(sqr(x*0.28)+sqr(y*0.28)))*7 - 5;

				if ( z < elevation ) {
					block.block = EB_STONE;
					block.normal_w = Math.Clamp( (elevation-z)/blockSize, 0, 1 ) * 15;
					//block.normal_w = 12;
				}

				SetDataAtPositionMinimal( block.raw, Vector3d(x,y,z) );
			}
		}
	}
	*/
	//Terrain::terra_b block;
	//block.block = EB_STONE;
	//SetDataAtPositionMinimal( block.raw, Vector3d(2000,0,0) );

	//DebugGenerateNormals();

	// Add some jobs
	Real minX, maxX, minZ, maxZ;
	minX = -(m_treeStartResolution*blockSize*0.5);
	maxX = +(m_treeStartResolution*blockSize*0.5);
	minZ = -(m_treeStartResolution*blockSize*0.5*0.5) + blockSize*0.5f;
	maxZ = +(m_treeStartResolution*blockSize*0.5*0.5) - blockSize*0.5f;

	/*minX *= 0.1;
	maxX *= 0.1;
	minZ *= 0.1;
	maxZ *= 0.1;*/
	for ( Real x = minX; x < maxX; x += blockSize*32 )
	{
		for ( Real y = minX; y < maxX; y += blockSize*32 )
		{
			for ( Real z = minZ; z < maxZ; z += blockSize*32 )
			{
				TerrainJob newJob ( Zones.PositionToRV( Vector3d(x+32,y+32,z+32) ) );
				newJob.area_min = Vector3d(x,y,z);
				newJob.area_max = newJob.area_min + Vector3d(32,32,32)*blockSize;

				newJob.type = TERRAJOB_GENERATE;
				Job_Enqueue( newJob );
			}
		}
	}
	for ( Real x = minX; x < maxX; x += blockSize*32 )
	{
		for ( Real y = minX; y < maxX; y += blockSize*32 )
		{
			for ( Real z = minZ; z < maxZ; z += blockSize*32 )
			{
				TerrainJob newJob ( Zones.PositionToRV( Vector3d(x+32,y+32,z+32) ) );
				newJob.area_min = Vector3d(x,y,z);
				newJob.area_max = newJob.area_min + Vector3d(32,32,32)*blockSize;

				newJob.type = TERRAJOB_RECALCULATE_NORMALS;
				Job_Enqueue( newJob );
			}
		}
	}
	for ( Real x = minX; x < maxX; x += blockSize*32 )
	{
		for ( Real y = minX; y < maxX; y += blockSize*32 )
		{
			for ( Real z = minZ; z < maxZ; z += blockSize*32 )
			{
				TerrainJob newJob ( Zones.PositionToRV( Vector3d(x+32,y+32,z+32) ) );
				newJob.area_min = Vector3d(x,y,z);
				newJob.area_max = newJob.area_min + Vector3d(32,32,32)*blockSize;

				newJob.type = TERRAJOB_REMESH_AREA;
				Job_Enqueue( newJob );
			}
		}
	}
}

void COctreeTerrain::DebugGenerateTestArea ( const Vector3d& nMin, const Vector3d& nMax, const Real& nStep )
{
	// Loop through terrain and set data
	for ( Real x = nMin.x+nStep*0.5f; x <= nMax.x-nStep*0.5; x += blockSize )
	{
		for ( Real y = nMin.y+nStep*0.5f; y <= nMax.y-nStep*0.5; y += blockSize )
		{
			for ( Real z = nMin.z+nStep*0.5f; z <= nMax.z-nStep*0.5; z += blockSize )
			{
				Terrain::terra_b block;
				block.raw = 0;
				Real distance = sqrt(sqr(x*0.24)+sqr(y*0.24));
				Real elevation = sin(distance)*7 - 5;

				elevation *= Math.Clamp( (96-distance)*0.1,0,1 );
				elevation += std::max<ftype>( distance-64, 0 ) * sin(x*0.03+y*0.01);

				if ( z < elevation ) {
					if ( sin(distance*0.5) > 0.0 ) {
						block.block = EB_DIRT;
					}
					else {
						block.block = EB_STONE;
					}
					block.normal_z_w = Math.Clamp( (elevation-z)/blockSize, 0, 1 ) * 15;
					//block.normal_w = 12;

					/*Vector3d normal;
					normal.x = sin(sqrt(sqr((x-blockSize*0.5)*0.28)+sqr(y*0.28))) - sin(sqrt(sqr((x+blockSize*0.5)*0.28)+sqr(y*0.28)));
					normal.y = sin(sqrt(sqr(x*0.28)+sqr((y-blockSize*0.5)*0.28))) - sin(sqrt(sqr(x*0.28)+sqr((y+blockSize*0.5)*0.28)));
					//normal.z = blockSize*2;
					//normal.z = (1-Vector2d(normal.x,normal.y).normal())*Vector2d(normal.x,normal.y).magnitude();
					normal.normalize();

					block.normal_x = (((normal.x+1)*6)+7);
					block.normal_y = (((normal.y+1)*6)+7);
					block.normal_z = (((normal.z+1)*6)+7);*/
				}

				SetDataAtPositionMinimal( block.raw, Vector3d(x,y,z) );
			}
		}
	}
}

void COctreeTerrain::DebugGenerateTestSubarea ( Terrain::Sector* n_sideBuffer, const Vector3d& nMin, const Vector3d& nMax )
{
	Real x,y,z;
	int32_t subX, subY, subZ;
	Real stepX, stepY, stepZ;
	stepX = (nMax.x - nMin.x)/32;
	stepY = (nMax.y - nMin.y)/32;
	stepZ = (nMax.z - nMin.z)/32;
	// Loop through sector buffer and set data
	for ( subX = 0; subX < 32; ++subX )
	{
		x = subX*stepX + nMin.x + blockSize*0.5f;
		for ( subY = 0; subY < 32; ++subY )
		{
			y = subY*stepY + nMin.y + blockSize*0.5f;
			for ( subZ = 0; subZ < 32; ++subZ )
			{
				z = subZ*stepZ + nMin.z + blockSize*0.5f;
				
				// x,y,z are now set to proper values

				// Generate test terrain
				Terrain::terra_b block;
				block.raw = 0;
				Real distance = sqrt(sqr(x*0.24)+sqr(y*0.24));
				Real elevation = sin(distance)*7 - 5;

				elevation *= Math.Clamp( (96-distance)*0.1,0,1 );
				elevation += std::max<ftype>( distance-64, 0 ) * sin(x*0.03+y*0.01);

				if ( z < elevation ) {
					if ( sin(distance*0.5) > 0.0 ) {
						block.block = EB_DIRT;
					}
					else {
						block.block = EB_STONE;
					}
					block.normal_z_w = Math.Clamp( (elevation-z)/blockSize, 0, 1 ) * 15;
				}

				// Generate sidebuffer information
				SetDataAtSidebufferPosition( n_sideBuffer, block.raw, RangeVector(subX,subY,subZ) );
			}
		}
	}

	DebugGenerateNormalsSubarea( n_sideBuffer );
}

void COctreeTerrain::DebugGenerateNormalsSubarea ( Terrain::Sector* n_sideBuffer )
{
	int32_t subX, subY, subZ;
	// Loop through sector buffer and set data
	for ( subX = 0; subX < 32; ++subX )
	{
		for ( subY = 0; subY < 32; ++subY )
		{
			for ( subZ = 0; subZ < 32; ++subZ )
			{

				// Initialize the Terrain tracker structures
				Terrain::terra_b t_blockSample;
				Terrain::terra_b t_normalBlockSample;

				GetDataAtSidebufferPosition( n_sideBuffer, t_blockSample.raw, RangeVector(subX,subY,subZ) );

				// Skip non needed calcs
				if ( Terrain::BlockHasValidNormals( t_blockSample ) ) {
					continue;
				}

				if ( !Terrain::Checker::BlocktypeOpaque(t_blockSample.block) ) {
					continue;
				}

				// Sample surrending area
				Vector3d currentNormal (0,0,0);
				const int rng = 1;
				for ( int ix = -rng; ix <= rng; ++ix )
				{
					for ( int iy = -rng; iy <= rng; ++iy )
					{
						for ( int iz = -rng; iz <= rng; ++iz )
						{
							if ( ix == 0 && iy == 0 && iz == 0 ) {
								continue;
							}
							if ( sqr(ix)+sqr(iy)+sqr(iz) > sqr(rng) ) {
								continue;
							}
							RangeVector sampleIndex (subX+ix,subY+iy,subZ+iz) ;
							if ( sampleIndex.x < 0 || sampleIndex.x >= 32 || sampleIndex.y < 0 || sampleIndex.y >= 32 || sampleIndex.z < 0 || sampleIndex.y >= 32 ) {
								continue;
							}
							GetDataAtSidebufferPosition( n_sideBuffer, t_normalBlockSample.raw, sampleIndex );
							if ( !Terrain::Checker::BlocktypeOpaque( t_normalBlockSample.block ) ) {
								currentNormal += Vector3d(ix,iy,iz);
							}
							//
						}
					}
				}
				// Only set normal if something changes.
				if ( currentNormal.sqrMagnitude() > FTYPE_PRECISION )
				{
					//cout << "NORMAL: " << currentNormal << endl;
					currentNormal.normalize();
					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( currentNormal.x );
					normal_y = Terrain::_normal_unbias( currentNormal.y );
					normal_z = Terrain::_normal_unbias( currentNormal.z );
					/*t_blockSample.normal_x = ((int)((currentNormal.x+1)*6))+1;
					t_blockSample.normal_y = ((int)((currentNormal.y+1)*6))+1;
					t_blockSample.normal_z = ((int)((currentNormal.z+1)*6))+1;*/
					t_blockSample.normal_y_x = normal_x;
					t_blockSample.normal_z_x = normal_x;
					t_blockSample.normal_x_y = normal_y;
					t_blockSample.normal_z_y = normal_y;
					t_blockSample.normal_x_z = normal_z;
					t_blockSample.normal_y_z = normal_z;
				
//					SetDataAtPositionMinimal( t_blockSample.raw, Vector3d(x,y,z) );
					SetDataAtSidebufferPosition( n_sideBuffer, t_blockSample.raw, RangeVector(subX,subY,subZ) );
				}
				//
			}
		}
	}
}


void COctreeTerrain::DebugGenerateNormals ( void )
{
	// Loop through terrain and set data
	/*Real minX, maxX, minZ, maxZ;
	minX = -(m_treeStartResolution*blockSize*0.5f) + blockSize*0.5f;
	maxX = +(m_treeStartResolution*blockSize*0.5f) - blockSize*0.5f;
	minZ = -(m_treeStartResolution*blockSize*0.25f) + blockSize*0.5f;
	maxZ = +(m_treeStartResolution*blockSize*0.25f) - blockSize*0.5f;

	minX *= 0.4f;
	maxX *= 0.4f;

	minZ *= 0.4f;
	maxZ *= 0.4f;

	minX = -26;
	maxX = 26;
	minZ = -26;
	maxZ = 26;

	for ( Real x = minX; x <= maxX; x += blockSize )
	{
		for ( Real y = minX; y <= maxX; y += blockSize )
		{
			for ( Real z = minZ; z <= maxZ; z += blockSize )
			{
				// Initialize the Terrain tracker structures
				Terrain::terra_b t_blockSample;
				Terrain::terra_b t_noramlBlockSample;
				
				TrackInfo tTrackerSource;
				tTrackerSource.center = Vector3d( 0,0,m_treeStartResolution*blockSize*0.5*0.5 );
				tTrackerSource.subdivision = m_treeStartResolution;
				tTrackerSource.sector = m_root;

				TrackInfo tTracker;

				// Get the zeroth position to see if should generate normals
				tTracker = tTrackerSource;
				while ( !SeekToPosition( tTracker, Vector3d(x,y,z), 2 ) ) {}
				if ( tTracker.sector == NULL ) {
					continue;
				}
				t_blockSample.raw = tTracker.sector->links[tTracker.bindex];

				// Skip non needed calcs
				if ( t_blockSample.normal_x != 0 ) {
					continue;
				}

				if ( !m_renderer->BlocktypeOpaque(t_blockSample.block) ) {
					//continue;
				}

				// Sample surrending area
				Vector3d currentNormal (0,0,0);
				const int rng = 1;
				for ( int ix = -rng; ix <= rng; ++ix )
				{
					for ( int iy = -rng; iy <= rng; ++iy )
					{
						for ( int iz = -rng; iz <= rng; ++iz )
						{
							if ( ix == 0 && iy == 0 && iz == 0 ) {
								continue;
							}
							if ( sqr(ix)+sqr(iy)+sqr(iz) > sqr(rng) ) {
								continue;
							}
							tTracker = tTrackerSource;
							while ( !SeekToPosition( tTracker, Vector3d(x+ix*blockSize,y+iy*blockSize,z+iz*blockSize) ) ) {}
							if ( tTracker.sector == NULL ) {
								continue;
							}
							t_noramlBlockSample.raw = tTracker.sector->links[tTracker.bindex];
							if ( !m_renderer->BlocktypeOpaque(t_noramlBlockSample.block) ) {
								currentNormal += Vector3d(ix,iy,iz);// * (1.0f/( abs(ix)+abs(iy)+abs(iz) ));
							}
						}
					}
				}
				// Only set normal if something changes.
				if ( currentNormal.sqrMagnitude() > FTYPE_PRECISION )
				{
					//cout << "NORMAL: " << currentNormal << endl;
					currentNormal.normalize();
					t_blockSample.normal_x = ((int)((currentNormal.x+1)*6))+1;
					t_blockSample.normal_y = ((int)((currentNormal.y+1)*6))+1;
					t_blockSample.normal_z = ((int)((currentNormal.z+1)*6))+1;
				
					//cout << "xyz" << t_blockSample.normal_x << " " << t_blockSample.normal_y << " " << t_blockSample.normal_z << endl;

					SetDataAtPositionMinimal( t_blockSample.raw, Vector3d(x,y,z) );
				}
				//
			}
		}
	}*/
}



