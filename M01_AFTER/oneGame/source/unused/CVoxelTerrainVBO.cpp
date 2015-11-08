#include "CVoxelTerrain.h"
#include "CTimeProfiler.h"

using std::cout;
using std::endl;

void CVoxelTerrain::UpdateTerrainMesh ( void )
{
	GenerateTree32( root, root_position );
}

void CVoxelTerrain::GenerateTree32 ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if (( pBoob->bitinfo[5] == false )&&( pBoob->hasBlockData ))
		{
			// Set VBO generation flag
			pBoob->bitinfo[5] = true;

			// If visible or the terrain is in loading mode
			if (( !bUpdateCameraPos )||( pBoob->bitinfo[0] == true ))
			{
				// And we want to update this area
				if ( pBoob->pMesh->needUpdateOnVBO )
				{
					if ( pBoob->pMesh->PrepareRegen() )
					{
						// Lock the terrain while reading from data
						//mutex::scoped_lock local_lock( mtGaurd );

						pBoob->pMesh->vCameraPos = vCameraPos;

						//TimeProfiler.BeginTimeProfile( "meshRegen" );
						// Update current boob
						for ( char i = 0; i < 8; i += 1 )
						{
							GenerateTree16( pBoob, &(pBoob->data[i]), i, position );
						}
						//TimeProfiler.EndPrintTimeProfile( "meshRegen" );
						
						// Generate the visual mesh
						if (( pBoob->pMesh->vertexCount >= 4 )&&( pBoob->pMesh->faceCount >= 2 )) {
							// Generate the mesh
							pBoob->pMesh->UpdateVBOMesh();
						}
						// Clean up the mesh
						pBoob->pMesh->CleanupRegen();
						// It's been updated
						pBoob->pMesh->needUpdateOnVBO = false;
					}
					else
					{
						pBoob->pMesh->needUpdateOnVBO = false;
						cout << "Terrain mesh allocation: Error in allocating memory for address " << pBoob << endl;
						bFailedBadAlloc = true;
					}
				}

				// update water
				if ( pBoob->pMesh->needUpdateOnWaterVBO )
				{
					// Allocate the memory
					if ( pBoob->pMesh->PrepareWaterRegen() )
					{
						// Lock the terrain while reading from data
						//mutex::scoped_lock local_lock( mtGaurd );

						// Update current boob
						for ( char i = 0; i < 8; i += 1 )
						{
							GenerateWater16( pBoob, &(pBoob->data[i]), i, position );
						}
						if (( pBoob->pMesh->vertexCountWater >= 4 )&&( pBoob->pMesh->faceCountWater >= 2 )) {
							// Generate the mesh
							pBoob->pMesh->UpdateWaterVBOMesh();
						}
						// Clean up the mesh
						pBoob->pMesh->CleanupWaterRegen();
						// It's been updated
						pBoob->pMesh->needUpdateOnWaterVBO = false;
					}
					else
					{
						pBoob->pMesh->needUpdateOnWaterVBO = false;
						cout << "Water mesh allocation: Error in allocating memory for address " << pBoob << endl;
						bFailedBadAlloc = true;
					}
				}
			}

			// Update other boobs
			GenerateTree32( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			GenerateTree32( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			GenerateTree32( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			GenerateTree32( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			GenerateTree32( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			GenerateTree32( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
	}
}

inline bool IsOpaque ( unsigned short blocktype )
{
	return (( blocktype != EB_NONE )&&( blocktype != EB_WATER ));
}
void CVoxelTerrain::GenerateTree16 ( CBoob * pBoob, subblock16 * block, char const index, RangeVector const& position )
{
	// Check if area is solid first
	pBoob->solid[index] = true;
	for ( char i = 0; i < 8; i += 1 )
	{
		if ( pBoob->solid[index] )
		{
			for ( short j = 0; j < 512; j += 1 )
			{
				if (( block->data[i].data[j].block == EB_NONE )||( block->data[i].data[j].block == EB_WATER ))
				{
					pBoob->solid[index] = false;
					break;
				}
			}
		}
	}
	// If area is not solid, then create lighting and mesh for the area
	if ( !pBoob->solid[index] )
	{
		// Update current block vbo
		for ( char i = 0; i < 8; i += 1 )
		{
			// Generate the lighting first
			LightTree8( pBoob, &(block->data[i]), index,i );
		}
		for ( char i = 0; i < 8; i += 1 )
		{
			// Generate the tree next
			GenerateTree8( pBoob, &(block->data[i]), index, i, position );
		}
	}
}
void CVoxelTerrain::GenerateTree8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, RangeVector const& position )
{
	// Draw current block
	Vector3d baseOffset = Vector3d(
		position.x*32.0f+((index%2)*16)+((subindex%2)*8),
		position.y*32.0f+(((index/2)%2)*16)+(((subindex/2)%2)*8),
		position.z*32.0f+((index/4)*16)+((subindex/4)*8) );

	Vector3d pos;

	//const int bsize = 2;

	// Draw current block
	char res = pBoob->current_resolution;
	char i, j, k;
	short b_index;
	for ( k = 0; k < 8; k += res )
	{
		for ( j = 0; j < 8; j += res )
		{
			for ( i = 0; i < 8; i += res )
			{
				/*if ( block->data[i+j*8+k*64] > 0 )
				{
					//if ( BlockVisibleEx( (char*)(block->data), i+j*8+k*64, 8 ) )
					//{
						pos.x = (baseOffset.x + (ftype)i)*2.0f;
						pos.y = (baseOffset.y + (ftype)j)*2.0f;
						pos.z = (baseOffset.z + (ftype)k)*2.0f;
						
						try
						{
							pBoob->pMesh->GenerateCube( pBoob, index,subindex,i+j*8+k*64, pos, res );
						}
						catch ( C_xcp_vertex_overflow& exception )
						{
							if (exception.what())
								MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
						}
					//}
				}*/
				/*if (( block->data[i+j*8+k*64] == EB_NONE )||( block->data[i+j*8+k*64] == EB_WATER ))
				{
					pos.x = (baseOffset.x + (ftype)i)*2.0f;
					pos.y = (baseOffset.y + (ftype)j)*2.0f;
					pos.z = (baseOffset.z + (ftype)k)*2.0f;
					
					try
					{
						pBoob->pMesh->GenerateInCube( pBoob, index,subindex,i+j*8+k*64, pos, res );
					}
					catch ( C_xcp_vertex_overflow& exception )
					{
						if (exception.what())
							MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
					}
				}*/

				/*{	// Use Smooth Gen
					pos.x = (baseOffset.x + (ftype)i)*2.0f;
					pos.y = (baseOffset.y + (ftype)j)*2.0f;
					pos.z = (baseOffset.z + (ftype)k)*2.0f;
					
					try
					{
						pBoob->pMesh->GenerateInVoxel( pBoob, index,subindex,i+j*8+k*64, pos, res );
					}
					catch ( C_xcp_vertex_overflow& exception )
					{
						if (exception.what())
							MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
					}
				}
				
				if (( block->data[i+j*8+k*64] == EB_NONE )||( block->data[i+j*8+k*64] == EB_WATER ))
				{	// Use Block Gen
					pos.x = (baseOffset.x + (ftype)i)*2.0f;
					pos.y = (baseOffset.y + (ftype)j)*2.0f;
					pos.z = (baseOffset.z + (ftype)k)*2.0f;
					
					try
					{
						pBoob->pMesh->GenerateInCube( pBoob, index,subindex,i+j*8+k*64, pos, res );
					}
					catch ( C_xcp_vertex_overflow& exception )
					{
						if (exception.what())
							MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
					}
				}*/
				b_index = i+j*8+k*64;
				if (( block->data[b_index].block == EB_NONE )||( block->data[b_index].block == EB_WATER ))
				{
					pos.x = (baseOffset.x + (ftype)i)*2.0f;
					pos.y = (baseOffset.y + (ftype)j)*2.0f;
					pos.z = (baseOffset.z + (ftype)k)*2.0f;
					try
					{
						pBoob->pMesh->AddBlockMesh( pBoob, index, subindex, b_index, pos, res );
					}
					catch ( C_xcp_vertex_overflow& exception )
					{
						if (exception.what())
							MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK",MB_OK | MB_ICONINFORMATION );
					}
				}
			}
		}
	}
}

void CVoxelTerrain::GenerateWater16 ( CBoob * pBoob, subblock16 * block, char const index, RangeVector const& position )
{
	// Update current block's water vbo
	for ( char i = 0; i < 8; i += 1 )
	{
		GenerateWater8( pBoob, &(block->data[i]), index, i, position );
	}
}
void CVoxelTerrain::GenerateWater8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, RangeVector const& position )
{
	// Draw current block
	Vector3d baseOffset = Vector3d(
		position.x*32.0f+((index%2)*16)+((subindex%2)*8),
		position.y*32.0f+(((index/2)%2)*16)+(((subindex/2)%2)*8),
		position.z*32.0f+((index/4)*16)+((subindex/4)*8) );

	Vector3d pos;

	//const int bsize = 2;

	// Draw current block
	char res = pBoob->current_resolution;
	char i, j, k;
	for ( k = 0; k < 8; k += res )
	{
		for ( j = 0; j < 8; j += res )
		{
			for ( i = 0; i < 8; i += res )
			{
				if ( block->data[i+j*8+k*64].block == EB_NONE ) // transparent non-water blocks only
				{
					pos.x = (baseOffset.x + (ftype)i)*2.0f;
					pos.y = (baseOffset.y + (ftype)j)*2.0f;
					pos.z = (baseOffset.z + (ftype)k)*2.0f;
					
					try
					{
						pBoob->pMesh->GenerateInCubeWater( pBoob, index,subindex,i+j*8+k*64, pos, res );
					}
					catch ( C_xcp_vertex_overflow& exception )
					{
						if (exception.what())
							MessageBox ( NULL,"Vertex indexes have had an overflow. Report such exceptions immediately.","FACK FACK FACK MEMORY FACK (water edition!)",MB_OK | MB_ICONINFORMATION );
					}
				}

				//MessageBox( NULL, "GenerateWater8, CVoxelTerrainVBO.cpp", "TODO", 0 );
			}
		}
	}
}

void CVoxelTerrain::LOD_Level1_UpdateMesh ( void )
{
	if ( bLODWantsRegen )
	{
		pLOD_Level1Mesh->PrepareRegen();
		terra_t_lod* dataSet = GetLODDataBlock(1);
		pLOD_Level1Mesh->m_dataSet = dataSet;

		static terra_t_lod dataSet_temp [64];

		// Generate mesh
		char i, j, k;
		Vector3d pos;

		for ( k = 0; k < 4; k += 1 ) {
			for ( j = 0; j < 4; j += 1 ) {
				for ( i = 0; i < 4; i += 1 ) {
					dataSet_temp[i+j*4+k*16] = dataSet[(i+15)+(j+15)*32+(k+15)*1024];
					dataSet[(i+15)+(j+15)*32+(k+15)*1024].solid = 0;
					dataSet[(i+15)+(j+15)*32+(k+15)*1024].loaded = 0;
				}
			}
		}

		for ( k = 0; k < 32; k += 1 )
		{
			for ( j = 0; j < 32; j += 1 )
			{
				for ( i = 0; i < 32; i += 1 )
				{
					/*if (( k >= 15 && k <= 18 )&&( j >= 15 && j <= 18 )&&( i >= 15 && i <= 18 )) {
						//dataSet[i+j*32+k*1024].solid = false;
						continue;
					}*/

					if (( !dataSet[i+j*32+k*1024].solid )
						||
						(( k >= 15 && k <= 18 )&&( j >= 15 && j <= 18 )&&( i >= 15 && i <= 18 )))
					{
						pos.x = (ftype)i*64;
						pos.y = (ftype)j*64;
						pos.z = (ftype)k*64;

						pLOD_Level1Mesh->AddBlockLOD( i,j,k, pos, 64.0f );
					}
				}
			}
		}

		for ( k = 0; k < 4; k += 1 ) {
			for ( j = 0; j < 4; j += 1 ) {
				for ( i = 0; i < 4; i += 1 ) {
					dataSet[(i+15)+(j+15)*32+(k+15)*1024].solid = dataSet_temp[i+j*4+k*16].solid;
					dataSet[(i+15)+(j+15)*32+(k+15)*1024].loaded = dataSet_temp[i+j*4+k*16].loaded;
				}
			}
		}

		// Send mesh to VBO
		if (( pLOD_Level1Mesh->vertexCount >= 4 )&&( pLOD_Level1Mesh->faceCount >= 2 )) {
			// Generate the mesh
			pLOD_Level1Mesh->UpdateVBOMesh();
			//cout << "MESH FACE COUNT: " << pLOD_Level1Mesh->faceCount << endl;
		}

		pLOD_Level1Mesh->CleanupRegen();

		lod_root_draw_position = root_position;
		bLODWantsRegen = false;
		bLODCanDraw_L1 = true;
	}
}