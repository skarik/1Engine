#include "CVoxelTerrain.h"
#include "CTimeProfiler.h"

void CVoxelTerrain::GenerateVBO ( void )
{
	GenerateTree32( root, root_position );
}

void CVoxelTerrain::GenerateTree32 ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( pBoob != NULL )
	{
		if (( pBoob->bitinfo[5] == false )&&( pBoob->hasBlockData ))
		{
			// Set VBO generation flag
			pBoob->bitinfo[5] = true;

			// If visible...
			if (( !bUpdateCameraPos )||( pBoob->bitinfo[0] == true ))
			{
				// And we want to update this area
				if ( pBoob->pMesh->needUpdateOnVBO )
				{
					/*pBoob->pMesh->needUpdateOnVBO = false;
					pBoob->pMesh->vCameraPos = vCameraPos;
					//pBoob->pMesh->vertexCount = 0;
					//pBoob->pMesh->faceCount = 0;
					pBoob->pMesh->PrepareRegen();
					// Update current boob
					for ( char i = 0; i < 8; i += 1 )
					{
						//if ( pBoob->visible[i] )
						//{
							GenerateTree16( pBoob, &(pBoob->data[i]), i, position );
						//}
					}

					//glBindBufferARB(GL_ARRAY_BUFFER_ARB, pBoob->pMesh->iVBOverts);         // for vertex coordinates
					//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, pBoob->pMesh->iVBOfaces); // for face vertex indexes
					//glBufferSubDataARB( GL_ARRAY_BUFFER_ARB, 0, pBoob->pMesh->vertexCount*sizeof(CTerrainVertex), pBoob->pMesh->vertices );
					//glBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0, pBoob->pMesh->faceCount*sizeof(CModelQuad), pBoob->pMesh->quads );

					pBoob->pMesh->UpdateRegen();

					pBoob->pMesh->CleanupRegen();*/

					// First state is just creating mesh data
					if ( pBoob->pMesh->iVBOUpdateState == 0 )
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
							pBoob->pMesh->UpdateRegen();

							
						}
						else
						{
							pBoob->pMesh->iVBOUpdateState = 0;
							pBoob->pMesh->needUpdateOnVBO = false;
							cout << "Terrain mesh allocation: Error in allocating memory for address " << pBoob << endl;
							bFailedBadAlloc = true;
						}
					}
					else if ( pBoob->pMesh->iVBOUpdateState == 3 )
					{
						if (( pBoob->pMesh->needUpdateOnWaterVBO )||( true ))
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
								// Generate the mesh
								pBoob->pMesh->UpdateWaterVBOMesh();
								// Clean up the mesh
								pBoob->pMesh->CleanupWaterRegen();
								// It's been updated
								pBoob->pMesh->needUpdateOnWaterVBO = false;
							}
							else
							{
								pBoob->pMesh->iVBOUpdateState = 0;
								pBoob->pMesh->needUpdateOnWaterVBO = false;
								cout << "Water mesh allocation: Error in allocating memory for address " << pBoob << endl;
								bFailedBadAlloc = true;
							}
						}

						// Run the rest
						pBoob->pMesh->UpdateRegen();
					}
					else
					{
						// Run the rest
						pBoob->pMesh->UpdateRegen();
					}
				}

				// update water
				if (( pBoob->pMesh->needUpdateOnWaterVBO )&&( !pBoob->pMesh->needUpdateOnVBO ))
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
						// Generate the mesh
						pBoob->pMesh->UpdateWaterVBOMesh();
						// Clean up the mesh
						pBoob->pMesh->CleanupWaterRegen();
						// It's been updated
						pBoob->pMesh->needUpdateOnWaterVBO = false;
					}
					else
					{
						pBoob->pMesh->iVBOUpdateState = 0;
						pBoob->pMesh->needUpdateOnWaterVBO = false;
						cout << "Water mesh allocation: Error in allocating memory for address " << pBoob << endl;
						bFailedBadAlloc = true;
					}
				}

				// Do an update on the water mesh if needed as well
				// And we want to update this area
				/*if (( pBoob->pMesh->needUpdateOnWaterVBO )&&( !pBoob->pMesh->needUpdateOnVBO )&&( pBoob->pMesh->iVBOUpdateState == 0 ))
				{
					// Allocate the memory
					if ( pBoob->pMesh->PrepareWaterRegen() )
					{
						// Update current boob
						for ( char i = 0; i < 8; i += 1 )
						{
							GenerateWater16( pBoob, &(pBoob->data[i]), i, position );
						}
						// Generate the mesh
						pBoob->pMesh->UpdateWaterVBOMesh();
						// Clean up the mesh
						pBoob->pMesh->CleanupWaterRegen();
						// It's been updated
						pBoob->pMesh->needUpdateOnWaterVBO = false;
					}
					else
					{
						pBoob->pMesh->needUpdateOnWaterVBO = false;
						cout << "Water mesh allocation: Error in allocating memory for address " << pBoob << endl;
					}
				}*/
			}

			// Draw other boobs
			GenerateTree32( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			GenerateTree32( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			GenerateTree32( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			GenerateTree32( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			GenerateTree32( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			GenerateTree32( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
	}
}
void CVoxelTerrain::GenerateTree16 ( CBoob * pBoob, subblock16 * block, char const index, LongIntPosition const& position )
{
	// Update current block vbo
	for ( char i = 0; i < 8; i += 1 )
	{
		//if ( block->visible[i] )
		//{
			// Generate the lighting first
			//LightTree8( pBoob, &(block->data[i]), index,i );
			// Generate the tree next
			GenerateTree8( pBoob, &(block->data[i]), index, i, position );
		//}
	}
}
void CVoxelTerrain::GenerateTree8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, LongIntPosition const& position )
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

				{	// Use Smooth Gen
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
				}
			}
		}
	}
}

void CVoxelTerrain::GenerateWater16 ( CBoob * pBoob, subblock16 * block, char const index, LongIntPosition const& position )
{
	// Update current block's water vbo
	for ( char i = 0; i < 8; i += 1 )
	{
		GenerateWater8( pBoob, &(block->data[i]), index, i, position );
	}
}
void CVoxelTerrain::GenerateWater8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, LongIntPosition const& position )
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
				if ( block->data[i+j*8+k*64] == EB_NONE ) // transparent non-water blocks only
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
			}
		}
	}
}