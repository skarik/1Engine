
#include "CTattooRenderer.h"

#include "engine/physics/raycast/Raycaster.h"

#include "renderer/logic/model/CSkinnedModel.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/debug/CDebugRTInspector.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "after/entities/character/CMccCharacterModel.h"
#include "after/states/player/CPlayerStats.h"

CTattooRenderer::CTattooRenderer ( CMccCharacterModel* nModel )
	: model( nModel )
{
	rt_tats_body = new CRenderTexture( RGBA8, 1024,512, Repeat, Repeat );
	rt_tats_face = new CRenderTexture( RGBA8, 1024,512, Repeat, Repeat );
}

CTattooRenderer::~CTattooRenderer ( void )
{
	delete rt_tats_body;
	rt_tats_body = NULL;
	delete rt_tats_face;
	rt_tats_face = NULL;
}

void CTattooRenderer::UpdateTattoos ( void )
{
	// Prepare to render to texture (directly!)
	GL_ACCESS GLd_ACCESS

	// Prepare skin color
	Color skinColor = Color( 0.6f,0.35f,0.4f,1.0f );
	{
		glMaterial* skinMat;
		if ( model->mdlBody ) {
			skinMat = model->mdlBody->FindMaterial( "skin" );
			if ( skinMat ) {
				skinColor = skinMat->m_diffuse;
			}
		}
	}
	// Prepare materials
	glMaterial tempMat;
	{
		tempMat.passinfo.push_back( glPass() );
		tempMat.passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		tempMat.passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		tempMat.passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
		tempMat.passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
		tempMat.passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	}
	tempMat.removeReference();

	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	GLd.P_PushColor( Color(1,1,1,1) );
	{	// Bind buffer
		rt_tats_body->BindBuffer();
		{	// Set up the rendering thing
			//GL.setupViewport( 0,0,256,256 ); // DirectX
			GL.setupViewport( 0,0,1024,512 ); // OpenGL
		}
		GL.beginOrtho( 0,0, 1024,512, -45,45, false );
		// Clear render target.
		GL.clearColor( Color( skinColor.red*0.5f, skinColor.green*0.5f, skinColor.blue*0.5f, 0.0f ) );
		GL.ClearBuffer( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
		// Draw tattoo list
		for ( uint i = 0; i < model->mstats->tattooList.size(); ++i )
		{
			CTexture* tempTexture = new CTexture( ".res/textures/tats/"+string(model->mstats->tattooList[i].pattern.c_str())+".png" );
			tempMat.setTexture( 0, tempTexture );
			tempMat.m_diffuse = model->mstats->tattooList[i].color;
			tempMat.bindPassForward(0);
				DrawTattooToTarget( true, model->mdlBody->GetModelDataByName("polyBody"), model->mstats->tattooList[i] );
			delete tempTexture;
		}
		// Draw racial markings
		if ( model->mstats->iRace == CRACE_FLUXXOR )
		{
			arstring<256> tGlowname;
			sprintf( tGlowname, ".res/textures/tats/body_flux_glow%d.png", model->mstats->iMiscType%10 );
			CTexture* tempTexture = new CTexture( tGlowname.c_str() );
			tempMat.setTexture( 0, tempTexture );
			tempMat.m_diffuse = Color(0.55,0.55,0.55)+(skinColor*0.5);
			tempMat.m_diffuse.alpha = 1;
			tempMat.bindPassForward(0);
				GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushTexcoord( 0,0 );
				GLd.P_AddVertex( 0*1024, 0*512 );
				GLd.P_PushTexcoord( 0,1  );
				GLd.P_AddVertex( 0*1024, 1*512 );
				GLd.P_PushTexcoord( 2,0  );
				GLd.P_AddVertex( 1*1024, 0*512 );
				GLd.P_PushTexcoord( 2,1  );
				GLd.P_AddVertex( 1*1024, 1*512 );
				GLd.EndPrimitive();
			delete tempTexture;
		}
		// Draw cuts
		for ( uint i = 0; i < model->tats_cuts.size(); ++i )
		{
			CTexture* tempTexture = new CTexture( ".res/textures/tats2/"+string(model->tats_cuts[i].pattern.c_str())+".png" );
			tempMat.setTexture( 0, tempTexture );
			tempMat.m_diffuse = model->tats_cuts[i].color;
			tempMat.m_diffuse.alpha = 1;
			tempMat.bindPassForward(0);
				DrawTattooToTarget( true, model->mdlBody->GetModelDataByName("polyBody"), model->tats_cuts[i] );
			delete tempTexture;
		}
		// Clear viewport
		GL.endOrtho();
		// Unbind buffer
		rt_tats_body->UnbindBuffer();
	}
	{	// Bind buffer
		rt_tats_face->BindBuffer();
		{	// Set up the rendering thing
			//GL.setupViewport( 0,0,256,256 ); // DirectX
			GL.setupViewport( 0,0,1024,512 ); // OpenGL
		}
		GL.beginOrtho( 0,0, 1024,512, -45,45, false );
		// Clear render target.
		GL.clearColor( Color( skinColor.red*0.5f, skinColor.green*0.5f, skinColor.blue*0.5f, 0.0f ) );
		GL.ClearBuffer( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
		// Draw some nose blush (kekek)
		if ( model->mstats->iRace == CRACE_HUMAN || model->mstats->iRace == CRACE_ELF )
		{
			arstring<256> tMapname;
			CTexture* tempTexture;

			// Generate a blush color
			{
				Color targetBlush;
				Color(0.93,0.31,0.32).GetHSLC( targetBlush );
				Color hslskinColor;
				skinColor.GetHSLC( hslskinColor );
				if ( targetBlush.red - hslskinColor.red > 180 ) {
					targetBlush.red -= 360;
				}
				else if ( hslskinColor.red - targetBlush.red > 180 ) {
					targetBlush.red += 360;
				}
				targetBlush.red = targetBlush.red*0.2f + hslskinColor.red*0.8f;
				targetBlush.green = targetBlush.green*0.4f + hslskinColor.green*0.6f;
				targetBlush.blue = targetBlush.blue*0.6f + hslskinColor.blue*0.4f;
				Color actualBlush;
				actualBlush.SetHSL( targetBlush );
				tempMat.m_diffuse = skinColor.Lerp( actualBlush,0.5f );
			}
			tempMat.m_diffuse = tempMat.m_diffuse * 0.5f;
			tempMat.m_diffuse.alpha = 1.0f;

			/*tMapname = ".res/textures/tats3/cl1_head_nose0.png";
			tempTexture = new CTexture( tMapname.c_str() );
			tempMat.setTexture( 0, tempTexture );
			tempMat.bindPassForward(0);
				GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushTexcoord( 0,0 );
				GLd.P_AddVertex( 0*1024, 0*512 );
				GLd.P_PushTexcoord( 0,1  );
				GLd.P_AddVertex( 0*1024, 1*512 );
				GLd.P_PushTexcoord( 1,0  );
				GLd.P_AddVertex( 1*1024, 0*512 );
				GLd.P_PushTexcoord( 1,1  );
				GLd.P_AddVertex( 1*1024, 1*512 );
				GLd.EndPrimitive();
			delete tempTexture;*/

			/*tMapname = ".res/textures/tats3/cl1_head_cheeks0.png";
			tempTexture = new CTexture( tMapname.c_str() );
			tempMat.setTexture( 0, tempTexture );
			tempMat.bindPassForward(0);
				GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushTexcoord( 0,0 );
				GLd.P_AddVertex( 0*1024, 0*512 );
				GLd.P_PushTexcoord( 0,1  );
				GLd.P_AddVertex( 0*1024, 1*512 );
				GLd.P_PushTexcoord( 1,0  );
				GLd.P_AddVertex( 1*1024, 0*512 );
				GLd.P_PushTexcoord( 1,1  );
				GLd.P_AddVertex( 1*1024, 1*512 );
				GLd.EndPrimitive();
			delete tempTexture;*/

			// Draw wierd makeup
			/*tempMat.m_diffuse = Color(0.11,0.04,0.32) * 0.5;
			tempMat.m_diffuse.alpha = 0.8f;
			tMapname = ".res/textures/tats3/cl1_head_eyeshadow_0.png";
			tempTexture = new CTexture( tMapname.c_str() );
			tempMat.setTexture( 0, tempTexture );
			tempMat.bindPassForward(0);
				GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushTexcoord( 0,0 );
				GLd.P_AddVertex( 0*1024, 0*512 );
				GLd.P_PushTexcoord( 0,1  );
				GLd.P_AddVertex( 0*1024, 1*512 );
				GLd.P_PushTexcoord( 1,0  );
				GLd.P_AddVertex( 1*1024, 0*512 );
				GLd.P_PushTexcoord( 1,1  );
				GLd.P_AddVertex( 1*1024, 1*512 );
				GLd.EndPrimitive();
			delete tempTexture;
			tMapname = ".res/textures/tats3/cl1_head_lips0.png";
			tempTexture = new CTexture( tMapname.c_str() );
			tempMat.setTexture( 0, tempTexture );
			tempMat.bindPassForward(0);
				GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushTexcoord( 0,0 );
				GLd.P_AddVertex( 0*1024, 0*512 );
				GLd.P_PushTexcoord( 0,1  );
				GLd.P_AddVertex( 0*1024, 1*512 );
				GLd.P_PushTexcoord( 1,0  );
				GLd.P_AddVertex( 1*1024, 0*512 );
				GLd.P_PushTexcoord( 1,1  );
				GLd.P_AddVertex( 1*1024, 1*512 );
				GLd.EndPrimitive();
			delete tempTexture;*/

		}
		// Draw tattoo list
		for ( uint i = 0; i < model->mstats->tattooList.size(); ++i )
		{
			CTexture* tempTexture = new CTexture( ".res/textures/tats/"+string(model->mstats->tattooList[i].pattern.c_str())+".png" );
			tempMat.setTexture( 0, tempTexture );
			tempMat.m_diffuse = model->mstats->tattooList[i].color;
			tempMat.bindPassForward(0);
				DrawTattooToTarget( false, model->mdlHead->GetModelDataByName("polyHead"), model->mstats->tattooList[i] ); // Save mesh names. Match mesh names.
			delete tempTexture;
		}
		// Draw racial markings
		if ( model->mstats->iRace == CRACE_FLUXXOR )
		{
			arstring<256> tGlowname;
			sprintf( tGlowname, ".res/textures/tats/head_fluxx_glow%d.png", model->mstats->iMiscType%10 );
			CTexture* tempTexture = new CTexture( tGlowname.c_str() );
			tempMat.setTexture( 0, tempTexture );
			tempMat.m_diffuse = Color(0.55,0.55,0.55)+(skinColor*0.5);
			tempMat.m_diffuse.alpha = 1;
			tempMat.bindPassForward(0);
				GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushTexcoord( 0,0 );
				GLd.P_AddVertex( 0*1024, 0*512 );
				GLd.P_PushTexcoord( 0,1  );
				GLd.P_AddVertex( 0*1024, 1*512 );
				GLd.P_PushTexcoord( 1,0  );
				GLd.P_AddVertex( 1*1024, 0*512 );
				GLd.P_PushTexcoord( 1,1  );
				GLd.P_AddVertex( 1*1024, 1*512 );
				GLd.EndPrimitive();
			delete tempTexture;
		}
		// Clear viewport
		GL.endOrtho();
		// Unbind buffer
		rt_tats_face->UnbindBuffer();
	}

	// Update skin mat
	glMaterial* skinMat;
	if ( model->mdlBody ) {
		skinMat = model->mdlBody->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->setTexture( 3, rt_tats_body );
			//skinMat->setUniform( "gm_HalfScale", 0.5f );
		}
		//model->mdlBody->SetShaderUniform( "gm_HalfScale", 0.5f );
		skinMat->gm_HalfScale = 0.5f;
	}
	if ( model->mdlHead ) {
		skinMat = model->mdlHead->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->setTexture( 3, rt_tats_face );
		}
		//model->mdlHead->SetShaderUniform( "gm_HalfScale", 1.0f );
		skinMat->gm_HalfScale = 1.0f;
	}


	//Debug::RTInspector->AddWatch( rt_tats_body );
	//Debug::RTInspector->AddWatch( rt_tats_face );
	/*if ( model->mdlHead ) {
		skinMat = model->mdlHead->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->pTextures[3] = texTattooTarget;
		}
	}
	if ( model->mdlEars ) {
		skinMat = model->mdlEars->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->pTextures[3] = texTattooTarget;
		}
	}*/
}


void CTattooRenderer::DrawTattooToTarget ( const bool isBody, CModelData* raw_model, CRacialStats::tattoo_t& tattoo )
{
	GL_ACCESS GLd_ACCESS
	//const ftype xscale = 0.5f;
	ftype xscale = 1.0f;
	if ( isBody ) {
		xscale = 0.5f;
	}
	ftype yscale = (ftype)glMaterial::current->getTexture(0)->GetHeight()/(ftype)glMaterial::current->getTexture(0)->GetWidth();

	Vector3d hitPos, hitCoords;
	Ray sourceRay;
	//CModelData* raw_model = model->mdlBody->GetModelData(0);
	int resultTri;

	//Rotator projection_rot = Rotator( tattoo.projection_angle,0,0 ) * Rotator( Quaternion::CreateRotationTo( Vector3d(1,0,0), tattoo.projection_dir ) );
	Rotator projection_rot = Rotator( Quaternion::CreateRotationTo( Vector3d(1,0,0), tattoo.projection_dir ) ) * Rotator( tattoo.projection_angle,0,0 );
	projection_rot = projection_rot.transpose();

	Matrix4x4 projection_matx;
	Matrix4x4 projection_matx_rot;
	//projection_matx.setTranslation( -tattoo.projection_pos );
	//projection_matx_rot.setRotation( !projection_rot );
	//projection_matx = projection_matx * projection_matx_rot;

	// Secondary technique: perform projection
	GLd.BeginPrimitive( GL_TRIANGLES );
	uint triCount = 0;
	for ( uint tri = 0; tri < raw_model->triangleNum; ++tri )
	{
		Vector3d uvCoords [3];
		uint failCount = 0;
		// Put each vertex into projection space
		for ( uint vert = 0; vert < 3; ++vert )
		{
			CModelVertex& vertex = raw_model->vertices[raw_model->triangles[tri].vert[vert]];

			// Convert the model position to projection space
			Vector3d vertexDifference = Vector3d( vertex.x, vertex.y, vertex.z )-tattoo.projection_pos;
			std::swap( vertexDifference.y, vertexDifference.z );
			uvCoords[vert] = projection_rot * vertexDifference;
			Vector3d normalDir = projection_rot * Vector3d( vertex.nx, vertex.nz, vertex.ny );

			uvCoords[vert].x /= tattoo.projection_scale.x*0.55f;
			uvCoords[vert].y /= tattoo.projection_scale.y*0.55f;

			// Check if vertex is out of bounds
			if ( normalDir.z > 0 || fabs(uvCoords[vert].x) > 0.5f || fabs(uvCoords[vert].y) > 0.5f ) {
				failCount += 1;
			}

			uvCoords[vert].x += 0.5f;
			uvCoords[vert].y += 0.5f;
		}

		// If one vert is in range, draw a triangle
		if ( failCount < 3 )
		{
			for ( int vert = 0; vert < 3; ++vert )
			{
				CModelVertex& vertex = raw_model->vertices[raw_model->triangles[tri].vert[vert]];
				GLd.P_PushTexcoord( uvCoords[vert] );
				GLd.P_AddVertex( (isBody)? vertex.u*512 : vertex.u*1024, (1+vertex.v)*512 );
				triCount += 1;
			}
			if ( tattoo.mirror )
			{
				if ( isBody )
				{
					for ( int vert = 0; vert < 3; ++vert )
					{
						CModelVertex& vertex = raw_model->vertices[raw_model->triangles[tri].vert[vert]];
						GLd.P_PushTexcoord( uvCoords[vert] );
						GLd.P_AddVertex( (vertex.u>1)? vertex.u*512 - 512 : vertex.u*512 + 512, (1+vertex.v)*512 );
						triCount += 1;
					}
				}
				else
				{
					for ( int vert = 0; vert < 3; ++vert )
					{
						CModelVertex& vertex = raw_model->vertices[raw_model->triangles[tri].vert[vert]];
						GLd.P_PushTexcoord( uvCoords[vert] );
						GLd.P_AddVertex( 1024-vertex.u*1024, (1+vertex.v)*512 );
						triCount += 1;
					}
				}
			}
		}

		// Perform limit break check
		if ( triCount >= 2048/3 - 4 ) {
			GLd.EndPrimitive();
			GLd.BeginPrimitive( GL_TRIANGLES );
			triCount = 0;
		}

		// Next triangle...
	}
	GLd.EndPrimitive();

	// Need to draw to object as a series of rectangles
	/*Vector2d uvCoords [64];
	const int subdivX = 8;
	const int subdivY = 8;
	for ( int ix = -subdivX/2; ix < subdivX/2; ++ix )
	{
		for ( int iy = -subdivY/2; iy < subdivY/2; ++iy )
		{
			sourceRay.pos = tattoo.projection_pos +
				(projection_rot * Vector3d( 0,ix/(ftype)subdivX * tattoo.projection_scale.x,iy/(ftype)subdivY * tattoo.projection_scale.y * yscale) );
			sourceRay.dir = tattoo.projection_dir;
			
			resultTri = Raytracer.RaycastExpensiveMiss( sourceRay, 20.0f, raw_model, hitPos, hitCoords );
			if ( resultTri >= 0 )
			{
				// detect texture coords
				Vector2d finalCoords;
				finalCoords.x =
					raw_model->vertices[raw_model->triangles[resultTri].vert[0]].u * hitCoords.x +
					raw_model->vertices[raw_model->triangles[resultTri].vert[1]].u * hitCoords.y +
					raw_model->vertices[raw_model->triangles[resultTri].vert[2]].u * hitCoords.z;
				finalCoords.x *= xscale;
				finalCoords.y =
					raw_model->vertices[raw_model->triangles[resultTri].vert[0]].v * hitCoords.x +
					raw_model->vertices[raw_model->triangles[resultTri].vert[1]].v * hitCoords.y +
					raw_model->vertices[raw_model->triangles[resultTri].vert[2]].v * hitCoords.z;
				//if ( isBody && (finalCoords.y < 0) ) {
					//cout << finalCoords << endl;
					finalCoords.y += 1;
				//}

				//GLd.DrawRectangle( finalCoords.x*512 -16,finalCoords.y*512 -16, 32,32 );
				uvCoords[(ix+subdivX/2)+(iy+subdivY/2)*subdivX] = finalCoords;
			}
			else
			{
				uvCoords[(ix+subdivX/2)+(iy+subdivY/2)*subdivX] = Vector2d(-1,-1);
			}
		}
	}

	// Now, loop through all coords and draw a prim with them
	Vector2d corners0 [4];
	Vector2d corners1 [4];
	Vector2d corners2 [4];
	Vector2d corners3 [4];
	Vector2d* corners [4];
	Vector2d uvcorners [4];
	corners[0] = corners0;
	corners[1] = corners1;
	corners[2] = corners2;
	corners[3] = corners3;
	ftype max_dist = tattoo.projection_scale.magnitude() * 0.25f * ( 2.0f/ftype(subdivX+subdivY) );

	for ( int ix = 0; ix < subdivX-1; ++ix )
	{
		for ( int iy = 0; iy < subdivY-1; ++iy )
		{
			// Look at first corner. Compute corner points.
			uvcorners[0] = Vector2d( ix/(ftype)subdivX,iy/(ftype)subdivY );
			uvcorners[1] = Vector2d( ix/(ftype)subdivX,(iy+1)/(ftype)subdivY );
			uvcorners[2] = Vector2d( (ix+1)/(ftype)subdivX,iy/(ftype)subdivY );
			uvcorners[3] = Vector2d( (ix+1)/(ftype)subdivX,(iy+1)/(ftype)subdivY );
			corners0[0] = Vector2d( uvCoords[ix+iy*subdivX] );
			corners0[1] = Vector2d( uvCoords[ix+(iy+1)*subdivX] );
			corners0[2] = Vector2d( uvCoords[(ix+1)+iy*subdivX] );
			corners0[3] = Vector2d( uvCoords[(ix+1)+(iy+1)*subdivX] );
			memcpy( corners1, corners0, sizeof(Vector2d)*4 );
			memcpy( corners2, corners0, sizeof(Vector2d)*4 );
			memcpy( corners3, corners0, sizeof(Vector2d)*4 );

			bool invalidQuad, drawQuad;
			int i, c;//, k;
			bool corners_drawn [4] = { false,false,false,false };

			// Loop through all the corners
			for ( c = 0; c < 4; ++c )
			{
				if ( c != 0 ) continue;
				
				// In relation to first corner, check for valid corner points.
				invalidQuad = false;
				drawQuad = true;
				// Check distance
				for ( i = 0; i < 4; ++i ) {
					if ( i == c ) continue;
					if ( (corners[c][i] - corners[c][c]).sqrMagnitude() > sqr( 0.34f ) ) {
						invalidQuad = true;
						drawQuad = false;
					}
				}

				// Draw the quad
				if ( drawQuad )
				{
					GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
					GLd.P_PushTexcoord( uvcorners[0].x, uvcorners[0].y );
					GLd.P_AddVertex( corners[c][0].x*1024, corners[c][0].y*512 );

					GLd.P_PushTexcoord( uvcorners[1].x, uvcorners[1].y  );
					GLd.P_AddVertex( corners[c][1].x*1024, corners[c][1].y*512 );

					GLd.P_PushTexcoord( uvcorners[2].x, uvcorners[2].y  );
					GLd.P_AddVertex( corners[c][2].x*1024, corners[c][2].y*512 );

					GLd.P_PushTexcoord( uvcorners[3].x, uvcorners[3].y  );
					GLd.P_AddVertex( corners[c][3].x*1024, corners[c][3].y*512 );
					GLd.EndPrimitive();

					// Draw the mirrored compy
					if ( tattoo.mirror )
					{
						GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
						if ( isBody )
						{
							GLd.P_PushTexcoord( uvcorners[0].x, uvcorners[0].y );
							GLd.P_AddVertex( (corners[c][0].x>0.5f)? corners[c][0].x*1024 - 512 : corners[c][0].x*1024 + 512, corners[c][0].y*512 );

							GLd.P_PushTexcoord( uvcorners[1].x, uvcorners[1].y  );
							GLd.P_AddVertex( (corners[c][0].x>0.5f)? corners[c][1].x*1024 - 512 : corners[c][1].x*1024 + 512, corners[c][1].y*512 );

							GLd.P_PushTexcoord( uvcorners[2].x, uvcorners[2].y  );
							GLd.P_AddVertex( (corners[c][0].x>0.5f)? corners[c][2].x*1024 - 512 : corners[c][2].x*1024 + 512, corners[c][2].y*512 );

							GLd.P_PushTexcoord( uvcorners[3].x, uvcorners[3].y  );
							GLd.P_AddVertex( (corners[c][0].x>0.5f)? corners[c][3].x*1024 - 512 : corners[c][3].x*1024 + 512, corners[c][3].y*512 );
						}
						else
						{
							GLd.P_PushTexcoord( uvcorners[3].x, uvcorners[3].y  );
							GLd.P_AddVertex( 1024-corners[c][3].x*1024, corners[c][3].y*512 );

							GLd.P_PushTexcoord( uvcorners[2].x, uvcorners[2].y  );
							GLd.P_AddVertex( 1024-corners[c][2].x*1024, corners[c][2].y*512 );

							GLd.P_PushTexcoord( uvcorners[1].x, uvcorners[1].y  );
							GLd.P_AddVertex( 1024-corners[c][1].x*1024, corners[c][1].y*512 );

							GLd.P_PushTexcoord( uvcorners[0].x, uvcorners[0].y );
							GLd.P_AddVertex( 1024-corners[c][0].x*1024, corners[c][0].y*512 );
						}
						GLd.EndPrimitive();
					}

					corners_drawn[c] = true; // Flag this corner set as drawn
				}
			}
		}
	}*/
}