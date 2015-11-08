#include "CVoxelTerrain.h"

// Tree visibilty culling, based on frustum culling.
void CVoxelTerrain::CullTree32 ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[2] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[2] = true;

			pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
			pBoob->li_position = position;
			char result;
			result = CCamera::activeCamera->SphereIsVisible( pBoob->position, 58 );
			// 32

			if ( bDrawDebugBoobs )
			{
				Maths::Cubic debugCube ( Vector3d(ftype(position.x*64),ftype(position.y*64),ftype(position.z*64)), Vector3d( 64,64,64 ) );
				debugCube.DrawDebug();
			}

			// Check current boob
			if ( result == 2 )
			{
				//if ( !pBoob->bitinfo[0] )
				//	needUpdateOnVBO = true;
				pBoob->bitinfo[0] = true;
				for ( char i = 0; i < 8; i += 1 )
				{
					//pBoob->visible[i] = true;
					/*for ( char j = 0; j < 8; j += 1 )
					{
						pBoob->data[i].visible[j] = true;
					}*/
				}
			}
			else if ( result == 1 )
			{
				pBoob->bitinfo[0] = true;
				/*for ( char i = 0; i < 8; i += 1 )
				{
					if ( pBoob->visible[i] )
					{
						CullTree16( pBoob, &(pBoob->data[i]), i, position );
					}
				}*/
				for ( char i = 0; i < 8; i += 1 )
				{
					//pBoob->visible[i] = true;
					/*for ( char j = 0; j < 8; j += 1 )
					{
						pBoob->data[i].visible[j] = true;
					}*/
				}
			}
			else if ( result == 0 )
			{
				//if ( pBoob->bitinfo[0] )
				//	needUpdateOnVBO = true;
				// Boob completely invisible
				pBoob->bitinfo[0] = false;
			}

			// Cull other boobs
			CullTree32( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			CullTree32( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			CullTree32( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			CullTree32( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			CullTree32( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			CullTree32( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
	}
}

void CVoxelTerrain::CullTree16 ( CBoob * pBoob, subblock16 * block, char const index, RangeVector const& position )
{
	// Draw current block
	for ( char i = 0; i < 8; i += 1 )
	{
		char result;
		result = CCamera::activeCamera->SphereIsVisible(
			Vector3d(
				ftype(position.x*64 + ((index%2))*32 + 16),
				ftype(position.y*64 + ((index/2)%2)*32 + 16),
				ftype(position.z*64 + ((index/4))*32 + 16)
				), 27 //16
			);
		if ( result == 2 )
		{
			//if ( !pBoob->visible[index] )
			//	needUpdateOnVBO = true;
			//pBoob->visible[index] = true;
			/*for ( char j = 0; j < 8; j += 1 )
			{
				block->visible[j] = true;
			}*/
		}
		else if ( result == 1 )
		{
			//pBoob->visible[index] = false;
			CullTree8( pBoob, &(block->data[i]), index, i, position );
		}
		else if ( result == 0 )
		{
			//if ( pBoob->visible[index] )
			//	needUpdateOnVBO = true;
			//pBoob->visible[index] = false;
		}

		/*if ( pBoob->visible[i] )
		{
			CullTree8( pBoob, &(block->data[i]), index, i, position );
		}*/
	}
}
void CVoxelTerrain::CullTree8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, RangeVector const& position )
{

}