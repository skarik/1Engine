
// ======== CVoxelTerrainRendering =======
// Handles the rendering aspect of the terrain's visuals.

#include "CVoxelTerrain.h"
#include "CTimeProfiler.h"
#include "CWorldState.h"

// == Renderer Initialization ==
void CVoxelTerrain::InitializeRenderProperties ( void )
{
	//my2ndRenderer	= new CVoxelTerrainTranslucentRenderer ( this );
	bgRenderer = new BackgroundRenderer( this );

	// Load a texture
	myMat = new glMaterial;
	myMat->loadFromFile( "nature_terra" );
	/*myMat->loadTexture( ".res\\textures\\terraTexture.jpg" );
	myMat->useColors = true;
	//myMat->useLighting = false;
	myMat->setShader( new glShader( ".res\\shaders\\world\\terrainDefault.glsl" ) );*/
	//myMat->iFaceMode = glMaterial::FM_FRONTANDBACK;

	/*
	// Load a texture
	myWaterMat = new glMaterial;
	myWaterMat->loadTexture( ".res\\textures\\fluidTexture.jpg" );
	myWaterMat->useColors = true;
	myWaterMat->setShader( new glShader( ".res\\shaders\\world\\terraFluidDefault.glsl" ) );
	myWaterMat->isTransparent = true;
	myWaterMat->useDepthMask = false; // Play with this value. The water is generally rather opaque, yes?
	myWaterMat->iFaceMode = myMat->FM_FRONTANDBACK;	// Draw both the topside and backside of the water

	//vMaterials.push_back( myMat );
	//vMaterials.push_back( myWaterMat );
	
	myLODMat = new glMaterial;
	myLODMat->loadTexture( ".res\\textures\\null.jpg" );
	myLODMat->useColors = true;
	myLODMat->useDepthMask = false;
	myLODMat->setShader( new glShader( ".res\\shaders\\world\\terrainDistance.glsl" ) );

	bgRenderer->SetMaterial( myLODMat );

	// Set the material as our base material
	this->SetMaterial( myMat );
	vMaterials.push_back( myWaterMat );*/

}

CVoxelTerrain::BackgroundRenderer::BackgroundRenderer ( CVoxelTerrain* target )
	: CRenderableObject(), m_terra(target)
{
	renderType = Renderer::Secondary;
}
CVoxelTerrain::BackgroundRenderer::~BackgroundRenderer ( void )
{
	// mehs
}

unsigned char	CVoxelTerrain::BackgroundRenderer::GetPassNumber ( void )
{
	if ( m_terra->bLODCanDraw_L1 )
		return 1;
	return 0;
}
glMaterial*		CVoxelTerrain::BackgroundRenderer::GetPassMaterial ( const char pass )
{
	return m_terra->myLODMat;
}

unsigned char CVoxelTerrain::GetPassNumber ( void )
{
	//return 2; // Main pass and Water pass
	/*uchar passNum = 2; // Main pass and water pass
	if ( bLODCanDraw_L1 ) {
		passNum += 1;
	}
	return passNum;*/
	return 2;
}
glMaterial* CVoxelTerrain::GetPassMaterial ( const char pass )
{
	/*if ( pass == 0 ) // Solid pass
	{
		return myMat;
	}
	else // Water pass
	{
		return myWaterMat;
	}*/
	/*vector<glMaterial*> passMats;
	if ( bLODCanDraw_L1 ) {
		passMats.push_back( myLODMat );
	}
	passMats.push_back( myMat );
	passMats.push_back( myWaterMat );

	return passMats[pass];*/
	if ( pass == 0 ) // Solid pass
	{
		//return myMat;
	//	return vMaterials[0];
	}
	else // Water pass
	{
		//return myWaterMat;
	//	return vMaterials[1];
	}
	return NULL;
}

// Terrain cull
bool CVoxelTerrain::PreRender ( const char pass )
{
	// Reset drawn flag before first render in case someone else used it
	ResetBlockDrawn ( root );

	//if ( pass == GetPassNumber()-2 )
	if ( pass == 0 )
	{
		// == VISUALS ==
		// Perform frustum culling
		CullTree32( root, root_position );
		// Perform occlusion culling
		//OccludeTree32( root, root_position );
	}

	return true;
}

// Main Render
bool CVoxelTerrain::Render ( const char pass )
{
/*	//int diffusePass = GetPassNumber()-2;
	int diffusePass = 0;

	// If in shadow mode, only cast from the terrain, not the water
	if ( CCamera::activeCamera->shadowCamera )
		if ( pass != diffusePass )
			return true;

	glMaterial* drawMat = NULL;
	if ( pass == diffusePass )
	{

		//drawMat = vMaterials[0];
	}
	else if ( pass == diffusePass+1 )
	{

		drawMat = vMaterials[1];
	}

	if ( drawMat ) {
		drawMat->bind();
		drawMat->setShaderConstants( this );
		if ( ActiveGameWorld ) {
			drawMat->setUniform( "terra_BaseAmbient", ActiveGameWorld->cBaseAmbient );
		}
		else {
			drawMat->setUniform( "terra_BaseAmbient", Color(0,0,0,1) );
		}
	}
	else {
		return false;
	}

//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// Disable face culling in shadow mode
	if ( CCamera::activeCamera->shadowCamera )
		glDisable( GL_CULL_FACE );
	
	// do same as vertex array except pointer
	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

	if ( pass == diffusePass ) {
		RenderOcttree( root, root_position );
	}
	else if ( pass == diffusePass+1 ) {
		RenderWaterOcttree( root, root_position );
	}

		
	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	// bind with 0, so, switch back to normal pointer operation
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Re-enable culling
	if ( CCamera::activeCamera->shadowCamera )
		glEnable( GL_CULL_FACE );
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	return true;*/
	return false;
}

void CVoxelTerrain::RenderOcttree ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[1] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[1] = true;

			// If visible...
			if (( pBoob->bitinfo[0] == true )&&( pBoob->hasBlockData )&&( pBoob->pMesh->faceCount > 0 )
				&&(pBoob->pMesh->iVBOverts && pBoob->pMesh->iVBOfaces))
			{
				// Draw current boob
				glBindBuffer(GL_ARRAY_BUFFER, pBoob->pMesh->iVBOverts);         // for vertex coordinates
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBoob->pMesh->iVBOfaces); // for face vertex indexes

				// Tell where the vertex coordinates are in the array
				glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
				glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
				glTexCoordPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
				glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );

				// Draw the sutff
				glDrawElements( GL_QUADS, pBoob->pMesh->faceCount*4, GL_UNSIGNED_SHORT, 0 );
			}

			// Draw other boobs
			RenderOcttree( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			RenderOcttree( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			RenderOcttree( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			RenderOcttree( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			RenderOcttree( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			RenderOcttree( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
	}
}

void CVoxelTerrain::RenderWaterOcttree ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[1] == true )
		{
			// Set drawn flag
			pBoob->bitinfo[1] = false;

			// Draw other boobs (render back-to-front)
			RenderWaterOcttree( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			RenderWaterOcttree( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			RenderWaterOcttree( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			RenderWaterOcttree( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			RenderWaterOcttree( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			RenderWaterOcttree( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );

			// Draw the water back to center
			if (( pBoob->bitinfo[0] == true )&&( pBoob->hasBlockData )&&( pBoob->pMesh->faceCountWater > 0 )
				&&(pBoob->pMesh->iWaterVBOverts && pBoob->pMesh->iWaterVBOfaces))
			{
				// Draw current boob
				glBindBuffer(GL_ARRAY_BUFFER, pBoob->pMesh->iWaterVBOverts);         // for vertex coordinates
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBoob->pMesh->iWaterVBOfaces); // for face vertex indexes

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



bool CVoxelTerrain::BackgroundRenderer::Render ( const char pass )
{
	return false;
	/*if ( CCamera::activeCamera->shadowCamera )
		return false;
	if ( m_terra->bLODCanDraw_L1 )
	{
		vMaterials[0]->bind();
		vMaterials[0]->setShaderConstants( this );
		if ( ActiveGameWorld ) {
			vMaterials[0]->setUniform( "terra_BaseAmbient", ActiveGameWorld->cBaseAmbient );
		}
		else {
			vMaterials[0]->setUniform( "terra_BaseAmbient", Color(0,0,0,1) );
		}

		// do same as vertex array except pointer
		glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		m_terra->RenderLODL1();
			
		glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		// bind with 0, so, switch back to normal pointer operation
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return true;
	}
	return false;*/
}

void CVoxelTerrain::RenderLODL1 ( void )
{
	GL.Translate( Vector3d(
		(ftype)(lod_root_draw_position.x-16)*64,
		(ftype)(lod_root_draw_position.y-16)*64,
		(ftype)(lod_root_draw_position.z-16)*64 )
		);

	// Draw current boob
	glBindBuffer(GL_ARRAY_BUFFER, pLOD_Level1Mesh->iVBOverts);         // for vertex coordinates
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pLOD_Level1Mesh->iVBOfaces); // for face vertex indexes

	// Tell where the vertex coordinates are in the array
	glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
	glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
	glTexCoordPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
	glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );

	// Draw the stuff
	glDrawElements( GL_QUADS, pLOD_Level1Mesh->faceCount*4, GL_UNSIGNED_SHORT, 0 );
}
// == Secondary Renderer (Water+Translucents) ==
/*CVoxelTerrainTranslucentRenderer::CVoxelTerrainTranslucentRenderer ( CVoxelTerrain* newOwner )
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
	glBindBufferARB(GL_ARRAY_BUFFER, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	if ( CCamera::activeCamera->shadowCamera )
		glDisable( GL_CULL_FACE );
	
	// do same as vertex array except pointer
	glEnableClientState(GL_VERTEX_ARRAY);             // activate vertex coords array
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

	RenderOcttree( root, root_position );

	//glEnable ( GL_COLOR_MATERIAL ) ;

	glDisableClientState(GL_VERTEX_ARRAY);            // deactivate vertex array
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

	// bind with 0, so, switch back to normal pointer operation
	glBindBufferARB(GL_ARRAY_BUFFER, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);

	if ( CCamera::activeCamera->shadowCamera )
		glEnable( GL_CULL_FACE );

	//myMat.unbind();
	//myMat->unbind();

	//TimeProfiler.EndPrintTimeProfile( "terradraw" );

	return true;
}

void CVoxelTerrain::RenderOcttree ( CBoob * pBoob, RangeVector const& position )
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
				glBindBufferARB(GL_ARRAY_BUFFER, pBoob->pMesh->iVBOverts);         // for vertex coordinates
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, pBoob->pMesh->iVBOfaces); // for face vertex indexes

				// Tell where the vertex coordinates are in the array
				glVertexPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), 0 ); 
				glNormalPointer( GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
				glTexCoordPointer( 3, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
				glColorPointer( 4, GL_FLOAT, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );

				// Draw the sutff
				glDrawElements( GL_QUADS, pBoob->pMesh->faceCount*4, GL_UNSIGNED_SHORT, 0 );

			}

			// Draw other boobs
			RenderOcttree( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			RenderOcttree( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			RenderOcttree( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			RenderOcttree( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			RenderOcttree( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			RenderOcttree( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
	}
}

void CVoxelTerrain::RenderWaterOcttree ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[1] == false )
		{
			// Set drawn flag
			pBoob->bitinfo[1] = true;

			// Draw other boobs (render back-to-front)
			RenderWaterOcttree( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			RenderWaterOcttree( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			RenderWaterOcttree( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			RenderWaterOcttree( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			RenderWaterOcttree( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			RenderWaterOcttree( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );

			// Draw the water back to center
			//if ( pBoob->bitinfo[0] == true )
			if (( pBoob->bitinfo[0] == true )&&( pBoob->hasBlockData )&&( pBoob->pMesh->faceCountWater > 0 ))
			{
				// ===== DRAW THE WATER ======

				// Draw current boob
				glBindBufferARB(GL_ARRAY_BUFFER, pBoob->pMesh->iWaterVBOverts);         // for vertex coordinates
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, pBoob->pMesh->iWaterVBOfaces); // for face vertex indexes

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
*/

// Material getters
glMaterial* CVoxelTerrain::GetBlockMaterial ( void )
{
	return myMat;
}