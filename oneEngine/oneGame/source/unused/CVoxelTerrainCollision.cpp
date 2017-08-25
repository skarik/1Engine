#include "CVoxelTerrain.h"

void CVoxelTerrain::UpdateTerrainCollision ( void )
{
	//UpdateCollision32( root );

	mtTerraListGuard.lock();
	for ( vector<CBoob*>::iterator it = terraList.begin(); it != terraList.end(); ++it )
	{
		if ( (*it)->pCollision->needUpdateOnCollider ) {
			(*it)->pCollision->Update();
		}
	}
	mtTerraListGuard.unlock();
}

void CVoxelTerrain::UpdateCollision32 ( CBoob * pBoob )
{
	/*if ( pBoob != NULL )
	{
		if (( pBoob->bitinfo[7] == false )&&( pBoob->hasBlockData && pBoob->hasPropData ))
		{
			// Set misc flag
			pBoob->bitinfo[7] = true;

			if ( pBoob->pCollision->needUpdateOnCollider )
			{
				pBoob->pCollision->Update();
				//pBoob->pCollision->needUpdateOnCollider = false;
			}

			// Draw other boobs
			UpdateCollision32( pBoob->top );
			UpdateCollision32( pBoob->front );
			UpdateCollision32( pBoob->left );
			UpdateCollision32( pBoob->bottom );
			UpdateCollision32( pBoob->back );
			UpdateCollision32( pBoob->right );
		}
	}*/
}
