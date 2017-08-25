
// Includes
#include "core/time.h"
#include "core/settings/CGameSettings.h"

#include "core-ext/profiler/CTimeProfiler.h"

#include "renderer/material/glMaterial.h"
#include "renderer/camera/CCamera.h"
#include "renderer/texture/CTexture.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "after/physics/water/Water.h"
#include "after/physics/wind/WindMotion.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/system/TerrainRenderer.h"
#include "after/terrain/system/DataSampler.h"

#include "after/types/terrain/GrassTypes.h"
#include "after/types/terrain/BlockTracker.h"
#include "after/types/terrain/BlockType.h"

#include "after/entities/item/weapon/CWeaponItemGenerator.h"

#include "CTerraGrass.h"

// Static members
CTerraGrass::sTerraGrassSorter CTerraGrass::mySorter;
glMaterial* CTerraGrass::grassMaterial = NULL;

// Constructor
CTerraGrass::CTerraGrass( std::vector<Terrain::grass_t> * pInVect )
	: pv_grasslist( pInVect ), /*pOwnerBoob( pInBoob ),*/ CRenderableObject()
{
	InitMaterials();
	SetMaterial( grassMaterial );

	// Init VBO values
	grassVerts = 0;
	grassFaces = 0;

	// Init mesh array values
	pVertices	= NULL;
	pQuads		= NULL;
	iVertices	= 0;
	iQuads		= 0;
	iMaxQuads	= 0;
	iGrassRes	= CGameSettings::Active()->i_ro_GrassSubdivisions;

	// Set position
	//transform.position = pOwnerBoob->position;
	if ( pv_grasslist == NULL ) {
		throw std::exception();
	}
}
// Destructor
CTerraGrass::~CTerraGrass ( void )
{
	FreeVBOData();

	if ( pVertices )
		delete [] pVertices;
	pVertices = NULL;
	if ( pQuads )
		delete [] pQuads;
	pQuads = NULL;
}

// Initialize materials
void CTerraGrass::InitMaterials ( void )
{
	if ( grassMaterial == NULL )
	{
		grassMaterial = new glMaterial ();
		grassMaterial->setTexture( 0, new CTexture(".res/textures/grassTexture.png") );

		grassMaterial->passinfo.push_back( glPass() );
		grassMaterial->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;	// 2-sided material to decrease bandwidth usage
		grassMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
		//grassMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		grassMaterial->passinfo[0].b_depthmask = false;

		grassMaterial->passinfo[0].shader = new glShader( ".res/shaders/world/grassDefault.glsl" );
	}
}
// Free VBO data
void CTerraGrass::FreeVBOData ( void )
{	GL_ACCESS
	GL.FreeBuffer( &grassVerts );
	GL.FreeBuffer( &grassFaces );
}

// Sorter, based on distance to camera
// Returns true if first argument should appear before second argument
bool CTerraGrass::sTerraGrassSorter::operator() ( Terrain::grass_t & left, Terrain::grass_t & right )
{
	// NOTE, IS CURRENTLY SORTED BACK-TO-FRONT (ALPHA NEEDS BACK-TO-FRONT)
	if ( (left.position - CCamera::activeCamera->transform.position/*-offset*/).sqrMagnitude() < (right.position - CCamera::activeCamera->transform.position/*-offset*/).sqrMagnitude() )
	{
		return false;
	}
	return true;
}

// Generates the mesh, sorted from back-to-front order
bool CTerraGrass::PreRender ( const char pass )
{
	static int iTickCounter = 0;

	// Do memory check just in case
	/*if ( pOwnerBoob->pGrass != this )
	{
		cout << "Bad ownership on grass! " << pOwnerBoob->pGrass << " != " << this << endl;
		visible = false;
		return false;
	}

	// Set position
	transform.position = pOwnerBoob->position;*/

	// Check for visibility
	//if ( (CCamera::activeCamera->transform.position - transform.position).sqrMagnitude() > sqr(96) )
	//if ( GetRenderDistance() > sqr(96) )
	if ( (CCamera::activeCamera->transform.position - (transform.position+Vector3d(32,32,32))).sqrMagnitude() > sqr(100) )
	{
		visible = false;
	}
	else
	{
		//visible = ( pOwnerBoob->bitinfo[0] && !pv_grasslist->empty() );
		visible = !pv_grasslist->empty();
	}
	// Visibility also depends on terrain visibility
	if ( CVoxelTerrain::GetActive() ) {
		visible = visible && CVoxelTerrain::GetActive()->Renderer()->GetVisible();
	}
	// Also do frustum culling
	if ( visible )
	{
		visible = CCamera::activeCamera->SphereIsVisible( transform.position+Vector3d(32,32,32), 56.0f );
	}


	// Regenerate the mesh only when visible and when tick counter hits limit
	if (( visible )&&( ++iTickCounter > 12 ))
	{
		// Reset tick counter
		iTickCounter = 0;

		// Check the VBO's, create if needed
		if (( grassVerts == 0 )||( grassFaces == 0 ))
		{	GL_ACCESS
			FreeVBOData();
			GL.BindVertexArray( 0 );

			GL.CreateBuffer( &grassVerts );
			GL.CreateBuffer( &grassFaces );

			GL.BindBuffer( GL_ARRAY_BUFFER, grassVerts );
			GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, grassFaces );

			GL.BindBuffer( GL_ARRAY_BUFFER, 0 );
			GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		// Take the grass list and sort it.
		mySorter.offset = transform.position;
		sort( pv_grasslist->begin(), pv_grasslist->end(), mySorter );

		// Allocate memory for the grass mesh
		if (( pVertices == NULL )||( pQuads == NULL )||( iMaxQuads < pv_grasslist->size()*3*iGrassRes ))
		{
			if ( pVertices )
				delete [] pVertices;
			if ( pQuads )
				delete [] pQuads;

			iMaxQuads = (uint)(pv_grasslist->size()*3*iGrassRes*2.3f);

			pVertices	= new CTerrainVertex [iMaxQuads*4];
			pQuads		= new CModelQuad [iMaxQuads];
		}

		// Start building the mesh
		iVertices	= 0;
		iQuads		= 0;

		bool errored = false;
		for ( auto it = pv_grasslist->begin(); it != pv_grasslist->end(); ++it )
		{
			// Check for proper index loaded
			/*if ( it->block.b16index >= 16 || it->block.b16index < 0 ) {
				if ( !errored ) {
#ifdef _ENGINE_DEBUG
					cout << "invalid grass" << endl;
#endif
					errored = true;
				}
				continue;
			}*/
			if ( iVertices >= iMaxQuads*4 )
			{
				std::cout << "grass mesh bounds error 1" << std::endl;
				break;
			}
			if ( iQuads >= iMaxQuads )
			{
				std::cout << "grass mesh bounds error 2" << std::endl;
				break;
			}

			switch ( it->type ) {
				case Terrain::EG_TALL:		genfHeight = 1.9f; break;
				case Terrain::EG_RED:		genfHeight = 0.7f; break;
				case Terrain::EG_FROSTLEAF:	genfHeight = 1.4f; break;
				default:	genfHeight = 0.9f;
			}

			//genfLight = it->block.pBlock->light * 0.14f;
			genfLight = 1;

			if ( iGrassRes == 1 ) {
				AddGrassPatch( it->position, it->type );
			}
			else {
				AddHighresGrassPatch( it->position, it->type );
			}
		}

		GL_ACCESS

		// Now, bind the buffers, and send the data
		GL.BindVertexArray( 0 );

		GL.BindBuffer( GL_ARRAY_BUFFER, grassVerts );
		GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, grassFaces );

		GL.UploadBuffer( GL_ARRAY_BUFFER, sizeof(CTerrainVertex)*iVertices, pVertices, GL_STREAM_DRAW );
		GL.UploadBuffer( GL_ELEMENT_ARRAY_BUFFER,   sizeof(CModelQuad)*iQuads, pQuads, GL_STREAM_DRAW );
		
		GL.BindBuffer( GL_ARRAY_BUFFER, 0 );
		GL.BindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}
	return true;
}

// Add a patch to the mesh
void CTerraGrass::AddGrassPatch ( const Vector3d & vNewPos, ushort type )
{
	static unsigned int i;
	static unsigned int f;
	i = iVertices;
	f = iQuads;

	Vector2d planeOffsets [6];
	Vector2d normalOffsets [3];
	Vector2d normalOffsetsR [3];
	for ( uint p = 0; p < 3; ++p ) {
		Matrix2x2 rotMatrix;
		rotMatrix.setRotation( p*120.0F );

		planeOffsets[p*2  ] = rotMatrix * Vector2d( 0.5f,0 );
		planeOffsets[p*2+1] = rotMatrix * Vector2d( -0.5f,0 );
		normalOffsets[p]	= rotMatrix * Vector2d( 0,0.12f );
		normalOffsetsR[p]	= rotMatrix * Vector2d( 0,0.1f );
	}
	

	pVertices[i  ].x = planeOffsets[0].x - normalOffsets[0].x;
	pVertices[i  ].y = planeOffsets[0].y - normalOffsets[0].y;
	pVertices[i  ].z = 0;
	pVertices[i+1].x = planeOffsets[0].x + normalOffsetsR[0].x;
	pVertices[i+1].y = planeOffsets[0].y + normalOffsetsR[0].y;
	pVertices[i+1].z = 1;
	pVertices[i+2].x = planeOffsets[1].x + normalOffsetsR[0].x;
	pVertices[i+2].y = planeOffsets[1].y + normalOffsetsR[0].y;
	pVertices[i+2].z = 1;
	pVertices[i+3].x = planeOffsets[1].x - normalOffsets[0].x;
	pVertices[i+3].y = planeOffsets[1].y - normalOffsets[0].y;
	pVertices[i+3].z = 0;

	pQuads[f].vert[0] = i;
	pQuads[f].vert[1] = i+1;
	pQuads[f].vert[2] = i+2;
	pQuads[f].vert[3] = i+3;

	i += 4;
	f += 1;

	pVertices[i  ].x = planeOffsets[2].x - normalOffsets[1].x;
	pVertices[i  ].y = planeOffsets[2].y - normalOffsets[1].y;
	pVertices[i  ].z = 0;
	pVertices[i+1].x = planeOffsets[2].x + normalOffsetsR[1].x;
	pVertices[i+1].y = planeOffsets[2].y + normalOffsetsR[1].y;
	pVertices[i+1].z = 1;
	pVertices[i+2].x = planeOffsets[3].x + normalOffsetsR[1].x;
	pVertices[i+2].y = planeOffsets[3].y + normalOffsetsR[1].y;
	pVertices[i+2].z = 1;
	pVertices[i+3].x = planeOffsets[3].x - normalOffsets[1].x;
	pVertices[i+3].y = planeOffsets[3].y - normalOffsets[1].y;
	pVertices[i+3].z = 0;

	pQuads[f].vert[0] = i;
	pQuads[f].vert[1] = i+1;
	pQuads[f].vert[2] = i+2;
	pQuads[f].vert[3] = i+3;

	i += 4;
	f += 1;

	pVertices[i  ].x = planeOffsets[4].x - normalOffsets[2].x;
	pVertices[i  ].y = planeOffsets[4].y - normalOffsets[2].y;
	pVertices[i  ].z = 0;
	pVertices[i+1].x = planeOffsets[4].x + normalOffsetsR[2].x;
	pVertices[i+1].y = planeOffsets[4].y + normalOffsetsR[2].y;
	pVertices[i+1].z = 1;
	pVertices[i+2].x = planeOffsets[5].x + normalOffsetsR[2].x;
	pVertices[i+2].y = planeOffsets[5].y + normalOffsetsR[2].y;
	pVertices[i+2].z = 1;
	pVertices[i+3].x = planeOffsets[5].x - normalOffsets[2].x;
	pVertices[i+3].y = planeOffsets[5].y - normalOffsets[2].y;
	pVertices[i+3].z = 0;

	pQuads[f].vert[0] = i;
	pQuads[f].vert[1] = i+1;
	pQuads[f].vert[2] = i+2;
	pQuads[f].vert[3] = i+3;

	i += 4;
	f += 1;

	ftype offset;
	switch ( type ) {
		case Terrain::EG_DRY:		offset = 0.25f;	break;
		case Terrain::EG_RED:		offset = 0.50f; break;
		case Terrain::EG_FROSTLEAF:	offset = 0.75f;	break;
		case Terrain::EG_DEFAULT:
		case Terrain::EG_TALL:
		default: offset = 0;
	}

	// Set colors and size
	for ( unsigned int v = 0; v < 12; ++v )
	{
		pVertices[iVertices+v].x = pVertices[iVertices+v].x*1.5f*(1+genfHeight) + vNewPos.x;
		pVertices[iVertices+v].y = pVertices[iVertices+v].y*1.5f*(1+genfHeight) + vNewPos.y;
		pVertices[iVertices+v].z = pVertices[iVertices+v].z*3*genfHeight + vNewPos.z;

		pVertices[iVertices+v].nx = 0;
		pVertices[iVertices+v].ny = 0;
		pVertices[iVertices+v].nz = 1;

		pVertices[iVertices+v].r = genfLight;
		pVertices[iVertices+v].g = genfLight;
		pVertices[iVertices+v].b = genfLight;
	}
	
	// Set UV's
	for ( unsigned int v = 0; v < 12; v += 4 )
	{
		pVertices[iVertices+v  ].u = offset+0.25f;
		pVertices[iVertices+v  ].v = 1;
		//pVertices[iVertices+v  ].nz = -1;
		pVertices[iVertices+v  ].a = 0;

		pVertices[iVertices+v+1].u = offset+0.25f;
		pVertices[iVertices+v+1].v = 0.15f;
		//pVertices[iVertices+v+1].nz = 1;
		pVertices[iVertices+v+1].a = genfLight;

		pVertices[iVertices+v+2].u = offset;
		pVertices[iVertices+v+2].v = 0.15f;
		//pVertices[iVertices+v+2].nz = 1;
		pVertices[iVertices+v+2].a = genfLight;

		pVertices[iVertices+v+3].u = offset;
		pVertices[iVertices+v+3].v = 1;
		//pVertices[iVertices+v+3].nz = -1;
		pVertices[iVertices+v+3].a = 0;
	}

	iVertices	= i;
	iQuads		= f;
}

void CTerraGrass::AddHighresGrassPatch ( const Vector3d & vNewPos, ushort type )
{
	static unsigned int i;
	static unsigned int f;
	static unsigned int k;
	i = iVertices;
	f = iQuads;

	for ( k = 0; k < iGrassRes; ++k )
	{
		pVertices[i  ].x = 0.5f;
		pVertices[i  ].y = 0.1f;
		pVertices[i  ].z = k/(ftype)iGrassRes;
		pVertices[i+1].x = 0.5f;
		pVertices[i+1].y = 0.1f;
		pVertices[i+1].z = (k+1)/(ftype)iGrassRes;
		pVertices[i+2].x = -0.5f;
		pVertices[i+2].y = 0.1f;
		pVertices[i+2].z = (k+1)/(ftype)iGrassRes;
		pVertices[i+3].x = -0.5f;
		pVertices[i+3].y = 0.1f;
		pVertices[i+3].z = k/(ftype)iGrassRes;

		pQuads[f].vert[0] = i;
		pQuads[f].vert[1] = i+1;
		pQuads[f].vert[2] = i+2;
		pQuads[f].vert[3] = i+3;

		i += 4;
		f += 1;
	}

	for ( k = 0; k < iGrassRes; ++k )
	{
		pVertices[i  ].x = -0.163f;
		pVertices[i  ].y = -0.483f;
		pVertices[i  ].z = k/(ftype)iGrassRes;
		pVertices[i+1].x = -0.163f;
		pVertices[i+1].y = -0.483f;
		pVertices[i+1].z = (k+1)/(ftype)iGrassRes;
		pVertices[i+2].x = 0.337f;
		pVertices[i+2].y = 0.383f;
		pVertices[i+2].z = (k+1)/(ftype)iGrassRes;
		pVertices[i+3].x = 0.337f;
		pVertices[i+3].y = 0.383f;
		pVertices[i+3].z = k/(ftype)iGrassRes;

		pQuads[f].vert[0] = i;
		pQuads[f].vert[1] = i+1;
		pQuads[f].vert[2] = i+2;
		pQuads[f].vert[3] = i+3;

		i += 4;
		f += 1;
	}

	for ( k = 0; k < iGrassRes; ++k )
	{
		pVertices[i  ].x = -0.337f;
		pVertices[i  ].y = 0.383f;
		pVertices[i  ].z = k/(ftype)iGrassRes;
		pVertices[i+1].x = -0.337f;
		pVertices[i+1].y = 0.383f;
		pVertices[i+1].z = (k+1)/(ftype)iGrassRes;
		pVertices[i+2].x = 0.163f;
		pVertices[i+2].y = -0.483f;
		pVertices[i+2].z = (k+1)/(ftype)iGrassRes;
		pVertices[i+3].x = 0.163f;
		pVertices[i+3].y = -0.483f;
		pVertices[i+3].z = k/(ftype)iGrassRes;

		pQuads[f].vert[0] = i;
		pQuads[f].vert[1] = i+1;
		pQuads[f].vert[2] = i+2;
		pQuads[f].vert[3] = i+3;

		i += 4;
		f += 1;
	}

	ftype offset;
	switch ( type ) {
		case Terrain::EG_DRY:		offset = 0.25f;	break;
		case Terrain::EG_RED:		offset = 0.50f; break;
		case Terrain::EG_FROSTLEAF:	offset = 0.75f;	break;
		case Terrain::EG_DEFAULT:
		case Terrain::EG_TALL:
		default: offset = 0;
	}

	// Set colors + size
	for ( unsigned int v = 0; v < 12*iGrassRes; ++v )
	{
		pVertices[iVertices+v].x = pVertices[iVertices+v].x*1.5f*(1+genfHeight) + vNewPos.x;
		pVertices[iVertices+v].y = pVertices[iVertices+v].y*1.5f*(1+genfHeight) + vNewPos.y;
		pVertices[iVertices+v].z = pVertices[iVertices+v].z*3*genfHeight + vNewPos.z;

		pVertices[iVertices+v].nx = 0;
		pVertices[iVertices+v].ny = 0;
		pVertices[iVertices+v].nz = 1;

		pVertices[iVertices+v].r = genfLight;
		pVertices[iVertices+v].g = genfLight;
		pVertices[iVertices+v].b = genfLight;
	}

	// Set UV's
	for ( unsigned int v = 0; v < 12*iGrassRes; v += 4*iGrassRes )
	{
		for ( k = 0; k < iGrassRes; ++k )
		{
			pVertices[iVertices+v+k*4  ].u = offset+0.25f;
			pVertices[iVertices+v+k*4  ].v = 1-(k/(ftype)iGrassRes)*0.85f;
			pVertices[iVertices+v+k*4  ].a = (k/(ftype)iGrassRes) * genfLight;

			pVertices[iVertices+v+k*4+1].u = offset+0.25f;
			pVertices[iVertices+v+k*4+1].v = 1-((k+1)/(ftype)iGrassRes)*0.85f;
			pVertices[iVertices+v+k*4+1].a = ((k+1)/(ftype)iGrassRes) * genfLight;

			pVertices[iVertices+v+k*4+2].u = offset;
			pVertices[iVertices+v+k*4+2].v = 1-((k+1)/(ftype)iGrassRes)*0.85f;
			pVertices[iVertices+v+k*4+2].a = ((k+1)/(ftype)iGrassRes) * genfLight;

			pVertices[iVertices+v+k*4+3].u = offset;
			pVertices[iVertices+v+k*4+3].v = 1-(k/(ftype)iGrassRes)*0.85f;
			pVertices[iVertices+v+k*4+3].a = (k/(ftype)iGrassRes) * genfLight;
		}
	}

	iVertices	= i;
	iQuads		= f;
}

// Sends the VBO and renders
bool CTerraGrass::Render ( const char pass )
{
	if ( iQuads <= 0 ) {
		return true;
	}
	GL_ACCESS GLd_ACCESS

	GL.Translate( transform.position );

	grassMaterial->gm_WindDirection = Vector4d(vWindDir);
	grassMaterial->bindPass(pass);
	grassMaterial->setShaderConstants( this );

	// Now set manual Z-buffer stuff
	//glDepthMask( GL_TRUE );
	/*glDepthMask( GL_TRUE );
	glAlphaFunc(GL_GREATER, 0.5);
	glEnable(GL_ALPHA_TEST);*/

	if ( BindVAO( pass, grassVerts, grassFaces, true ) )
	{
		grassMaterial->bindAttribute( "mdl_Vertex",		3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*0) );
		grassMaterial->bindAttribute( "mdl_TexCoord",	3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*3) );
		grassMaterial->bindAttribute( "mdl_Color",		4, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*12) );
		grassMaterial->bindAttribute( "mdl_Normal",		3, GL_FLOAT, false, sizeof(CTerrainVertex), ((char*)0) + (sizeof(float)*6) );
	}

	// Draw the stuff
	GL.DrawElements( GL_QUADS, iQuads*4, GL_UNSIGNED_SHORT, 0 );

#ifndef _ENGINE_RELEASE
	//std::cout << (void*)pv_grasslist << " " << pv_grasslist->size() << " " << iQuads << std::endl;
	GLd.vertexCount += iQuads*8;
	GLd.triangleCount += iQuads*2;
#endif

	return true;
}

void CTerraGrass::Simulate ( void )
{
	// Only simulate if visible.
	if ( visible )
	{
		// Loop through the grass
		for ( auto it = pv_grasslist->begin(); it != pv_grasslist->end(); )
		{
			// First check for unassigned blocks
			//if ( it->block.b16index >= 16 || it->block.b16index < 0 )
			//if ( it->block.x_index >= 32 || it->block.x_index < 0 )
			if ( it->block.block == NULL )
			{
				// Grab the block index.
				BlockTrackInfo targetBlock;
				targetBlock.valid = false;
				bool result = false;
				if ( CVoxelTerrain::GetActive() ) {
					CVoxelTerrain::GetActive()->Sampler->BlockTrackerAt( transform.position + it->position - Vector3d( 0,0,1.0f ), targetBlock );
					result = targetBlock.valid;
				}

				// Assign the block info to the grass
				if ( result )
				{
					it->block.block = targetBlock.pBlock;
					it->block.x_index = targetBlock.pos_x;
					it->block.y_index = targetBlock.pos_y;
					it->block.z_index = targetBlock.pos_z;
				}
			}

			if ( it->block.block )
			{
				// Next, check if the grass block is invalid
				if ( it->type == 0 )
				{
					// Check for no block under
					if ( it->block.block->block == Terrain::EB_NONE )
					{
						// Need to remove this grass if so
						it = pv_grasslist->erase( it );
						continue;
					}
				}
			}

			// Go to next grass
			++it;
		}

		// Now, get the wind motion
		Vector3d windDir = WindMotion.GetFlowField( transform.position );
		vWindDir += (Vector2d( windDir.x, windDir.y ) - vWindDir)*CTime::TrainerFactor( 0.1f );
	}
}

// Breaks a grass on the target block
void CTerraGrass::BreakGrass ( Terrain::terra_b* target )
{
	for ( auto it = pv_grasslist->begin(); it != pv_grasslist->end(); ++it )
	{
		// Destroy grass if it matches
		if ( it->block.block == target ) {
			// Chance to drop items
			if ( ItemGenerator.ChanceGrass( it->type ) ) {
				ItemGenerator.DropGrass( it->position, it->type );
			}
			// Remove item from list
			pv_grasslist->erase( it );
		}
	}
}
// Check if target block is in grass list
bool CTerraGrass::HasGrass ( Terrain::terra_b* target )
{
	for ( auto it = pv_grasslist->begin(); it != pv_grasslist->end(); ++it )
	{
		// Destroy grass if it matches
		if ( it->block.block == target ) {
			return true;
		}
	}
	return false;
}