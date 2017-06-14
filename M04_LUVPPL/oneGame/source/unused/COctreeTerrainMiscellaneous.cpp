
#include "COctreeTerrain.h"
#include "COctreeRenderer.h"
#include "Math.h"

#include "after/world/generation/CWorldGenerator.h"

void COctreeTerrain::Edit_EstimateNormals ( const Vector3d& nMin, const Vector3d& nMax, const Real& nStep )
{
	// Loop through terrain and set data
	for ( Real x = nMin.x-nStep*0.5; x <= nMax.x+nStep*0.5; x += nStep )
	{
		for ( Real y = nMin.y-nStep*0.5; y <= nMax.y+nStep*0.5; y += nStep )
		{
			for ( Real z = nMin.z-nStep*0.5; z <= nMax.z+nStep*0.5; z += nStep )
			{
				if ( !PositionInOctree( Vector3d(x,y,z) ) ) {
					continue;
				}

				// Initialize the Terrain tracker structures
				Terrain::terra_b t_blockSample;
				Terrain::terra_b t_normalBlockSample;
				
				TrackInfo tTrackerSource;
				tTrackerSource.center = Vector3d( 0,0,m_treeStartResolution*blockSize*0.5*0.5 );
				tTrackerSource.subdivision = m_treeStartResolution;
				tTrackerSource.sector = m_root;

				TrackInfo tTracker;
				
				// Get the zeroth position to see if should generate normals
				SampleBlock( Vector3d(x,y,z), t_blockSample.raw );

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
							//if ( !PositionInOctree( Vector3d(x+ix*nStep,y+iy*nStep,z+iz*nStep) ) ) {
								//continue;
							//}
							SampleBlock( Vector3d(x+ix*nStep,y+iy*nStep,z+iz*nStep), t_normalBlockSample.raw );
							if ( !Terrain::Checker::BlocktypeOpaque( t_normalBlockSample.block ) ) {
								currentNormal += Vector3d(ix,iy,iz);
							}
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
				
					SetDataAtPositionMinimal( t_blockSample.raw, Vector3d(x,y,z) );
				}
				//
				
			}
		}
	}
}


void COctreeTerrain::SetGenerator ( Terrain::CWorldGenerator* n_generator )
{
	mGenerator = n_generator;
}

void COctreeTerrain::Generation_GenerateSubarea ( Terrain::Sector* n_sideBuffer, Terrain::SectorGameData* n_sideBufferGameData, const RangeVector& n_rangeIndex/*, const Vector3d& nMin, const Vector3d& nMax*/ )
{
	/*Terrain::terra_b block;
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
				// Generate terrain
				mGenerator->GenerateBlockAt( this, n_sideBuffer, Vector3d(x,y,z), block );
				// Generate sidebuffer information
				SetDataAtSidebufferPosition( n_sideBuffer, block.raw, RangeVector(subX,subY,subZ) );
			}
		}
	}*/
	Vector3d_d nMin, nMax;
	nMin.x = n_rangeIndex.x * 64;
	nMin.y = n_rangeIndex.y * 64;
	nMin.z = n_rangeIndex.z * 64;
	nMax = nMin + Vector3d_d(64,64,64);

	Terrain::inputTerrain_t input;
	input.terrain = this;
	input.regions = m_regions;
	input.patterns= m_patterns;
	mGenerator->GenerateSector( input, n_sideBuffer, n_sideBufferGameData, nMin, nMax, n_rangeIndex );

	DebugGenerateNormalsSubarea( n_sideBuffer );
}