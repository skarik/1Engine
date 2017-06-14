
#include "CTerrainGenerator.h"


void	CTerrainGenerator::SB_Ray ( CBoob* pBoob, const RangeVector& position, const Ray& inRay, const terra_t block )
{
	ftype rayMag = inRay.dir.magnitude();
	// First do a sphere check
	if ( Vector3d(position.x-inRay.pos.x,position.y-inRay.pos.y,position.z-inRay.pos.z).sqrMagnitude() > sqr(rayMag+1.8f) ) {
		return;	// Exit if ray is nowhere near
	}

	ftype stepSize = (0.03125f*0.577f) / rayMag;
	Vector3d blockPos;
	RangeVector roundPos;
	int x,y,z;
	char i,j; short k;

	// Loop through each position of the ray
	for ( ftype step = 0; step < 1; step += stepSize )
	{
		// Get position
		blockPos = inRay.pos + inRay.dir*step;

		// Get major index from position
		roundPos.x = (rangeint)floor(blockPos.x+0.015625f);
		roundPos.y = (rangeint)floor(blockPos.y+0.015625f);
		roundPos.z = (rangeint)floor(blockPos.z+0.015625f);

		// Skip if out of range
		if ( roundPos != position ) {
			continue;
		}
	
		// Get array indices
		x = (int)((blockPos.x-roundPos.x)*32 + 0.5f);
		y = (int)((blockPos.y-roundPos.y)*32 + 0.5f);
		z = (int)((blockPos.z-roundPos.z)*32 + 0.5f);

		// Skip if out of range
		if ( x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32 ) {
			//cout << "Skipped because of bad range" << endl;
			continue;
		}

		// Get biased array index and set block
		i = ((x/16))+(2*(y/16))+(4*(z/16));
		j = (((x%16)/8))+(2*((y%16)/8))+(4*((z%16)/8));
		k = ((x%8))+(8*(y%8))+(64*(z%8));
		pBoob->data[i].data[j].data[k] = block;
	}
}
void	CTerrainGenerator::SB_Line ( CBoob* pBoob, const RangeVector& position, const Ray& inRay, const ftype inRadius, const terra_t block )
{
	// First do a sphere check
	if ( Vector3d(position.x-inRay.pos.x,position.y-inRay.pos.y,position.z-inRay.pos.z).sqrMagnitude() > sqr(inRay.dir.magnitude()+inRadius+1.8f) ) {
		return;	// Exit if ray is nowhere near
	}

	Vector3d baseVector ( 0,0,1 );
	Vector3d normalDir = inRay.dir.normal();
	if ( fabs( baseVector.dot(normalDir) ) > 0.98f ) {
		baseVector = Vector3d( 1,0,0 );
	}
	Vector3d vect_ortho = baseVector.cross( normalDir ).normal(); // Get the grid divs
	Vector3d vect_up	= vect_ortho.cross( normalDir ).normal();
	
	// Do center ray
	//SB_Ray( pBoob, position, Ray(inRay.pos-(normalDir*inRadius),inRay.dir+(normalDir*inRadius)), block );

	// Do SB_Ray in a grid radius around
	ftype step_dist;
	//for ( ftype step_x = 0.03125f*0.577f; step_x < inRadius; step_x += 0.03125f*0.577f )
	for ( ftype step_x = 0; step_x < inRadius; step_x += 0.03125f*0.577f )
	{
		//for ( ftype step_y = 0.03125f*0.577f; step_y < inRadius; step_y += 0.03125f*0.577f )
		for ( ftype step_y = 0; step_y < inRadius; step_y += 0.03125f*0.577f )
		{
			step_dist = sqr(step_x)+sqr(step_y);
			// Skip if out of range.
			if ( step_dist > sqr(inRadius) ) {
				continue;
			}
			step_dist = inRadius - sqrt(step_dist);
			SB_Ray( pBoob, position, Ray( inRay.pos+(vect_ortho*step_x)+(vect_up*step_y)-(normalDir*step_dist), inRay.dir+(normalDir*step_dist) ), block );
			SB_Ray( pBoob, position, Ray( inRay.pos-(vect_ortho*step_x)+(vect_up*step_y)-(normalDir*step_dist), inRay.dir+(normalDir*step_dist) ), block );
			SB_Ray( pBoob, position, Ray( inRay.pos+(vect_ortho*step_x)-(vect_up*step_y)-(normalDir*step_dist), inRay.dir+(normalDir*step_dist) ), block );
			SB_Ray( pBoob, position, Ray( inRay.pos-(vect_ortho*step_x)-(vect_up*step_y)-(normalDir*step_dist), inRay.dir+(normalDir*step_dist) ), block );
		}
	}
}

void	CTerrainGenerator::SB_RoadLine ( CBoob* pBoob, const RangeVector& position, const Ray& inRay, const ftype inWidth, const ftype inHeight, const terra_t block )
{
	// First do a sphere check
	if ( Vector3d(position.x-inRay.pos.x,position.y-inRay.pos.y,position.z-inRay.pos.z).sqrMagnitude() > sqr(inRay.dir.magnitude()+inWidth+1.8f) ) {
		return;	// Exit if ray is nowhere near
	}

	Vector3d baseVector ( 0,0,1 );
	Vector3d normalDir = inRay.dir.normal();
	/*if ( fabs( baseVector.dot(normalDir) ) > 0.98f ) {
		baseVector = Vector3d( 1,0,0 );
	}*/
	Vector3d vect_ortho = baseVector.cross( normalDir ).normal(); // Get the grid divs
	Vector3d vect_up	= baseVector;
	
	// Do center ray
	//SB_Ray( pBoob, position, Ray(inRay.pos,inRay.dir, block );

	// Do SB_Ray in a grid radius around
	//ftype step_dist;
	//for ( ftype step_x = 0.03125f*0.577f; step_x < inWidth; step_x += 0.03125f*0.577f )
	for ( ftype step_x = 0; step_x < inWidth; step_x += 0.03125f*0.577f )
	{
		//for ( ftype step_y = 0.03125f*0.577f; step_y < inHeight; step_y += 0.03125f*0.577f )
		for ( ftype step_y = 0; step_y < inHeight; step_y += 0.03125f*0.577f )
		{
			/*step_dist = sqr(step_x)+sqr(step_y);
			// Skip if out of range.
			if ( step_dist > sqr(inRadius) ) {
				continue;
			}*/
			//step_dist = inRadius - sqrt(step_dist);
			SB_Ray( pBoob, position, Ray( inRay.pos+(vect_ortho*step_x)-(vect_up*step_y), inRay.dir ), block );
			SB_Ray( pBoob, position, Ray( inRay.pos-(vect_ortho*step_x)-(vect_up*step_y), inRay.dir ), block );
			//SB_Ray( pBoob, position, Ray( inRay.pos+(vect_ortho*step_x)-(vect_up*step_y)-(normalDir*step_dist), inRay.dir+(normalDir*step_dist) ), block );
			//SB_Ray( pBoob, position, Ray( inRay.pos-(vect_ortho*step_x)-(vect_up*step_y)-(normalDir*step_dist), inRay.dir+(normalDir*step_dist) ), block );
		}
	}
}


void	CTerrainGenerator::SB_Cube ( CBoob* pBoob, const RangeVector& position, const Maths::Cubic& inCubic, const terra_t block )
{
	const ftype stepSize = 1.0f/32.0f;
	int x,y,z;
	char i,j; short k;
	Vector3d blockPos;
	RangeVector roundPos;
	Vector3d maxPos = inCubic.position + inCubic.size;

	// Quick 1 pass if statement to increase speed
	if (( position.x+1.0f < inCubic.position.x )||( position.y+1.0f < inCubic.position.y )||( position.z+1.0f < inCubic.position.z )) {
		return;
	}
	if (( position.x-1.0f > maxPos.x )||( position.y-1.0f > maxPos.y )||( position.z-1.0f > maxPos.z )) {
		return;
	}

	// Loop through each position of the block
	for ( blockPos.x = inCubic.position.x; blockPos.x < maxPos.x; blockPos.x += stepSize )
	{
		for ( blockPos.y = inCubic.position.y; blockPos.y < maxPos.y; blockPos.y += stepSize )
		{
			for ( blockPos.z = inCubic.position.z; blockPos.z < maxPos.z; blockPos.z += stepSize )
			{
				// Get major index from position
				roundPos.x = (rangeint)floor(blockPos.x + 0.015625f);
				roundPos.y = (rangeint)floor(blockPos.y + 0.015625f);
				roundPos.z = (rangeint)floor(blockPos.z + 0.015625f);

				// Skip if out of range
				if ( roundPos != position ) {
					continue;
				}
			
				// Get array indices
				x = (int)((blockPos.x-roundPos.x)*32 + 0.5f);
				y = (int)((blockPos.y-roundPos.y)*32 + 0.5f);
				z = (int)((blockPos.z-roundPos.z)*32 + 0.5f);

				// Skip if out of range
				if ( x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32 ) {
					//cout << "Skipped because of bad range" << endl;
					continue;
				}

				// Get biased array index and set block
				i = ((x/16))+(2*(y/16))+(4*(z/16));
				j = (((x%16)/8))+(2*((y%16)/8))+(4*((z%16)/8));
				k = ((x%8))+(8*(y%8))+(64*(z%8));
				pBoob->data[i].data[j].data[k] = block;
			}
		}
	}
}

void CTerrainGenerator::SB_PasteVXG ( CBoob* pBoob, const RangeVector& position, const Vector3d& terraposition, const RangeVector& size, const terra_t* rawvxg )
{
	Vector3d blockPos;
	RangeVector loopPos;
	RangeVector roundPos;
	int x,y,z;
	char i,j; short k;
	
	// Quick 1 pass if statement to increase speed
	if (( position.x+1.0f < terraposition.x )||( position.y+1.0f < terraposition.y )||( position.z+1.0f < terraposition.z )) {
		return;
	}
	if (( position.x-1.0f > terraposition.x+size.x*0.03125f )||( position.y-1.0f > terraposition.y+size.y*0.03125f )||( position.z-1.0f > terraposition.z+size.z*0.03125f )) {
		return;
	}

	// Loop through each index of the raw VXG
	for ( loopPos.x = 0; loopPos.x < size.x; ++loopPos.x )
	{
		blockPos.x = terraposition.x + loopPos.x*0.03125f;
		for ( loopPos.y = 0; loopPos.y < size.y; ++loopPos.y )
		{
			blockPos.y = terraposition.y + loopPos.y*0.03125f;
			for ( loopPos.z = 0; loopPos.z < size.z; ++loopPos.z )
			{
				blockPos.z = terraposition.z + loopPos.z*0.03125f;

				// Get major index from position
				roundPos.x = (rangeint)floor(blockPos.x + 0.015625f);
				roundPos.y = (rangeint)floor(blockPos.y + 0.015625f);
				roundPos.z = (rangeint)floor(blockPos.z + 0.015625f);

				// Skip if out of range
				if ( roundPos != position ) {
					continue;
				}

				// Get array indices
				x = (int)((blockPos.x-roundPos.x)*32 + 0.5f);
				y = (int)((blockPos.y-roundPos.y)*32 + 0.5f);
				z = (int)((blockPos.z-roundPos.z)*32 + 0.5f);

				// Skip if out of range
				if ( x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32 ) {
					//cout << "Skipped because of bad range" << endl;
					continue;
				}

				// Get biased array index and set block
				i = ((x/16))+(2*(y/16))+(4*(z/16));
				j = (((x%16)/8))+(2*((y%16)/8))+(4*((z%16)/8));
				k = ((x%8))+(8*(y%8))+(64*(z%8));
				if ( rawvxg[loopPos.x + loopPos.y*size.x + loopPos.z*size.x*size.y].block != EB_IGNORE ) {
					pBoob->data[i].data[j].data[k] = rawvxg[
						loopPos.x + 
						loopPos.y*size.x + 
						loopPos.z*size.x*size.y
					];
				}
			}
		}
	}
}

void CTerrainGenerator::SB_PasteVXGLoop ( CBoob* pBoob, const RangeVector& position, const Vector3d& terraposition, const RangeVector& size, const RangeVector& target, const terra_t* rawvxg )
{
	Vector3d blockPos;
	RangeVector loopPos;
	RangeVector roundPos;
	int x,y,z;
	char i,j; short k;
	
	// Quick 1 pass if statement to increase speed
	if (( position.x+1.0f < terraposition.x )||( position.y+1.0f < terraposition.y )||( position.z+1.0f < terraposition.z )) {
		return;
	}
	if (( position.x-1.0f > terraposition.x+target.x*0.03125f )||( position.y-1.0f > terraposition.y+target.y*0.03125f )||( position.z-1.0f > terraposition.z+target.z*0.03125f )) {
		return;
	}

	// Loop through each index of the raw VXG
	for ( loopPos.x = 0; loopPos.x < target.x; ++loopPos.x )
	{
		blockPos.x = terraposition.x + loopPos.x*0.03125f;
		for ( loopPos.y = 0; loopPos.y < target.y; ++loopPos.y )
		{
			blockPos.y = terraposition.y + loopPos.y*0.03125f;
			for ( loopPos.z = 0; loopPos.z < target.z; ++loopPos.z )
			{
				blockPos.z = terraposition.z + loopPos.z*0.03125f;

				// Get major index from position
				roundPos.x = (rangeint)floor(blockPos.x + 0.015625f);
				roundPos.y = (rangeint)floor(blockPos.y + 0.015625f);
				roundPos.z = (rangeint)floor(blockPos.z + 0.015625f);

				// Skip if out of range
				if ( roundPos != position ) {
					continue;
				}

				// Get array indices
				x = (int)((blockPos.x-roundPos.x)*32 + 0.5f);
				y = (int)((blockPos.y-roundPos.y)*32 + 0.5f);
				z = (int)((blockPos.z-roundPos.z)*32 + 0.5f);

				// Skip if out of range
				if ( x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32 ) {
					//cout << "Skipped because of bad range" << endl;
					continue;
				}

				// Get biased array index and set block
				i = ((x/16))+(2*(y/16))+(4*(z/16));
				j = (((x%16)/8))+(2*((y%16)/8))+(4*((z%16)/8));
				k = ((x%8))+(8*(y%8))+(64*(z%8));
				pBoob->data[i].data[j].data[k] = rawvxg[
					(abs(loopPos.x)%size.x) +
					(abs(loopPos.y)%size.y)*size.x +
					(abs(loopPos.z)%size.z)*size.x*size.y
				];
			}
		}
	}
}

void CTerrainGenerator::SB_PasteVXGOffset (
	CBoob* pBoob, const RangeVector& position, const Vector3d& terraposition,
	const RangeVector& size, const RangeVector& target, const RangeVector& baseoffset,
	const terra_t* rawvxg, bool overwriteEmpty )
{
	Vector3d blockPos;
	RangeVector loopPos;
	RangeVector roundPos;
	int x,y,z;
	char i,j; short k;
	
	// Quick 1 pass if statement to increase speed
	if (( position.x+1.0f < terraposition.x )||( position.y+1.0f < terraposition.y )||( position.z+1.0f < terraposition.z )) {
		return;
	}
	if (( position.x-1.0f > terraposition.x+target.x*0.03125f )||( position.y-1.0f > terraposition.y+target.y*0.03125f )||( position.z-1.0f > terraposition.z+target.z*0.03125f )) {
		return;
	}

	// Loop through each index of the raw VXG
	for ( loopPos.x = 0; loopPos.x < target.x; ++loopPos.x )
	{
		blockPos.x = terraposition.x + loopPos.x*0.03125f;
		for ( loopPos.y = 0; loopPos.y < target.y; ++loopPos.y )
		{
			blockPos.y = terraposition.y + loopPos.y*0.03125f;
			for ( loopPos.z = 0; loopPos.z < target.z; ++loopPos.z )
			{
				blockPos.z = terraposition.z + loopPos.z*0.03125f;

				// Get major index from position
				roundPos.x = (rangeint)floor(blockPos.x + 0.015625f);
				roundPos.y = (rangeint)floor(blockPos.y + 0.015625f);
				roundPos.z = (rangeint)floor(blockPos.z + 0.015625f);

				// Skip if out of range
				if ( roundPos != position ) {
					continue;
				}

				// Get array indices
				x = (int)((blockPos.x-roundPos.x)*32 + 0.5f);
				y = (int)((blockPos.y-roundPos.y)*32 + 0.5f);
				z = (int)((blockPos.z-roundPos.z)*32 + 0.5f);

				// Skip if out of range
				if ( x < 0 || x >= 32 || y < 0 || y >= 32 || z < 0 || z >= 32 ) {
					//cout << "Skipped because of bad range" << endl;
					continue;
				}

				// Get biased array index and set block
				i = ((x/16))+(2*(y/16))+(4*(z/16));
				j = (((x%16)/8))+(2*((y%16)/8))+(4*((z%16)/8));
				k = ((x%8))+(8*(y%8))+(64*(z%8));
				// Get target array index for offset
				x = ((loopPos.x+baseoffset.x)%size.x);
				if ( x < 0 ) x += size.x;
				y = ((loopPos.y+baseoffset.y)%size.y);
				if ( y < 0 ) y += size.y;
				z = ((loopPos.z+baseoffset.z)%size.z);
				if ( z < 0 ) z += size.z;
				// Set the new data
				if ( overwriteEmpty ) {
					if ( rawvxg[ x + y*size.x + z*size.x*size.y ].block != EB_IGNORE ) {
						pBoob->data[i].data[j].data[k] = rawvxg[ x + y*size.x + z*size.x*size.y ];
					}
				}
				else {
					if ( (z == size.z-1) || (rawvxg[ x + y*size.x + z*size.x*size.y ].block != EB_NONE) ) {
						if ( rawvxg[ x + y*size.x + z*size.x*size.y ].block != EB_IGNORE ) {
							pBoob->data[i].data[j].data[k] = rawvxg[ x + y*size.x + z*size.x*size.y ];
						}
					}
				}
			}
		}
	}
}
