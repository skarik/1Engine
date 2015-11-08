#include "CVoxelTerrain.h"
#include "CTimeProfiler.h"

// == Secondary Renderer (Water+Translucents) ==
CVoxelTerrainTranslucentRenderer::CVoxelTerrainTranslucentRenderer ( CVoxelTerrain* newOwner )
	: pMaster( newOwner ), CRenderableObject()
{
	// Load a texture
	myMat = new glMaterial;
	myMat->loadTexture( ".res\\textures\\terraTexture.jpg" );
	myMat->useColors = true;
	myMat->setShader( new glShader( ".res\\shaders\\world\\terrainDefault.glsl" ) );
	myMat->isTransparent = true;
	myMat->useDepthMask = false; // Play with this value. The water is generally rather opaque, yes?
	myMat->iFaceMode = myMat->FM_FRONTANDBACK;	// Draw both the topside and backside of the water

	// Set the material as our base material
	this->SetMaterial( myMat );
}

CVoxelTerrainTranslucentRenderer::~CVoxelTerrainTranslucentRenderer ( void )
{
	myMat->releaseOwnership();
	//delete myMat; TODO
}

bool CVoxelTerrainTranslucentRenderer::Render ( const char pass )
{
	myMat->bind();
	myMat->setShaderConstants( this );

	// do same as vertex array except pointer
	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

	// Reset drawn
	pMaster->ResetBlockDrawn ( pMaster->root );
	// Draw water
	glTranslatef( 0,0,-0.1f );
	pMaster->RenderWaterOcttree( pMaster->root, pMaster->root_position );

	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	//myMat->unbind();

	return true;
}

// Terrain cull
bool CVoxelTerrain::PreRender ( const char pass )
{
	// Lock the terrain while updating
	//mutex::scoped_lock local_lock( mtGaurd );

	// Reset drawn
	ResetBlockDrawn ( root );

	// == VISUALS ==
	// Do culling
	CullTree32( root, root_position );

	return true;
}

// Rendering
bool CVoxelTerrain::Render ( const char pass )
{
	//glEnable(GL_CULL_FACE);
	//RenderBoob( root, root_position );
	//RenderTree32( root, root_position );
	//TimeProfiler.BeginTimeProfile( "terradraw" );

	myMat->bind();
	myMat->setShaderConstants( this );

	//{
		// Lock the terrain while writing to data
		//mutex::scoped_lock local_lock( mtGaurd );

	//}

	if ( CCamera::pActiveCamera->shadowCamera )
		glDisable( GL_CULL_FACE );
	
	// do same as vertex array except pointer
	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	/*
	// Tell where the vertex coordinates are in the array
	glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
	//glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), (char*)0 + (sizeof(float)*6) );
	glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
	//glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), (char*)0 + (sizeof(float)*12) );

	// Draw the sutff
	glDrawElements( GL_QUADS, faceCount*4, GL_UNSIGNED_INT, 0 );
	
	*/

	RenderOcttree( root, root_position );

	//glEnable ( GL_COLOR_MATERIAL ) ;

	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	if ( CCamera::pActiveCamera->shadowCamera )
		glEnable( GL_CULL_FACE );

	//myMat.unbind();
	//myMat->unbind();

	// Render the terrain
	// bind VBOs for vertex array and index array
	/*glBindBufferARB(GL_ARRAY_BUFFER_ARB, root->pMesh->iVBOverts);         // for vertex coordinates
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, root->pMesh->iVBOfaces); // for face vertex indexes

	// do same as vertex array except pointer
	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);

	// Tell where the vertex coordinates are in the array
	glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
	glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );

	// Draw the sutff
	glDrawElements( GL_QUADS, root->pMesh->faceCount*4, GL_UNSIGNED_INT, 0 );

	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);*/

	//TimeProfiler.EndPrintTimeProfile( "terradraw" );

	return true;
}

void CVoxelTerrain::RenderOcttree ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[1] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[1] = true;

			// If visible...
			if (( pBoob->bitinfo[0] == true )&&( pBoob->hasBlockData )&&( pBoob->pMesh->faceCount > 0 ))
			{
				// Set the position
				//glLoadIdentity();
				//glTranslatef( position.x*64.0f, position.y*64.0f, position.z*64.0f );

				// Draw current boob
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, pBoob->pMesh->iVBOverts);         // for vertex coordinates
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, pBoob->pMesh->iVBOfaces); // for face vertex indexes

				// Tell where the vertex coordinates are in the array
				glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
				glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
				glTexCoordPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
				glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );

				// Draw the sutff
				glDrawElements( GL_QUADS, pBoob->pMesh->faceCount*4, GL_UNSIGNED_SHORT, 0 );

			}

			// Draw other boobs
			RenderOcttree( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			RenderOcttree( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			RenderOcttree( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			RenderOcttree( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			RenderOcttree( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			RenderOcttree( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );

			// Draw the water back to center
			/*if ( pBoob->bitinfo[0] == true )
			{
				// ===== DRAW THE WATER ======

				// Draw current boob
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, pBoob->pMesh->iWaterVBOverts);         // for vertex coordinates
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, pBoob->pMesh->iWaterVBOfaces); // for face vertex indexes

				// Tell where the vertex coordinates are in the array
				glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
				glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
				glTexCoordPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
				glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );

				// Draw the sutff
				glDrawElements( GL_QUADS, pBoob->pMesh->faceCountWater*4, GL_UNSIGNED_SHORT, 0 );
			}*/
		}
	}
}

void CVoxelTerrain::RenderWaterOcttree ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[1] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[1] = true;

			// Draw other boobs (render back-to-front)
			RenderWaterOcttree( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			RenderWaterOcttree( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			RenderWaterOcttree( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			RenderWaterOcttree( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			RenderWaterOcttree( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			RenderWaterOcttree( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );

			// Draw the water back to center
			//if ( pBoob->bitinfo[0] == true )
			if (( pBoob->bitinfo[0] == true )&&( pBoob->hasBlockData )&&( pBoob->pMesh->faceCountWater > 0 ))
			{
				// ===== DRAW THE WATER ======

				// Draw current boob
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, pBoob->pMesh->iWaterVBOverts);         // for vertex coordinates
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, pBoob->pMesh->iWaterVBOfaces); // for face vertex indexes

				// Tell where the vertex coordinates are in the array
				glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
				glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
				glTexCoordPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
				glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );

				// Draw the sutff
				glDrawElements( GL_QUADS, pBoob->pMesh->faceCountWater*4, GL_UNSIGNED_SHORT, 0 );
			}
		}
	}
}