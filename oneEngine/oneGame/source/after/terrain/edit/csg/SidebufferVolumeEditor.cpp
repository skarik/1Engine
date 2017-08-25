
#include "SidebufferVolumeEditor.h"
#include "core/math/Math.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/types/terrain/BlockType.h"
#include "after/terrain/edit/SidebufferAccessor.h"

using namespace Terrain;

SidebufferVolumeEditor::SidebufferVolumeEditor ( SidebufferAccessor* n_accessor, const Vector3d_d& n_zeropoint )
	: m_accessor( n_accessor ), m_zeropoint( n_zeropoint )
{

}

SidebufferVolumeEditor::~SidebufferVolumeEditor ( void )
{

}

inline bool outOfRange ( const int n )
{
	if ( n < 0 ) return true;
	if ( n >= 32 ) return true;
	return false;
}

void SidebufferVolumeEditor::Sub_Sphere( const Vector3d_d& n_centerPoint, const Real_d& n_radius )
{
	// Generate the center block to start on.
	const int radialOffset = n_radius/Terrain::BlockSize + 1;
	const int centerX	= (n_centerPoint.x-m_zeropoint.x)/Terrain::BlockSize;
	const int centerY	= (n_centerPoint.y-m_zeropoint.y)/Terrain::BlockSize;
	const int centerZ	= (n_centerPoint.z-m_zeropoint.z)/Terrain::BlockSize;

	const int minX = centerX-radialOffset;
	const int minY = centerY-radialOffset;
	const int minZ = centerZ-radialOffset;

	const int maxX = centerX+radialOffset;
	const int maxY = centerY+radialOffset;
	const int maxZ = centerZ+radialOffset;

	// Skip if out of range
	if ( outOfRange(minX) && outOfRange(centerX) && outOfRange(maxX) ) return;
	if ( outOfRange(minY) && outOfRange(centerY) && outOfRange(maxY) ) return;
	if ( outOfRange(minZ) && outOfRange(centerZ) && outOfRange(maxZ) ) return;

	// Loop through all indicies in range
	Vector3d_d circleDelta;
	Real_d circleDistance;
	int subX, subY, subZ;
	for ( subX = minX; subX <= maxX; ++subX )
	{
		if ( outOfRange(subX) ) continue;
		circleDelta.x = (subX*Terrain::BlockSize) - (n_centerPoint.x-m_zeropoint.x);
		for ( subY = minY; subY <= maxY; ++subY )
		{
			if ( outOfRange(subY) ) continue;
			circleDelta.y = (subY*Terrain::BlockSize) - (n_centerPoint.y-m_zeropoint.y);
			for ( subZ = minZ; subZ <= maxZ; ++subZ )
			{
				if ( outOfRange(subZ) ) continue;
				circleDelta.z = (subZ*Terrain::BlockSize) - (n_centerPoint.z-m_zeropoint.z);
				circleDistance = circleDelta.magnitude();

				// Check if in block editing range
				if ( circleDistance < n_radius+Terrain::BlockSize*1.74 )
				{
					// Grab the current block
					terra_b block;
					m_accessor->GetData( block.raw, RangeVector(subX,subY,subZ) );

					// Generate the normal
					Vector3d blockNormal ( -circleDelta.x,-circleDelta.y,-circleDelta.z );
					blockNormal /= circleDistance;

					// Generate the extrusion
					Vector3d targetDelta ( circleDelta.x,circleDelta.y,circleDelta.z );
					targetDelta /= circleDistance;
					targetDelta *= n_radius;
					Vector3d blockNormalW ( 0,0,0 );
					blockNormalW.x = std::max<Real>( 0, fabs(circleDelta.x) - fabs(targetDelta.x) );
					blockNormalW.y = std::max<Real>( 0, fabs(circleDelta.y) - fabs(targetDelta.y) );
					blockNormalW.z = std::max<Real>( 0, fabs(circleDelta.z) - fabs(targetDelta.z) );

					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( blockNormal.x );
					normal_y = Terrain::_normal_unbias( blockNormal.y );
					normal_z = Terrain::_normal_unbias( blockNormal.z );
					block.normal_y_x = normal_x;
					block.normal_z_x = normal_x;
					block.normal_x_y = normal_y;
					block.normal_z_y = normal_y;
					block.normal_x_z = normal_z;
					block.normal_y_z = normal_z;
					// Set block offset
					block.normal_x_w = Terrain::_depth_unbias( blockNormalW.x / Terrain::BlockSize );
					block.normal_y_w = Terrain::_depth_unbias( blockNormalW.y / Terrain::BlockSize );
					block.normal_z_w = Terrain::_depth_unbias( blockNormalW.z / Terrain::BlockSize );

					// If the block is in range, empty out the block type
					if ( circleDistance < n_radius )
					{
						block.block = EB_NONE;
					}
					
					// Set the data
					m_accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
				}
			}
		}
	}
}


void SidebufferVolumeEditor::Add_Sphere( const Vector3d_d& n_centerPoint, const Real_d& n_radius, const ushort n_block )
{
	// Generate the center block to start on.
	const int radialOffset = n_radius/Terrain::BlockSize + 1;
	const int centerX	= (n_centerPoint.x-m_zeropoint.x)/Terrain::BlockSize;
	const int centerY	= (n_centerPoint.y-m_zeropoint.y)/Terrain::BlockSize;
	const int centerZ	= (n_centerPoint.z-m_zeropoint.z)/Terrain::BlockSize;

	const int minX = centerX-radialOffset;
	const int minY = centerY-radialOffset;
	const int minZ = centerZ-radialOffset;

	const int maxX = centerX+radialOffset;
	const int maxY = centerY+radialOffset;
	const int maxZ = centerZ+radialOffset;

	// Skip if out of range
	if ( outOfRange(minX) && outOfRange(centerX) && outOfRange(maxX) ) return;
	if ( outOfRange(minY) && outOfRange(centerY) && outOfRange(maxY) ) return;
	if ( outOfRange(minZ) && outOfRange(centerZ) && outOfRange(maxZ) ) return;

	// Loop through all indicies in range
	Vector3d_d circleDelta;
	Real_d circleDistance;
	int subX, subY, subZ;
	for ( subX = minX; subX <= maxX; ++subX )
	{
		if ( outOfRange(subX) ) continue;
		circleDelta.x = (subX*Terrain::BlockSize) - (n_centerPoint.x-m_zeropoint.x);
		for ( subY = minY; subY <= maxY; ++subY )
		{
			if ( outOfRange(subY) ) continue;
			circleDelta.y = (subY*Terrain::BlockSize) - (n_centerPoint.y-m_zeropoint.y);
			for ( subZ = minZ; subZ <= maxZ; ++subZ )
			{
				if ( outOfRange(subZ) ) continue;
				circleDelta.z = (subZ*Terrain::BlockSize) - (n_centerPoint.z-m_zeropoint.z);
				circleDistance = circleDelta.magnitude();

				// Check if in block editing range
				if ( circleDistance < n_radius+Terrain::BlockSize*1.74 )
				{
					// Grab the current block
					terra_b block;
					m_accessor->GetData( block.raw, RangeVector(subX,subY,subZ) );

					// Generate the normal
					Vector3d blockNormal ( circleDelta.x,circleDelta.y,circleDelta.z );
					blockNormal /= circleDistance;

					// Generate the extrusion
					Vector3d targetDelta ( circleDelta.x,circleDelta.y,circleDelta.z );
					targetDelta /= circleDistance;
					targetDelta *= n_radius;
					Vector3d blockNormalW ( 0,0,0 );
					blockNormalW.x = std::max<Real>( 0, fabs(circleDelta.x) - fabs(targetDelta.x) );
					blockNormalW.y = std::max<Real>( 0, fabs(circleDelta.y) - fabs(targetDelta.y) );
					blockNormalW.z = std::max<Real>( 0, fabs(circleDelta.z) - fabs(targetDelta.z) );

					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( blockNormal.x );
					normal_y = Terrain::_normal_unbias( blockNormal.y );
					normal_z = Terrain::_normal_unbias( blockNormal.z );
					block.normal_y_x = normal_x;
					block.normal_z_x = normal_x;
					block.normal_x_y = normal_y;
					block.normal_z_y = normal_y;
					block.normal_x_z = normal_z;
					block.normal_y_z = normal_z;
					// Set block offset
					block.normal_x_w = Terrain::_depth_unbias( blockNormalW.x / Terrain::BlockSize );
					block.normal_y_w = Terrain::_depth_unbias( blockNormalW.y / Terrain::BlockSize );
					block.normal_z_w = Terrain::_depth_unbias( blockNormalW.z / Terrain::BlockSize );

					// If the block is in range, empty out the block type
					if ( circleDistance < n_radius )
					{
						block.block = n_block;
					}
					
					// Set the data
					m_accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
				}
			}
		}
	}
}


//	gen_editor->Mak_Line( hallway.ray.pos, hallway.ray.dir, hallway.height/32.0f, emptyBlock );
void SidebufferVolumeEditor::Mak_Line ( const Vector3d_d& n_sourcepoint, const Vector3d_d& n_raydir, const Real_d& n_radius, const ushort n_block )
{
	// First do a sphere check
	if ( Vector3d_d(m_zeropoint.x-n_sourcepoint.x,m_zeropoint.y-n_sourcepoint.y,m_zeropoint.z-n_sourcepoint.z).sqrMagnitude() > sqr(n_raydir.magnitude()+n_radius+(1.8f*64.0f)) ) {
		return;	// Exit if ray is nowhere near
	}

	// Generate the normals
	Vector3d_d baseVector ( 0,0,1 );
	Vector3d_d normalDir = n_raydir.normal();
	if ( fabs( baseVector.dot(normalDir) ) > 0.96f ) {
		baseVector = Vector3d_d( 1,0,0 );
	}
	Vector3d_d vect_ortho = baseVector.cross( normalDir ).normal(); // Get the aligned grid divs.
	Vector3d_d vect_up	= vect_ortho.cross( normalDir ).normal();
	
	// Loop from here to the end of the line.
	Real_d t_rayLength = n_raydir.magnitude() + n_radius;
	Real_d t_stepLength = 1.0f / t_rayLength;

	for ( Real_d i_step = 0; i_step < 1; i_step += t_stepLength )
	{
		Vector3d_d linePosition = (n_sourcepoint - m_zeropoint) + n_raydir * i_step;

		Sub_Sphere( linePosition+m_zeropoint, n_radius );

		continue;

		// Loop around the line position in a square.
		const Real_d i_sstep = 1.3f;
		int32_t t_stepLimit = floor(n_radius+2);
		for ( int32_t i_step_x = -t_stepLimit; i_step_x < t_stepLimit; i_step_x += 1 )
		{
			for ( int32_t i_step_y = -t_stepLimit; i_step_y < t_stepLimit; i_step_y += 1 )
			{
				// Turn the step into a position
				Vector3d_d circleOffset = vect_ortho*i_step_x + vect_up*i_step_y;
				Vector3d_d blockPosition = linePosition + circleOffset;

				// Turn the position into an index
				int32_t subX = blockPosition.x / Terrain::BlockSize;
				int32_t subY = blockPosition.y / Terrain::BlockSize;
				int32_t subZ = blockPosition.z / Terrain::BlockSize;

				// Skip if out of range
				if ( outOfRange(subX) ) continue;
				if ( outOfRange(subY) ) continue;
				if ( outOfRange(subZ) ) continue;

				// Check distance now
				Real_d circleDistance = circleOffset.magnitude();
				if ( circleDistance < n_radius+Terrain::BlockSize*1.74 )
				{
					// Grab the current block
					terra_b block;
					m_accessor->GetData( block.raw, RangeVector(subX,subY,subZ) );

					// Generate the normal
					Vector3d blockNormal ( circleOffset.x,circleOffset.y,circleOffset.z );
					blockNormal /= circleDistance;
					// Generate the extrusion
					/*Vector3d targetDelta ( circleDelta.x,circleDelta.y,circleDelta.z );
					targetDelta /= circleDistance;
					targetDelta *= n_radius;
					Vector3d blockNormalW ( 0,0,0 );
					blockNormalW.x = std::max<Real>( 0, fabs(circleDelta.x) - fabs(targetDelta.x) );
					blockNormalW.y = std::max<Real>( 0, fabs(circleDelta.y) - fabs(targetDelta.y) );
					blockNormalW.z = std::max<Real>( 0, fabs(circleDelta.z) - fabs(targetDelta.z) );*/
					// Set block normals
					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( blockNormal.x );
					normal_y = Terrain::_normal_unbias( blockNormal.y );
					normal_z = Terrain::_normal_unbias( blockNormal.z );
					block.normal_y_x = normal_x;
					block.normal_z_x = normal_x;
					block.normal_x_y = normal_y;
					block.normal_z_y = normal_y;
					block.normal_x_z = normal_z;
					block.normal_y_z = normal_z;
					// Set block offset
					/*block.normal_x_w = Terrain::_depth_unbias( blockNormalW.x / COctreeTerrain::blockSize );
					block.normal_y_w = Terrain::_depth_unbias( blockNormalW.y / COctreeTerrain::blockSize );
					block.normal_z_w = Terrain::_depth_unbias( blockNormalW.z / COctreeTerrain::blockSize );*/

					// Do command
					if ( circleDistance < n_radius ) {
						block.block = EB_NONE;
					}

					// Set new block
					m_accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
				}
			}
		}
	}

	// Do SB_Ray in a grid radius around
	/*ftype step_dist;
	//for ( ftype step_x = 0.03125f*0.577f; step_x < inRadius; step_x += 0.03125f*0.577f )
	for ( ftype step_x = -n_radius; step_x < n_radius; step_x += 0.577f )
	{
		//for ( ftype step_y = 0.03125f*0.577f; step_y < inRadius; step_y += 0.03125f*0.577f )
		for ( ftype step_y = -n_radius; step_y < n_radius; step_y += 0.577f )
		{
			step_dist = sqr(step_x)+sqr(step_y);
			// Skip if out of range.
			if ( step_dist > sqr(n_radius) ) {
				continue;
			}

			Vector3d_d t_sourcepoint = n_sourcepoint+(vect_ortho*step_x)+(vect_up*step_y)-(normalDir*step_dist);
			Vector3d_d t_endDir = n_raydir+(normalDir*step_dist);

			ftype t_raylength = t_endDir.magnitude();
			ftype stepSize = (0.577f) / t_raylength;
			Vector3d_d blockPos;
			int subX, subY, subZ;
			for ( ftype step = 0; step < 1; step += stepSize )
			{
				blockPos = t_sourcepoint + t_endDir*step;

				subX = (int)((blockPos.x - m_zeropoint.x) + 0.5f);
				subY = (int)((blockPos.y - m_zeropoint.y) + 0.5f);
				subZ = (int)((blockPos.z - m_zeropoint.z) + 0.5f);
			}
		}
	}*/
}

void SidebufferVolumeEditor::Sub_Line ( const Vector3d_d& n_sourcepoint, const Vector3d_d& n_raydir )
{
	ftype rayMag = n_raydir.magnitude();
	// First do a sphere check
	if ( Vector3d_d(m_zeropoint.x-n_sourcepoint.x,m_zeropoint.y-n_sourcepoint.y,m_zeropoint.z-n_sourcepoint.z).sqrMagnitude() > sqr(n_raydir.magnitude()+(1.8f*64.0f)) ) {
		return;	// Exit if ray is nowhere near
	}

	ftype stepSize = (0.577f) / rayMag;
	Vector3d_d blockPos;
	RangeVector roundPos;
	int x,y,z;
	char i,j; short k;

	// Loop through each position of the ray
	for ( ftype step = 0; step < 1; step += stepSize )
	{
		// Get position
		blockPos = n_sourcepoint + n_raydir*step;

		// Get major index from position
		/*roundPos.x = (rangeint)floor(blockPos.x / 64.0f);
		roundPos.y = (rangeint)floor(blockPos.y / 64.0f);
		roundPos.z = (rangeint)floor(blockPos.z / 64.0f);*/

		// Skip if out of range
		/*if ( roundPos != position ) {
			continue;
		}*/
	
		// Get array indices
		/*x = (int)((blockPos.x-roundPos.x)*32 + 0.5f);
		y = (int)((blockPos.y-roundPos.y)*32 + 0.5f);
		z = (int)((blockPos.z-roundPos.z)*32 + 0.5f);*/
		x = (int)((blockPos.x - m_zeropoint.x) / Terrain::BlockSize + 0.5f);
		y = (int)((blockPos.y - m_zeropoint.y) / Terrain::BlockSize + 0.5f);
		z = (int)((blockPos.z - m_zeropoint.z) / Terrain::BlockSize + 0.5f);

		// Skip if out of range
		if ( outOfRange(x) ) continue;
		if ( outOfRange(y) ) continue;
		if ( outOfRange(z) ) continue;

		// Get biased array index and set block
		/*i = ((x/16))+(2*(y/16))+(4*(z/16));
		j = (((x%16)/8))+(2*((y%16)/8))+(4*((z%16)/8));
		k = ((x%8))+(8*(y%8))+(64*(z%8));
		pBoob->data[i].data[j].data[k] = block;*/
		// Grab the current block
		terra_b block;
		m_accessor->GetData( block.raw, RangeVector(x,y,z) );

		block.block = EB_NONE;

		m_accessor->SetData( block.raw, RangeVector(x,y,z) );
	}
}


void SidebufferVolumeEditor::Add_Box ( const Vector3d_d& n_centerpoint, const Vector3d_d& n_halfextents, const ushort n_block )
{
	// Generate the center block to start on.
	const int halfxoffset = n_halfextents.x/Terrain::BlockSize + 1;
	const int halfyoffset = n_halfextents.y/Terrain::BlockSize + 1;
	const int halfzoffset = n_halfextents.z/Terrain::BlockSize + 1;
	const int centerX	= (n_centerpoint.x-m_zeropoint.x)/Terrain::BlockSize;
	const int centerY	= (n_centerpoint.y-m_zeropoint.y)/Terrain::BlockSize;
	const int centerZ	= (n_centerpoint.z-m_zeropoint.z)/Terrain::BlockSize;

	const int minX = centerX-halfxoffset;
	const int minY = centerY-halfyoffset;
	const int minZ = centerZ-halfzoffset;

	const int maxX = centerX+halfxoffset;
	const int maxY = centerY+halfyoffset;
	const int maxZ = centerZ+halfzoffset;

	// Skip if out of range
	if ( outOfRange(minX) && outOfRange(centerX) && outOfRange(maxX) ) return;
	if ( outOfRange(minY) && outOfRange(centerY) && outOfRange(maxY) ) return;
	if ( outOfRange(minZ) && outOfRange(centerZ) && outOfRange(maxZ) ) return;

	// Loop through the area now.
	Vector3d_d boxDelta;
	int subX, subY, subZ;
	for ( subX = minX; subX <= maxX; ++subX )
	{
		if ( outOfRange(subX) ) continue;
		boxDelta.x = (subX*Terrain::BlockSize) - (n_centerpoint.x-m_zeropoint.x);
		for ( subY = minY; subY <= maxY; ++subY )
		{
			if ( outOfRange(subY) ) continue;
			boxDelta.y = (subY*Terrain::BlockSize) - (n_centerpoint.y-m_zeropoint.y);
			for ( subZ = minZ; subZ <= maxZ; ++subZ )
			{
				if ( outOfRange(subZ) ) continue;
				boxDelta.z = (subZ*Terrain::BlockSize) - (n_centerpoint.z-m_zeropoint.z);

				// Check if in block editing range
				if (( abs(boxDelta.x) < n_halfextents.x+Terrain::BlockSize*1.74 )&&
					( abs(boxDelta.y) < n_halfextents.y+Terrain::BlockSize*1.74 )&&
					( abs(boxDelta.z) < n_halfextents.z+Terrain::BlockSize*1.74 ))
				{
					// Grab the current block
					terra_b block;
					m_accessor->GetData( block.raw, RangeVector(subX,subY,subZ) );

					// Generate the normal
					//Vector3d blockNormal ( boxDelta.x,boxDelta.y,boxDelta.z );
					//blockNormal /= circleDistance;
					//blockNormal.normal();

					// Generate the extrusion
					Vector3d targetDelta ( boxDelta.x,boxDelta.y,boxDelta.z );
					//targetDelta /= circleDistance;
					//targetDelta *= n_radius;
					Vector3d blockNormalW ( 0,0,0 );
					blockNormalW.x = 0.5f;//std::max<Real>( 0, fabs(circleDelta.x) - fabs(targetDelta.x) );
					blockNormalW.y = 0.5f;//std::max<Real>( 0, fabs(circleDelta.y) - fabs(targetDelta.y) );
					blockNormalW.z = 0.5f;//std::max<Real>( 0, fabs(circleDelta.z) - fabs(targetDelta.z) );
					// Set block offset
					block.normal_x_w = Terrain::_depth_unbias( blockNormalW.x / Terrain::BlockSize );
					block.normal_y_w = Terrain::_depth_unbias( blockNormalW.y / Terrain::BlockSize );
					block.normal_z_w = Terrain::_depth_unbias( blockNormalW.z / Terrain::BlockSize );

					// Calculate normals. How?
					// First move to cubic coordinates.
					Vector3d cubeCoordinates = Vector3d(
						boxDelta.x/n_halfextents.x,
						boxDelta.y/n_halfextents.y,
						boxDelta.z/n_halfextents.z );

					// Do X/Y/Z selection
					/*bool useX = false, useY = false, useZ = false;
					if ( fabs(cubeCoordinates.x)-fabs(cubeCoordinates.y) > COctreeTerrain::blockSize && fabs(cubeCoordinates.x)-fabs(cubeCoordinates.z) > COctreeTerrain::blockSize )
					{
						// use x
						useX = true;
					}
					else if ( fabs(cubeCoordinates.y)-fabs(cubeCoordinates.x) > COctreeTerrain::blockSize && fabs(cubeCoordinates.y)-fabs(cubeCoordinates.z) > COctreeTerrain::blockSize )
					{
						// use y
						useY = true;
					}
					else if ( fabs(cubeCoordinates.x)-fabs(cubeCoordinates.z) > COctreeTerrain::blockSize && fabs(cubeCoordinates.y)-fabs(cubeCoordinates.z) > COctreeTerrain::blockSize )
					{
						// use xy
						useX = true;
						useY = true;
					}
					else if ( fabs(cubeCoordinates.z)-fabs(cubeCoordinates.x) > COctreeTerrain::blockSize && fabs(cubeCoordinates.z)-fabs(cubeCoordinates.y) > COctreeTerrain::blockSize )
					{
						// use z
						useZ = true;
					}
					else if ( fabs(cubeCoordinates.x)-fabs(cubeCoordinates.y) > COctreeTerrain::blockSize )
					{
						// use xz
						useX = true;
						useZ = true;
					}
					else if ( fabs(cubeCoordinates.y)-fabs(cubeCoordinates.x) > COctreeTerrain::blockSize )
					{
						// use yz
						useY = true;
						useZ = true;
					}
					else
					{
						// use xyz
						useX = true;
						useY = true;
						useZ = true;
					}

					Vector3d blockNormal = Vector3d( Math.sgn<Real>(cubeCoordinates.x), Math.sgn<Real>(cubeCoordinates.y), Math.sgn<Real>(cubeCoordinates.z) );
					int normal_x, normal_y, normal_z;
					if ( useX ) {
						normal_x = Terrain::_normal_unbias( blockNormal.x );
						normal_y = Terrain::_normal_unbias( 0 );
						normal_z = Terrain::_normal_unbias( 0 );
						block.normal_x_y = normal_y;
						block.normal_x_z = normal_z;
					}
					if ( useY ) {
						normal_x = Terrain::_normal_unbias( 0 );
						normal_y = Terrain::_normal_unbias( blockNormal.y );
						normal_z = Terrain::_normal_unbias( 0 );
						block.normal_y_x = normal_x;
						block.normal_y_z = normal_z;
					}
					if ( useZ ) {
						normal_x = Terrain::_normal_unbias( 0 );
						normal_y = Terrain::_normal_unbias( blockNormal.y );
						normal_z = Terrain::_normal_unbias( 0 );
						block.normal_y_x = normal_x;
						block.normal_y_z = normal_z;
					}*/

					Vector3d blockNormal( 0,0,0 );
					int normal_x, normal_y, normal_z;
					normal_x = Terrain::_normal_unbias( blockNormal.x );
					normal_y = Terrain::_normal_unbias( blockNormal.y );
					normal_z = Terrain::_normal_unbias( blockNormal.z );
					block.normal_y_x = normal_x;
					block.normal_z_x = normal_x;
					block.normal_x_y = normal_y;
					block.normal_z_y = normal_y;
					block.normal_x_z = normal_z;
					block.normal_y_z = normal_z;

					// If the block is in range, empty out the block type
					if (( abs(boxDelta.x) < n_halfextents.x )&&
						( abs(boxDelta.y) < n_halfextents.y )&&
						( abs(boxDelta.z) < n_halfextents.z ))
					{
						block.block = n_block;
						block.smooth_normal = 0;
						// Set the data
						m_accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
					}
					else if ( block.block == EB_NONE )
					{
						// Set the data
						block.smooth_normal = 0;
						m_accessor->SetData( block.raw, RangeVector(subX,subY,subZ) );
					}
				}
			}
		}
	}
}