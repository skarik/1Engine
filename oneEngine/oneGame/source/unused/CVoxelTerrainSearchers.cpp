
// class CVoxelTerrain
// CVoxelTerrainSearchers.cpp
// 
// Definitions for searching routines and item creation routines.
//


#include "CVoxelTerrain.h"
#include "ItemTerraBlok.h"
#include "game/item/CWeaponItem.h"

// --Grabbin roots-
const RangeVector&	CVoxelTerrain::GetRootPosition ( void ) const
{
	return root_position;
}

// --Checkin actives--
bool CVoxelTerrain::GetActiveCollision ( const Vector3d& position )
{
	// Move position into boobspace
	/*Vector3d dif = position;
	dif.x /= (boobSize.x*blockSize);
	dif.y /= (boobSize.y*blockSize);
	dif.z /= (boobSize.z*blockSize);

	// Check range on it
	dif -= Vector3d( (ftype)root_position.x, (ftype)root_position.y, (ftype)root_position.z );

	if ( fabs(dif.x) > iTerrainSize-1 )
	{
		return false;
	}
	else if ( fabs(dif.y) > iTerrainSize-1 )
	{
		return false;
	}
	else if ( fabs(dif.z) > iTerrainSize-1 )
	{
		return false;
	}
	return true;*/

	CBoob* result = root;
	Vector3d pos = position;
	pos.x /= (boobSize.x*blockSize);
	pos.y /= (boobSize.y*blockSize);
	pos.z /= (boobSize.z*blockSize);

	ftype dif;
	const static Vector3d offset ( 0,0,0 );

	if ( result == NULL ) {
		return false;
	}

	// Go forward
	if ( pos.x > root_position.x )
	{
		dif = (pos.x-root_position.x)-1+offset.x;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			result = result->front;
			if ( result == NULL )
				return false;
		}
	}
	else // Go backward
	{
		dif = (root_position.x-pos.x)+offset.x;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			result = result->back;
			if ( result == NULL )
				return false;
		}
	}

	// Go left
	if ( pos.y > root_position.y )
	{
		dif = (pos.y-root_position.y)-1+offset.y;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			result = result->left;
			if ( result == NULL )
				return false;
		}
	}
	else // Go right
	{
		dif = (root_position.y-pos.y)+offset.y;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			result = result->right;
			if ( result == NULL )
				return false;
		}
	}

	// Go up
	if ( pos.z > root_position.z )
	{
		dif = (pos.z-root_position.z)-1+offset.z;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			result = result->top;
			if ( result == NULL )
				return false;
		}
	}
	else // Go down
	{
		dif = (root_position.z-pos.z)+offset.z;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			result = result->bottom;
			if ( result == NULL )
				return false;
		}
	}

	if ( result == NULL ) {
		return false;
	}

	return result->hasBlockData;
}

// --Looker functions for lookin--
CBoob*		CVoxelTerrain::GetBoobAtPosition ( Vector3d const& position )
{
	// Need floor( x,y,z divided by boobSize*blockSize )
	/*int posx = (int)floor( position.x/(boobSize.x*blockSize) );
	int posy = (int)floor( position.y/(boobSize.y*blockSize) );
	int posz = (int)floor( position.z/(boobSize.z*blockSize) );*/
	CBoob* result = root;
	Vector3d pos = position;
	pos.x /= (boobSize.x*blockSize);
	pos.y /= (boobSize.y*blockSize);
	pos.z /= (boobSize.z*blockSize);
	ftype dif;
	//const static Vector3d offset ( 1.0f/boobSize.x, 1.0f/boobSize.y, 1.0f/boobSize.z );
	const static Vector3d offset ( 0,0,0 );

	// Go forward
	if ( pos.x > root_position.x )
	{
		dif = (pos.x-root_position.x)-1+offset.x;
		for ( ftype i = 0; i < dif; i += 1 )
		{
			if ( result != NULL )
				result = result->front;
			//if ( result == NULL )
			//	throw 
		}
	}
	else // Go backward
	{
		dif = (root_position.x-pos.x)+offset.x;
		for ( ftype i = 0; i < dif; i += 1 )
			if ( result != NULL )
				result = result->back;
	}

	// Go left
	if ( pos.y > root_position.y )
	{
		dif = (pos.y-root_position.y)-1+offset.y;
		for ( ftype i = 0; i < dif; i += 1 )
			if ( result != NULL )
				result = result->left;
	}
	else // Go right
	{
		dif = (root_position.y-pos.y)+offset.y;
		for ( ftype i = 0; i < dif; i += 1 )
			if ( result != NULL )
				result = result->right;
	}

	// Go up
	if ( pos.z > root_position.z )
	{
		dif = (pos.z-root_position.z)-1+offset.z;
		for ( ftype i = 0; i < dif; i += 1 )
			if ( result != NULL )
				result = result->top;
	}
	else // Go down
	{
		dif = (root_position.z-pos.z)+offset.z;
		for ( ftype i = 0; i < dif; i += 1 )
			if ( result != NULL )
			result = result->bottom;
	}

	return result;
}
subblock16*	CVoxelTerrain::GetSubblock16AtPosition ( Vector3d const& position )
{
	return NULL;
}
subblock8*	CVoxelTerrain::GetSubblock8AtPosition ( Vector3d const& position )
{
	return NULL;
}
Terrain::terra_t	CVoxelTerrain::GetBlockAtPosition ( Vector3d const& position )
{
	BlockInfo block;
	block.block.block = EB_NONE;
	GetBlockInfoAtPosition( position, block );
	return block.block;
}
bool		CVoxelTerrain::GetBlockInfoAtPosition ( Vector3d const& position, BlockInfo & finalBlockInfo )
{
	// Need floor( x,y,z divided by boobSize*blockSize )
	//BlockInfo finalBlockInfo;
	CBoob* result = root;
	Vector3d pos = position;
	pos.x /= (boobSize.x*blockSize);
	pos.y /= (boobSize.y*blockSize);
	pos.z /= (boobSize.z*blockSize);
	ftype dif, i;
	ftype dummy;
	//const static Vector3d offset ( 1.0f/boobSize.x, 1.0f/boobSize.y, 1.0f/boobSize.z );
	const static Vector3d offset ( 0.0f/boobSize.x, 0.0f/boobSize.y, 0.0f/boobSize.z );
	
	/*
	// Debug output
	char szTitle[256]={0};
	sprintf( szTitle, "%lf %lf %lf\n", position.x,position.y,position.z );
	OutputDebugStringA( szTitle );
	*/

	// Get the current boob
	// Go forward
	if ( pos.x > root_position.x )
	{
		dif = (pos.x-root_position.x)-1+offset.x;
		for ( i = 0; i < dif; i += 1 )
		{
			result = result->front;
			if ( result == NULL )
				return false;
		}
	}
	else // Go backward
	{
		dif = (root_position.x-pos.x)-offset.x;
		for ( i = 0; i < dif; i += 1 )
		{
			result = result->back;
			if ( result == NULL )
				return false;
		}
	}
	// Go left
	if ( pos.y > root_position.y )
	{
		dif = (pos.y-root_position.y)-1+offset.y;
		for ( i = 0; i < dif; i += 1 )
		{
			result = result->left;
			if ( result == NULL )
				return false;
		}
	}
	else // Go right
	{
		dif = (root_position.y-pos.y)-offset.y;
		for ( i = 0; i < dif; i += 1 )
		{
			result = result->right;
			if ( result == NULL )
				return false;
		}
	}
	// Go up
	if ( pos.z > root_position.z )
	{
		dif = (pos.z-root_position.z)-1+offset.z;
		for ( i = 0; i < dif; i += 1 )
		{
			result = result->top;
			if ( result == NULL )
				return false;
		}
	}
	else // Go down
	{
		dif = (root_position.z-pos.z)-offset.z;
		for ( i = 0; i < dif; i += 1 )
		{
			result = result->bottom;
			if ( result == NULL )
				return false;
		}
	}

	// Final check
	if ( result == NULL )
		return false;
	finalBlockInfo.pBoob = result;
	// Found Boob

	// Find current subblock16
	pos.x = modf( pos.x, &dummy );
	pos.y = modf( pos.y, &dummy );
	pos.z = modf( pos.z, &dummy );
	if ( pos.x < 0 )
		pos.x += 1;
	if ( pos.y < 0 )
		pos.y += 1;
	if ( pos.z < 0 )
		pos.z += 1;

	/*
	// Debug output
	//char szTitle[256]={0};
	sprintf( szTitle, "%lf %lf %lf\n", pos.x,pos.y,pos.z );
	OutputDebugStringA( szTitle );
	*/

	pos *= 2.0f;

	finalBlockInfo.b16index = 0;
	//for ( i = 0; i < pos.x-1; i += 1 )
	if ( pos.x > 1.0f )
		finalBlockInfo.b16index += 1;
	//for ( i = 0; i < pos.y-1; i += 1 )
	if ( pos.y > 1.0f )
		finalBlockInfo.b16index += 2;
	//for ( i = 0; i < pos.z-1; i += 1 )
	if ( pos.z > 1.0f )
		finalBlockInfo.b16index += 4;
	finalBlockInfo.pBlock16 = &(finalBlockInfo.pBoob->data[finalBlockInfo.b16index]);
	// Found current subblock16

	// Find current subblock8
	pos.x = modf( pos.x, &dummy );
	pos.y = modf( pos.y, &dummy );
	pos.z = modf( pos.z, &dummy );

	pos *= 2.0f;

	finalBlockInfo.b8index = 0;
	//for ( i = 0; i < pos.x-1; i += 1 )
	if ( pos.x > 1.0f )
		finalBlockInfo.b8index += 1;
	//for ( i = 0; i < pos.y-1; i += 1 )
	if ( pos.y > 1.0f )
		finalBlockInfo.b8index += 2;
	//for ( i = 0; i < pos.z-1; i += 1 )
	if ( pos.z > 1.0f )
		finalBlockInfo.b8index += 4;
	finalBlockInfo.pBlock8 = &(finalBlockInfo.pBoob->data[finalBlockInfo.b16index].data[finalBlockInfo.b8index]);
	// Found current subblock8

	// Find current block
	pos.x = modf( pos.x, &dummy );
	pos.y = modf( pos.y, &dummy );
	pos.z = modf( pos.z, &dummy );

	pos *= 8.0f;

	finalBlockInfo.b1index = 0;
	for ( i = 1; i < pos.x; i += 1 )
		finalBlockInfo.b1index += 1;
	for ( i = 1; i < pos.y; i += 1 )
		finalBlockInfo.b1index += 8;
	for ( i = 1; i < pos.z; i += 1 )
		finalBlockInfo.b1index += 64;
	finalBlockInfo.block = finalBlockInfo.pBoob->data[finalBlockInfo.b16index].data[finalBlockInfo.b8index].data[finalBlockInfo.b1index];
	finalBlockInfo.pBlock = &(finalBlockInfo.pBoob->data[finalBlockInfo.b16index].data[finalBlockInfo.b8index].data[finalBlockInfo.b1index]);
	// Found current block

	//return finalBlockInfo;
	return true;
}

bool		CVoxelTerrain::IsBlockOnEdge ( BlockInfo const& block, EFaceDir direction )
{
	return IsBlockOnEdge( block.b16index, block.b8index, block.b1index, direction );
}
bool		CVoxelTerrain::IsBlockOnEdge ( char index16, char index8, short i, EFaceDir direction )
{
	const char width = 8;
	const char ofs = 1;

	switch ( direction )
	{
	case FRONT:
		{
			if ( i%width < width-ofs )
			{
				//i = i+ofs;
			}
			else
			{
				if ( index8%2 != 1 )
				{
					/*i = i-(width-ofs);
					index8 = index8+1;*/
				}
				else if ( index16%2 != 1 )
				{
					/*i = i-(width-ofs);
					index8 = index8-1;
					index16 = index16+1;*/
				}
				else// if ( (*pBoob)->front != NULL )
				{
					/*i = i-(width-ofs);
					index8 = index8-1;
					index16 = index16-1;
					*pBoob = (*pBoob)->front;*/
					return true;
				}
				/*else
				{
					return false;
				}*/
			}
		}
		break;
	case BACK:
		{
			if ( i%width >= ofs )
			{
				//i = i-ofs;
			}
			else
			{
				if ( index8%2 != 0 )
				{
					/*i = i+(width-ofs);
					index8 = index8-1;*/
				}
				else if ( index16%2 != 0 )
				{
					/*i = i+(width-ofs);
					index8 = index8+1;
					index16 = index16-1;*/
				}
				else// if ( (*pBoob)->back != NULL )
				{
					/*i = i+(width-ofs);
					index8 = index8+1;
					index16 = index16+1;
					*pBoob = (*pBoob)->back;*/
					return true;
				}
				/*else
				{
					return false;
				}*/
			}
		}
		break;
	case LEFT:
		{
			if ( ((i/width)%width) < width-ofs )
			{
				//i = i+(width*ofs);
			}
			else
			{
				if ( ((index8/2)%2) != 1 )
				{
					/*i = i-(width*(width-ofs));
					index8 = index8+2;*/
				}
				else if ( ((index16/2)%2) != 1 )
				{
					/*i = i-(width*(width-ofs));
					index8 = index8-2;
					index16 = index16+2;*/
				}
				else// if ( (*pBoob)->left != NULL )
				{
					/*i = i-(width*(width-ofs));
					index8 = index8-2;
					index16 = index16-2;
					*pBoob = (*pBoob)->left;*/
					return true;
				}
				/*else
				{
					return false;
				}*/
			}
		}
		break;
	case RIGHT:
		{
			if ( ((i/width)%width) >= ofs )
			{
				//i = i-(width*ofs);
			}
			else
			{
				if ( ((index8/2)%2) != 0 )
				{
					/*i = i+(width*(width-ofs));
					index8 = index8-2;*/
				}
				else if ( ((index16/2)%2) != 0 )
				{
					/*i = i+(width*(width-ofs));
					index8 = index8+2;
					index16 = index16-2;*/
				}
				else// if ( (*pBoob)->right != NULL )
				{
					/*i = i+(width*(width-ofs));
					index8 = index8+2;
					index16 = index16+2;
					*pBoob = (*pBoob)->right;*/
					return true;
				}
				/*else
				{
					return false;
				}*/
			}
		}
		break;
	case TOP:
		{
			if ( ((i/width/width)%width) != width-ofs )
			{
				//i = i+(width*width*ofs);
			}
			else
			{
				if ( ((index8/4)%2) != 1 )
				{
					/*i = i-(width*width*(width-ofs));
					index8 = index8+4;*/
				}
				else if ( ((index16/4)%2) != 1 )
				{
					/*i = i-(width*width*(width-ofs));
					index8 = index8-4;
					index16 = index16+4;*/
				}
				else// if ( (*pBoob)->top != NULL )
				{
					/*i = i-(width*width*(width-ofs));
					index8 = index8-4;
					index16 = index16-4;
					*pBoob = (*pBoob)->top;*/
					return true;
				}
				/*else
				{
					return false;
				}*/
			}
		}
		break;
	case BOTTOM:
		{
			if ( ((i/width/width)%width) >= ofs )
			{
				//i = i-(width*width*ofs);
			}
			else
			{
				if ( ((index8/4)%2) != 0 )
				{
					/*i = i+(width*width*(width-ofs));
					index8 = index8-4;*/
				}
				else if ( ((index16/4)%2) != 0 )
				{
					/*i = i+(width*width*(width-ofs));
					index8 = index8+4;
					index16 = index16-4;*/
				}
				else// if ( (*pBoob)->bottom != NULL )
				{
					/*i = i+(width*width*(width-ofs));
					index8 = index8+4;
					index16 = index16+4;
					*pBoob = (*pBoob)->bottom;*/
					return true;
				}
				/*else
				{
					return false;
				}*/
			}
		}
		break;
	}

	return false;
}

bool		CVoxelTerrain::SetBlockAtPosition ( Vector3d const& position, ushort nData )
{
	BlockInfo infoes;
	if ( GetBlockInfoAtPosition( position, infoes ) )
	{
		if ( infoes.pBlock->block != nData )
		{
			infoes.pBlock->block = nData;
			infoes.pBoob->pMesh->needUpdateOnVBO = true;
			infoes.pBoob->pCollision->needUpdateOnCollider = true;

			infoes.pBoob->pCollision->update[infoes.b16index] = true;

			if ( infoes.pBoob->top )
				if ( IsBlockOnEdge( infoes, TOP ) )
					infoes.pBoob->top->pMesh->needUpdateOnVBO = true;
			if ( infoes.pBoob->bottom )
				if ( IsBlockOnEdge( infoes, BOTTOM ) )
					infoes.pBoob->bottom->pMesh->needUpdateOnVBO = true;
			if ( infoes.pBoob->left )
				if ( IsBlockOnEdge( infoes, LEFT ) )
					infoes.pBoob->left->pMesh->needUpdateOnVBO = true;
			if ( infoes.pBoob->right )
				if ( IsBlockOnEdge( infoes, RIGHT ) )
					infoes.pBoob->right->pMesh->needUpdateOnVBO = true;
			if ( infoes.pBoob->front )
				if ( IsBlockOnEdge( infoes, FRONT ) )
					infoes.pBoob->front->pMesh->needUpdateOnVBO = true;
			if ( infoes.pBoob->back )
				if ( IsBlockOnEdge( infoes, BACK ) )
					infoes.pBoob->back->pMesh->needUpdateOnVBO = true;
		}
		return true;
	}
	return false;
}

bool		CVoxelTerrain::SetBlock( BlockInfo const& infoes, ushort nData )
{
	if ( infoes.pBlock->block != nData )
	{
		infoes.pBlock->block = nData;
		infoes.pBoob->pMesh->needUpdateOnVBO = true;
		infoes.pBoob->pCollision->needUpdateOnCollider = true;

		infoes.pBoob->pCollision->update[infoes.b16index] = true;

		if ( infoes.pBoob->top )
			if ( IsBlockOnEdge( infoes, TOP ) )
				infoes.pBoob->top->pMesh->needUpdateOnVBO = true;
		if ( infoes.pBoob->bottom )
			if ( IsBlockOnEdge( infoes, BOTTOM ) )
				infoes.pBoob->bottom->pMesh->needUpdateOnVBO = true;
		if ( infoes.pBoob->left )
			if ( IsBlockOnEdge( infoes, LEFT ) )
				infoes.pBoob->left->pMesh->needUpdateOnVBO = true;
		if ( infoes.pBoob->right )
			if ( IsBlockOnEdge( infoes, RIGHT ) )
				infoes.pBoob->right->pMesh->needUpdateOnVBO = true;
		if ( infoes.pBoob->front )
			if ( IsBlockOnEdge( infoes, FRONT ) )
				infoes.pBoob->front->pMesh->needUpdateOnVBO = true;
		if ( infoes.pBoob->back )
			if ( IsBlockOnEdge( infoes, BACK ) )
				infoes.pBoob->back->pMesh->needUpdateOnVBO = true;
	}
	return true;
}


CWeaponItem* CVoxelTerrain::ItemizeBlockAtPosition ( Vector3d const& position )
{
	BlockInfo infoes;
	if ( GetBlockInfoAtPosition( position, infoes ) )
	{
		return ItemizeBlock( infoes );
	}
	return NULL;
}
CWeaponItem* CVoxelTerrain::ItemizeBlock( BlockInfo const& infoes )
{
	ItemTerraBlok* newBlok = NULL;

	// Check for certain blocks that need to be changed
	if ( infoes.pBlock->block == EB_GRASS )
		infoes.pBlock->block = EB_DIRT;

	// Now itemize it
	if ( infoes.pBlock->block != EB_NONE )
		newBlok = new ItemTerraBlok ( myMat, infoes.pBlock->block );

	if ( newBlok )
	{
		newBlok->transform.position 
			= (infoes.pBoob->position) - Vector3d(31,31,31) + (Vector3d(
			ftype( ((infoes.b16index%2)*16)+((infoes.b8index%2)*8)+(infoes.b1index%8) ),
			ftype( (((infoes.b16index/2)%2)*16)+(((infoes.b8index/2)%2)*8)+((infoes.b1index/8)%8) ),
			ftype( ((infoes.b16index/4)*16)+((infoes.b8index/4)*8)+(infoes.b1index/64) ) ) * 2.0f );
		newBlok->transform.SetDirty();
	}

	// And then turn the block to air
	SetBlock( infoes, EB_NONE );

	return (CWeaponItem*)(newBlok);
}

void CVoxelTerrain::DestroyBlock ( BlockInfo const& infoes )
{
	ItemizeBlock( infoes );
}
void CVoxelTerrain::CompressBlock( BlockInfo const& infoes )
{
	switch ( infoes.block.block ) {
		case EB_SAND:
			infoes.pBlock->block = EB_SANDSTONE;
			break;
		case EB_SNOW:
			infoes.pBlock->block = EB_ICE;
			break;
		case EB_ASH:
			infoes.pBlock->block = EB_CLAY;
			break;
		case EB_STONE:
			infoes.pBlock->block = EB_GRAVEL;
			break;
		case EB_DEADSTONE:
			infoes.pBlock->block = EB_CURSED_DEADSTONE; // nice try assholes :D
			break;
	}
}