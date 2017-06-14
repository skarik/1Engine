#include "CVoxelTerrain.h"

using std::cout;
using std::endl;
using std::min;

// Gives ownership of the returned array away. Remember to delete it.
Vector3d* pointsOnSphere ( int N )
{
	Vector3d* pts = new Vector3d [N];

	/*const float inc = float(PI) * ( 3 - sqrt( 5.0f ) );
	const float off = 2 / float( N );*/

	/*float y, r, phi;
	for ( int k = 0; k < N; k += 1 )
	{
		y = k*off - 1.0f + ( off/2.0f );
		r = sqrt( 1-y*y );
		phi = k * inc;
		pts[k] = Vector3d( cosf( phi )*r, y, sinf( phi ) * r ).normal();
	}*/
	float y, r, phi = 0;
	const float s = 3.6f / sqrt( float(N) );
	
	for ( int k = 1; k < N - 1; k += 1 )
	{
		y = -1.0f + ( 2.0f * k ) / ( N-1 );
		r = sqrt( 1-y*y );
		phi = phi + s / r;
		pts[k] = Vector3d( cosf(phi)*r, sinf(phi)*r, y );
	}

	pts[0] = Vector3d( 0,0,-1 );
	pts[N-1] = Vector3d (0,0,1);

	return pts;
}

void CVoxelTerrain::LightTree8 ( CBoob * pBoob, subblock8 * data, char const index16, char const index8 )
{
	static float acculm;
	static float block_z;
	static float scalar;
	/*const int samples = 12;
	Vector3d* pts = pointsOnSphere ( samples );

	for ( short i = 0; i < 512; i += 1 )
	{
		// Only calculate lighting only if the block is not solid and borders some other thing
		if (( data->data[i] == 0 )&&( BlockHasNeighbors( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data, i, 1 ) ))
		{
			// TODO: check for a colliding block
			acculm = 0.1f;
			
			for ( int n = 0; n < samples; n += 1 )
			{
				if ( !LightTrace( pBoob, data, index16, index8, i, pts[n] ) )
				{
					acculm += 2.0f/samples;
				}
			}

			data->light[i] = (unsigned char) ( min(acculm,1.0f)*255 );
		}
	}*/

	const int samples = 10;
	static Vector3d* pts = NULL;
	if ( pts == NULL )
	{
		pts = pointsOnSphere ( samples );
		// Make all samples point upwards
		for ( int n = 0; n < samples; n += 1 ) {
			pts[n].z = abs( pts[n].z );
		}
	}

	for ( short i = 0; i < 512; i += 1 )
	{
		if (( data->data[i].block == EB_NONE )&&( BlockHasNeighbors( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data, i, 1 ) ))
		{
			// Reset acculmulation
			acculm = 0.07f;

			// Get block Z
			block_z = (float)pBoob->li_position.z + (((index16/4) * 16) + ((index8/4) * 8)  + (i/64))/32.0f;

			// Calculate scalar
			scalar = 1 - std::min<float>( 1, std::max<float>( 0, (pBoob->elevation-block_z)*1.7f-0.07f ) );

			// Do basic radiosity
			for ( int n = 0; n < samples; n += 1 )
			{
				if ( !LightTrace( pBoob, data, index16, index8, i, pts[n] ) )
				{
					acculm += (1.34f/samples) * scalar;
				}
			}
			
			// Darken if a block above
			if ( acculm > 0.89f )
			{
				acculm = min<float>( acculm, 1.0f );
				if ( LightTraceUpLong( pBoob, data, index16, index8, i ) )
				{
					acculm -= 1.8f/8.1f; // Increase this factor as elevation drops
				}
			}
			else
			{
				if ( LightTraceUpLong( pBoob, data, index16, index8, i ) )
				{
					acculm *= 7.0f/8.0f;
				}
				acculm = min<float>( acculm, 1.0f );
			}

			// Set light data
			data->data[i].light = (unsigned char)( acculm * 7.9f );

			// Set light of block below
			if ( InSideVisible( pBoob, index16, index8, (char*)pBoob->data[index16].data[index8].data, i, 1, BOTTOM, 1 ) ) {
				CBoob* pCurrentBoob = pBoob;
				char cCIndex16 = index16, cCIndex8 = index8;
				short iCIndexD = i;
				if ( TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, BOTTOM ) ) {
					pCurrentBoob->data[cCIndex16].data[cCIndex8].data[iCIndexD].light = data->data[i].light;
				}
			}
		}
	}

	//delete [] pts;
}

inline bool CVoxelTerrain::BlockHasNeighbors ( CBoob* pBoob, char const index16, char const index8, char * data, int const i, int const width )
{
	char res = 1;
	if ( InSideVisible( pBoob, index16, index8, data, i, width, FRONT, res ) )
		return true;
	if ( InSideVisible( pBoob, index16, index8, data, i, width, BACK, res ) )
		return true;
	if ( InSideVisible( pBoob, index16, index8, data, i, width, LEFT, res ) )
		return true;
	if ( InSideVisible( pBoob, index16, index8, data, i, width, RIGHT, res ) )
		return true;
	if ( InSideVisible( pBoob, index16, index8, data, i, width, BOTTOM, res ) )
		return true;
	if ( InSideVisible( pBoob, index16, index8, data, i, width, TOP, res ) )
		return true;
	return false;
}

inline bool CVoxelTerrain::LightTrace ( CBoob * pBoob, subblock8 * data, char const index16, char const index8, short const indexD, Vector3d dir )
{
	Vector3d targetDelta = dir*1.4f;
	Vector3d currentDelta = Vector3d( 0,0,0 );

	CBoob* pCurrentBoob = pBoob;
	char cCIndex16 = index16, cCIndex8 = index8;
	short iCIndexD = indexD;
	while ( targetDelta.sqrMagnitude() < sqr(6) )
	{
		//
		while ( currentDelta.x < targetDelta.x )
			if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, FRONT ) )
				return false;
			else
				currentDelta.x += 1;
		//
		while ( currentDelta.x > targetDelta.x )
			if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, BACK ) )
				return false;
			else
				currentDelta.x -= 1;
		//
		while ( currentDelta.y < targetDelta.y )
			if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, LEFT ) )
				return false;
			else
				currentDelta.y += 1;
		//
		while ( currentDelta.y > targetDelta.y )
			if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, RIGHT ) )
				return false;
			else
				currentDelta.y -= 1;
		//
		while ( currentDelta.z < targetDelta.z )
			if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, TOP ) )
				return false;
			else
				currentDelta.z += 1;
		//
		while ( currentDelta.z > targetDelta.z )
			if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, BOTTOM ) )
				return false;
			else
				currentDelta.z -= 1;
		//

		if ( pCurrentBoob->data[cCIndex16].data[cCIndex8].data[iCIndexD].block != EB_NONE )
			return true;

		//if ( currentDelta.sq )
		targetDelta += dir;
	}

	return false;
}

inline bool CVoxelTerrain::LightTraceUpLong ( CBoob * pBoob, subblock8 * data, char const index16, char const index8, short const indexD )
{
	static int i;

	CBoob* pCurrentBoob = pBoob;
	char cCIndex16 = index16, cCIndex8 = index8;
	short iCIndexD = indexD;

	for ( i = 0; i < 29; ++i )
	{
		if ( !TraverseTree( &pCurrentBoob, cCIndex16, cCIndex8, iCIndexD, TOP ) )
			return false;

		if ( pCurrentBoob->data[cCIndex16].data[cCIndex8].data[iCIndexD].block != EB_NONE )
			return true;
	}

	return false;
}

bool CVoxelTerrain::TraverseTree ( CBoob ** pBoob, char& index16, char& index8, short& i, EFaceDir direction )
{
	const char width = 8;
	const char ofs = 1;

	if ( (*pBoob) == NULL)
		cout << "pBoob is NULL for some reason..." << endl;

	switch ( direction )
	{
	case FRONT:
		{
			if ( i%width < width-ofs )
			{
				//checkdata = data[i+ofs];
				i = i+ofs;
			}
			else
			{
				if ( index8%2 != 1 )
				{
					//checkdata = pBoob->data[index16].data[index8+1].data[i-(width-ofs)];
					i = i-(width-ofs);
					index8 = index8+1;
				}
				else if ( index16%2 != 1 )
				{
					//checkdata = pBoob->data[index16+1].data[index8-1].data[i-(width-ofs)];
					i = i-(width-ofs);
					index8 = index8-1;
					index16 = index16+1;
				}
				else if ( (*pBoob)->front != NULL )
				{
					//checkdata = pBoob->front->data[index16-1].data[index8-1].data[i-(width-ofs)];
					i = i-(width-ofs);
					index8 = index8-1;
					index16 = index16-1;
					*pBoob = (*pBoob)->front;
				}
				else
				{
					return false;
				}
			}
		}
		break;
	case BACK:
		{
			if ( i%width >= ofs )
			{
				//checkdata = data[i-ofs];
				i = i-ofs;
			}
			else
			{
				if ( index8%2 != 0 )
				{
					//checkdata = pBoob->data[index16].data[index8-1].data[i+(width-ofs)];
					i = i+(width-ofs);
					index8 = index8-1;
				}
				else if ( index16%2 != 0 )
				{
					//checkdata = pBoob->data[index16-1].data[index8+1].data[i+(width-ofs)];
					i = i+(width-ofs);
					index8 = index8+1;
					index16 = index16-1;
				}
				else if ( (*pBoob)->back != NULL )
				{
					//checkdata = pBoob->back->data[index16+1].data[index8+1].data[i+(width-ofs)];
					i = i+(width-ofs);
					index8 = index8+1;
					index16 = index16+1;
					*pBoob = (*pBoob)->back;
				}
				else
				{
					return false;
				}
			}
		}
		break;
	case LEFT:
		{
			if ( ((i/width)%width) < width-ofs )
			{
				//checkdata = data[i+(width*ofs)];
				i = i+(width*ofs);
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					//checkdata = pBoob->data[index16].data[index8+2].data[i-(width*(width-ofs))];
					i = i-(width*(width-ofs));
					index8 = index8+2;
				}
				else if ( ((index16/2)%2) != 1 )
				{
					//checkdata = pBoob->data[index16+2].data[index8-2].data[i-(width*(width-ofs))];
					i = i-(width*(width-ofs));
					index8 = index8-2;
					index16 = index16+2;
				}
				else if ( (*pBoob)->left != NULL )
				{
					//checkdata = pBoob->left->data[index16-2].data[index8-2].data[i-(width*(width-ofs))];
					i = i-(width*(width-ofs));
					index8 = index8-2;
					index16 = index16-2;
					*pBoob = (*pBoob)->left;
				}
				else
				{
					return false;
				}
			}
		}
		break;
	case RIGHT:
		{
			if ( ((i/width)%width) >= ofs )
			{
				//checkdata = data[i-(width*ofs)];
				i = i-(width*ofs);
			}
			else
			{
				if ( ((index8/2)%2) != 0 )
				{
					//checkdata = pBoob->data[index16].data[index8-2].data[i+(width*(width-ofs))];
					i = i+(width*(width-ofs));
					index8 = index8-2;
				}
				else if ( ((index16/2)%2) != 0 )
				{
					//checkdata = pBoob->data[index16-2].data[index8+2].data[i+(width*(width-ofs))];
					i = i+(width*(width-ofs));
					index8 = index8+2;
					index16 = index16-2;
				}
				else if ( (*pBoob)->right != NULL )
				{
					//checkdata = pBoob->right->data[index16+2].data[index8+2].data[i+(width*(width-ofs))];
					i = i+(width*(width-ofs));
					index8 = index8+2;
					index16 = index16+2;
					*pBoob = (*pBoob)->right;
				}
				else
				{
					return false;
				}
			}
		}
		break;
	case TOP:
		{
			if ( ((i/width/width)%width) != width-ofs )
			{
				//checkdata = data[i+(width*width*ofs)];
				i = i+(width*width*ofs);
			}
			else
			{
				if ( ((index8/4)%2) != 1 )
				{
					//checkdata = pBoob->data[index16].data[index8+4].data[i-(width*width*(width-ofs))];
					i = i-(width*width*(width-ofs));
					index8 = index8+4;
				}
				else if ( ((index16/4)%2) != 1 )
				{
					//checkdata = pBoob->data[index16+4].data[index8-4].data[i-(width*width*(width-ofs))];
					i = i-(width*width*(width-ofs));
					index8 = index8-4;
					index16 = index16+4;
				}
				else if (( (*pBoob)->top != NULL )&&( (*pBoob)->top->bitinfo[3] ))
				{
					//checkdata = pBoob->top->data[index16-4].data[index8-4].data[i-(width*width*(width-ofs))];
					i = i-(width*width*(width-ofs));
					index8 = index8-4;
					index16 = index16-4;
					*pBoob = (*pBoob)->top;
				}
				else
				{
					return false;
				}
			}
		}
		break;
	case BOTTOM:
		{
			if ( ((i/width/width)%width) >= ofs )
			{
				//checkdata = data[i-(width*width*ofs)];
				i = i-(width*width*ofs);
			}
			else
			{
				if ( ((index8/4)%2) != 0 )
				{
					//checkdata = pBoob->data[index16].data[index8-4].data[i+(width*width*(width-ofs))];
					i = i+(width*width*(width-ofs));
					index8 = index8-4;
				}
				else if ( ((index16/4)%2) != 0 )
				{
					//checkdata = pBoob->data[index16-4].data[index8+4].data[i+(width*width*(width-ofs))];
					i = i+(width*width*(width-ofs));
					index8 = index8+4;
					index16 = index16-4;
				}
				else if (( (*pBoob)->bottom != NULL )&&( (*pBoob)->bottom->bitinfo[3] ))
				{
					//checkdata = pBoob->bottom->data[index16+4].data[index8+4].data[i+(width*width*(width-ofs))];
					i = i+(width*width*(width-ofs));
					index8 = index8+4;
					index16 = index16+4;
					*pBoob = (*pBoob)->bottom;
				}
				else
				{
					return false;
				}
			}
		}
		break;
	}

	if ( i < 0 )
		cout << "WTF INDEX ";
	if ( index8 < 0 )
		cout << "WTF INDEX8 ";
	if ( index16 < 0 )
		cout << "WTF INDEX16 " << (int)index16 << " ";
	if (( i < 0 )||( index8 < 0 )||( index16 < 0 ))
		cout << endl;
	if ( (*pBoob) == NULL)
		cout << "Should never get here" << endl;
	return true;
}