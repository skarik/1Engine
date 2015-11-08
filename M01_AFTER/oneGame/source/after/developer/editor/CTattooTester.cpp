
#include "CTattooTester.h"
#include "renderer/logic/model/CSkinnedModel.h"

#include "after/entities/character/CMccCharacterModel.h"
#include "after/states/player/CPlayerStats.h"
#include "core/settings/CGameSettings.h"

#include "renderer/debug/CDebugDrawer.h"
#include "renderer/debug/CDebugRTInspector.h"

#include "renderer/camera/CCamera.h"
#include "engine-common/entities/CPlayer.h"
#include "engine/state/CGameState.h"

#include "engine/physics/Raycast/Raycaster.h"

#include "renderer/texture/CTexture.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/material/glMaterial.h"

//#include "StringUtils.h"
#include "core/utils/StringUtils.h"
#include <fstream>

#include "core/debug/console.h"

CTattooTester::CTattooTester ( void ) : CGameBehavior()
{
	CreateGUI();
	model = NULL;

	texTattoo = new CTexture( ".res/textures/tats/laak.png" );
	texTattooTarget = new CRenderTexture( RGBA8, 1024,512, Repeat, Repeat );

	//tattoo0.valid = false;
	pl_stats = NULL;
}

CTattooTester::~CTattooTester ( void )
{
	pl_stats->SaveToFile();

	delete_safe( gui );

	/*delete_safe( mdlSrc );
	delete_safe( mdlBody );
	delete_safe( mdlHead );*/
	delete_safe( model );
	delete_safe( pl_stats );

	delete_safe( texTattoo );
	delete_safe( texTattooTarget );
}

void CTattooTester::Update ( void )
{
	if ( model == NULL ) {
		LoadModels();
	}
	DoGUIWork();
	pl_stats->UpdateStats();
	UpdateModels();
	if ( tCharMarks.need_redraw_tattoos ) {
		//UpdateTarget();
		((CMccCharacterModel*)(model))->UpdateTattoos();
	}
	RaycastModel();
}

void CTattooTester::LoadModels ( void )
{
	CGameSettings::Active()->SetPlayerSaveFile( "DarkElf" );

	pl_stats = new CPlayerStats();
	pl_stats->LoadFromFile();
	model = new CMccCharacterModel ( NULL );
	model->LoadBase( "clara" );
	model->SetVisualsFromStats( pl_stats->race_stats );

	uvCursor = Vector2d(0,0);
}
void CTattooTester::UpdateModels ( void )
{
	/*mdlSrc->transform.position = Vector3d(0,0,0);
	mdlSrc->visible = false;
	mdlSrc->UpdateSkeleton();

	mdlBody->transform.position = Vector3d(0,0,0);
	mdlHead->transform.position = Vector3d(0,0,0);

	mdlSrc->GetAnimation()->Play( "idle" );

	// Set uniforms for child models
	uchar passNum = mdlHead->GetPassNumber();
	for ( uchar pass = 0; pass < passNum; ++pass ) {
		mdlHead->SetShaderUniform( "gm_FadeValue", 1 );
	}
	passNum = mdlBody->GetPassNumber();
	for ( uchar pass = 0; pass < passNum; ++pass ) {
		mdlBody->SetShaderUniform( "gm_FadeValue", 1 );
	}*/

	//model->SetMoveAnimation( "ref" );
	model->SetMoveAnimation( "idle_02" );
	
}

void CTattooTester::UpdateTarget ( void )
{	GL_ACCESS GLd_ACCESS
	// Prepare to render to texture (directly!)

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

	{	// Bind buffer
		texTattooTarget->BindBuffer();
	}

	{	// Set up the rendering thing
		//GL.setupViewport( 0,0,256,256 ); // DirectX
		GL.setupViewport( 0,0,512,512 ); // OpenGL
	}
	GL.beginOrtho( 0,0, 512,512, -45,45, false );

	//glClearColor( 0.5f,0.5f,0.5f,0.0f );
	GL.clearColor( Color( skinColor.red*0.5f, skinColor.green*0.5f, skinColor.blue*0.5f, 0.0f ) );
	GL.ClearBuffer( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

	glMaterial tempMat;
	//tempMat.useTexture = false;
	tempMat.passinfo.push_back( glPass() );
	tempMat.passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	tempMat.passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	tempMat.passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
	tempMat.passinfo[0].m_face_mode = Renderer::FM_BACK;
	tempMat.passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	tempMat.removeReference();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );
	/*for ( uint i = 0; i < PART_MAX_COUNT; ++i )
	{
		if ( aPartHidden[i] ) {
			tempMat.setTexture( texParts[i] );
			tempMat.bind();
			GLd.DrawRectangle( 0,0, 256,256 );
			tempMat.unbind();
		}
	}*/
		/*tempMat.setTexture( texTattoo );
		tempMat.bind();
		glColor4f( 0.5f,0.5f,0.5f,1.0f );
		GLd.DrawRectangle( 0,0, 256,256 );
		GLd.DrawRectangle( 256,0, 256,256 );
		GLd.DrawRectangle( 0,256, 256,256 );
		GLd.DrawRectangle( 256,256, 256,256 );

		GLd.DrawRectangle( uvCursor.x*512 -16,uvCursor.y*512 -16, 32,32 );

		DrawToTarget();

		tempMat.unbind();*/
	/*for ( uint i = 0; i < tattoos.size(); ++i )
	{
		CTexture* tempTexture = new CTexture( ".res/textures/tats/"+string(tattoos[i].pattern.c_str())+".png" );
		tempMat.setTexture( tempTexture );
		tempMat.bind();
		glColor4fv( tattoos[i].color.start_point() );
		DrawTattooToTarget( tattoos[i] );
		tempMat.unbind();
		delete tempTexture;
	}*/

	GL.endOrtho();


	{	// Unbind buffer
		texTattooTarget->UnbindBuffer();
	}

	Debug::RTInspector->AddWatch( texTattooTarget );

	// Update skin mat
	glMaterial* skinMat;
	if ( model->mdlBody ) {
		skinMat = model->mdlBody->FindMaterial( "skin" );
		if ( skinMat ) {
			skinMat->setTexture( 3, texTattooTarget );
		}
	}
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

void CTattooTester::DrawToTarget ( void )
{	GL_ACCESS GLd_ACCESS
	//if ( !tattoo0.valid ) {
		return;
	//}

	/*
	sourceRay.pos = tattoo0.projection_pos;
	sourceRay.dir = tattoo0.projection_dir;
	CModelData* raw_model = model->mdlBody->GetModelData(0);
	int resultTri = Raytracer.RaycastExpensiveMiss( sourceRay, 20.0f, raw_model, hitPos, hitCoords );

	if ( resultTri >= 0 )
	{
		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0.5f,0,0 ) );
		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( -0.5f,0,0 ) );
		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,0.5f,0 ) );
		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,-0.5f,0 ) );
		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,0,0.5f ) );
		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,0,-0.5f ) );

		// detect texture coords
		Vector2d finalCoords;
		finalCoords.x =
			raw_model->vertices[raw_model->triangles[resultTri].vert[0]].u * hitCoords.x +
			raw_model->vertices[raw_model->triangles[resultTri].vert[1]].u * hitCoords.y +
			raw_model->vertices[raw_model->triangles[resultTri].vert[2]].u * hitCoords.z;
		finalCoords.y =
			raw_model->vertices[raw_model->triangles[resultTri].vert[0]].v * hitCoords.x +
			raw_model->vertices[raw_model->triangles[resultTri].vert[1]].v * hitCoords.y +
			raw_model->vertices[raw_model->triangles[resultTri].vert[2]].v * hitCoords.z;

		if ( finalCoords.y < 0 ) {
			finalCoords.y += 1;
		}

		GLd.DrawRectangle( finalCoords.x*512 -16,finalCoords.y*512 -16, 32,32 );
	}
	*/
	Rotator projection_rot;

	Vector3d hitPos, hitCoords;
	Ray sourceRay;
	CModelData* raw_model = model->mdlBody->GetModelData(0);
	int resultTri;

	// Need to draw to object as a series of rectangles
	Vector2d uvCoords [64];
	const int subdivX = 8;
	const int subdivY = 8;
	for ( int ix = -subdivX/2; ix < subdivX/2; ++ix )
	{
		for ( int iy = -subdivY/2; iy < subdivY/2; ++iy )
		{
			sourceRay.pos = tattoo0.projection_pos +
				(projection_rot * Vector3d( 0,ix/(ftype)subdivX * tattoo0.projection_scale.x,iy/(ftype)subdivY * tattoo0.projection_scale.y) );
			sourceRay.dir = tattoo0.projection_dir;
			
			resultTri = Raycaster.RaycastExpensiveMiss( sourceRay, 20.0f, raw_model, hitPos, hitCoords );

			// detect texture coords
			Vector2d finalCoords;
			finalCoords.x =
				raw_model->vertices[raw_model->triangles[resultTri].vert[0]].u * hitCoords.x +
				raw_model->vertices[raw_model->triangles[resultTri].vert[1]].u * hitCoords.y +
				raw_model->vertices[raw_model->triangles[resultTri].vert[2]].u * hitCoords.z;
			finalCoords.y =
				raw_model->vertices[raw_model->triangles[resultTri].vert[0]].v * hitCoords.x +
				raw_model->vertices[raw_model->triangles[resultTri].vert[1]].v * hitCoords.y +
				raw_model->vertices[raw_model->triangles[resultTri].vert[2]].v * hitCoords.z;
			if ( finalCoords.y < 0 ) {
				finalCoords.y += 1;
			}

			//GLd.DrawRectangle( finalCoords.x*512 -16,finalCoords.y*512 -16, 32,32 );
			uvCoords[(ix+subdivX/2)+(iy+subdivY/2)*subdivX] = finalCoords;
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
	ftype max_dist = tattoo0.projection_scale.magnitude() * 0.25f * ( 2.0f/ftype(subdivX+subdivY) );

	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
	for ( int ix = 0; ix < subdivX-1; ++ix )
	{
		for ( int iy = 0; iy < subdivY-1; ++iy )
		{
			/*bool singleGroup = true;
			if ( singleGroup )
			{
				glTexCoord2f( ix/(ftype)subdivX,iy/(ftype)subdivY );
				glVertex2f( uvCoords[ix+iy*subdivX].x*512, uvCoords[ix+iy*subdivX].y*512 );
				glTexCoord2f( ix/(ftype)subdivX,(iy+1)/(ftype)subdivY );
				glVertex2f( uvCoords[ix+(iy+1)*subdivX].x*512, uvCoords[ix+(iy+1)*subdivX].y*512  );
				glTexCoord2f( (ix+1)/(ftype)subdivX,iy/(ftype)subdivY );
				glVertex2f( uvCoords[(ix+1)+iy*subdivX].x*512, uvCoords[(ix+1)+iy*subdivX].y*512  );
				glTexCoord2f( (ix+1)/(ftype)subdivX,(iy+1)/(ftype)subdivY );
				glVertex2f( uvCoords[(ix+1)+(iy+1)*subdivX].x*512, uvCoords[(ix+1)+(iy+1)*subdivX].y*512  );
			}
			else
			{
				// take first corner, compute width and height. save.
				// with next corner, compute width and height. if same as first, skip.
				
			}*/

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
			int c;//int i, c, k;
			bool corners_drawn [4] = { false,false,false,false };

			// Loop through all the corners
			for ( c = 0; c < 4; ++c ) {
				if ( c != 0 ) continue;
				
				// In relation to first corner, check for valid corner points.
				invalidQuad = false;
				drawQuad = true;
				/*for ( i = 0; i < 4; ++i ) {
					if ( i == c ) continue;
					if ( (corners[c][i] - corners[c][c]).sqrMagnitude() > sqr( max_dist ) ) {
						invalidQuad = true;
					}
				}
				// If invalid quad, then change the corners
				if ( invalidQuad )
				{
					//for ( i = 0; i < 4; ++i ) {
					//	if ( i == c ) continue;
					//	if ( (corners[c][i] - corners[c][c]).sqrMagnitude() > sqr( max_dist ) ) {
					//		corners[c][i] = corners[c][c] + (corners[c][i] - corners[c][c]).normal() * max_dist;
					//	}
					//}
					drawQuad = false;
				}
				// Check if the quad should be drawn
				for ( k = 0; k < 4; ++k ) {
					// If one has been drawn
					if ( corners_drawn[k] ) {
						// Compare the corners with that 
						bool identical = true;
						for ( i = 0; i < 4; ++i ) {
							if ( (corners[c][i] - corners[k][i]).sqrMagnitude() > 1.0e-7 ) {
								identical = false;
							}
						}
						// Don't draw this one if it's identical
						if ( identical ) {
							drawQuad = false;
						}
					}
				}*/
				// Draw the quad
				if ( drawQuad )
				{
					GLd.P_PushTexcoord( uvcorners[0].x, uvcorners[0].y );
					GLd.P_AddVertex( corners[c][0].x*512, corners[c][0].y*512 );
					GLd.P_PushTexcoord( uvcorners[1].x, uvcorners[1].y  );
					GLd.P_AddVertex( corners[c][1].x*512, corners[c][1].y*512  );
					GLd.P_PushTexcoord( uvcorners[2].x, uvcorners[2].y  );
					GLd.P_AddVertex( corners[c][2].x*512, corners[c][2].y*512  );
					GLd.P_PushTexcoord( uvcorners[3].x, uvcorners[3].y  );
					GLd.P_AddVertex( corners[c][3].x*512, corners[c][3].y*512  );

					corners_drawn[c] = true; // Flag this corner set as drawn
				}
			}
		}
	}
	GLd.EndPrimitive();
}

void CTattooTester::DrawTattooToTarget ( tattoo_t& tattoo )
{ GL_ACCESS GLd_ACCESS
	const ftype xscale = 0.5f;
	const ftype yscale = (ftype)glMaterial::current->getTexture(0)->GetHeight()/(ftype)glMaterial::current->getTexture(0)->GetWidth();

	Vector3d hitPos, hitCoords;
	Ray sourceRay;
	CModelData* raw_model = model->mdlBody->GetModelData(0);
	int resultTri;

	Rotator projection_rot = Rotator( tattoo.projection_angle,0,0 ) * Rotator( Quaternion::CreateRotationTo( Vector3d(1,0,0), tattoo.projection_dir ) );
	//tattoo.projection_rot = Rotator( Quaternion::CreateRotationTo( Vector3d(1,0,0), tattoo.projection_dir ) );

	// Need to draw to object as a series of rectangles
	Vector2d uvCoords [64];
	const int subdivX = 8;
	const int subdivY = 8;
	for ( int ix = -subdivX/2; ix < subdivX/2; ++ix )
	{
		for ( int iy = -subdivY/2; iy < subdivY/2; ++iy )
		{
			sourceRay.pos = tattoo.projection_pos +
				(projection_rot * Vector3d( 0,ix/(ftype)subdivX * tattoo.projection_scale.x,iy/(ftype)subdivY * tattoo.projection_scale.y * yscale) );
			sourceRay.dir = tattoo.projection_dir;
			
			resultTri = Raycaster.RaycastExpensiveMiss( sourceRay, 20.0f, raw_model, hitPos, hitCoords );

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
			if ( finalCoords.y < 0 ) {
				finalCoords.y += 1;
			}

			//GLd.DrawRectangle( finalCoords.x*512 -16,finalCoords.y*512 -16, 32,32 );
			uvCoords[(ix+subdivX/2)+(iy+subdivY/2)*subdivX] = finalCoords;
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

	GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
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
			int c;//int i, c, k;
			bool corners_drawn [4] = { false,false,false,false };

			// Loop through all the corners
			for ( c = 0; c < 4; ++c ) {
				if ( c != 0 ) continue;
				
				// In relation to first corner, check for valid corner points.
				invalidQuad = false;
				drawQuad = true;
				// Draw the quad
				if ( drawQuad )
				{
					GLd.P_PushTexcoord( uvcorners[0].x, uvcorners[0].y );
					GLd.P_AddVertex( corners[c][0].x*512, corners[c][0].y*512 );
					GLd.P_PushTexcoord( uvcorners[1].x, uvcorners[1].y  );
					GLd.P_AddVertex( corners[c][1].x*512, corners[c][1].y*512  );
					GLd.P_PushTexcoord( uvcorners[2].x, uvcorners[2].y  );
					GLd.P_AddVertex( corners[c][2].x*512, corners[c][2].y*512  );
					GLd.P_PushTexcoord( uvcorners[3].x, uvcorners[3].y  );
					GLd.P_AddVertex( corners[c][3].x*512, corners[c][3].y*512  );

					corners_drawn[c] = true; // Flag this corner set as drawn
				}
			}
		}
	}
	GLd.EndPrimitive();
}

void CTattooTester::RaycastModel ( void )
{
//	Vector3d hitPos, hitCoords;
//	Ray sourceRay;
//	sourceRay.pos = CCamera::activeCamera->transform.position;
//	sourceRay.dir = CCamera::activeCamera->transform.Forward();
//	CModelData* raw_model = model->mdlBody->GetModelData(0);
//	int resultTri = Raytracer.Raycast( sourceRay, 20.0f, raw_model, hitPos, hitCoords );
//
//	if ( resultTri >= 0 )
//	{
//		/*Ray debugRay;
//		debugRay.dir = pOutHitInfo->hitNormal * 2.0f;
//		debugRay.pos = pOutHitInfo->hitPos;
//		Debug::Drawer->DrawRay(debugRay);*/
//		/*Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0.5f,0,0 ) );
//		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( -0.5f,0,0 ) );
//		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,0.5f,0 ) );
//		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,-0.5f,0 ) );
//		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,0,0.5f ) );
//		Debug::Drawer->DrawLine( hitPos, hitPos+Vector3d( 0,0,-0.5f ) );*/
//
//		// detect texture coords
//		Vector2d finalCoords;
//		finalCoords.x =
//			raw_model->vertices[raw_model->triangles[resultTri].vert[0]].u * hitCoords.x +
//			raw_model->vertices[raw_model->triangles[resultTri].vert[1]].u * hitCoords.y +
//			raw_model->vertices[raw_model->triangles[resultTri].vert[2]].u * hitCoords.z;
//		finalCoords.y =
//			raw_model->vertices[raw_model->triangles[resultTri].vert[0]].v * hitCoords.x +
//			raw_model->vertices[raw_model->triangles[resultTri].vert[1]].v * hitCoords.y +
//			raw_model->vertices[raw_model->triangles[resultTri].vert[2]].v * hitCoords.z;
//
//		if ( finalCoords.y < 0 ) {
//			finalCoords.y += 1;
//		}
//
//		uvCursor = finalCoords;
//
//		cout << "Hit UV pos: " << finalCoords << endl;
//	}
}

void CTattooTester::CreateGUI ( void )
{
	gui = new CDuskGUI;
	//gui->SetDefaultFont( new CBitmapFont( "YanoneKaffeesatz-R.otf", 13 ) );

	//sTattooElements_t
	{
		// Create container
		tCharMarks.container = gui->CreateDraggablePanel();
		gui->SetElementRect( tCharMarks.container, Rect( 0.03f,0.03f,0.27f,0.94f ) );
		gui->SetElementVisible( tCharMarks.container, false );
		gui->SetElementText( tCharMarks.container, "Marks, Scars and Tattoos" );
		
		tCharMarks.btndone = gui->CreateButton( tCharMarks.container );
		gui->SetElementRect( tCharMarks.btndone, Rect( 0.18f,0.91f,0.1f,0.05f ) );
		gui->SetElementText( tCharMarks.btndone, "Back" );

		tCharMarks.btn_next_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_next_tattoo.SetRect( Rect(0.25f,0.10f,0.04f,0.04f) );
		tCharMarks.btn_next_tattoo.SetText( ">" );
		tCharMarks.btn_prev_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_prev_tattoo.SetRect( Rect(0.04f,0.10f,0.04f,0.04f) );
		tCharMarks.btn_prev_tattoo.SetText( "<" );
		
		tCharMarks.lbl_current_tattoo = gui->CreateText( tCharMarks.container, "0/0" );
		tCharMarks.lbl_current_tattoo.SetRect( Rect(0.09f,0.10f,0.15f,0.04f) );

		tCharMarks.current_loaded_tattoo = 0;
		tCharMarks.need_list_refresh = false;

		tCharMarks.btn_add_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_add_tattoo.SetRect( Rect( 0.04f,0.15f,0.12f,0.04f ) );
		tCharMarks.btn_add_tattoo.SetText( "Add Mark" );
		tCharMarks.btn_delete_tattoo = gui->CreateButton( tCharMarks.container );
		tCharMarks.btn_delete_tattoo.SetRect( Rect( 0.17f,0.15f,0.12f,0.04f ) );
		tCharMarks.btn_delete_tattoo.SetText( "Delete Mark" );



		tCharMarks.lbl_position = gui->CreateText( tCharMarks.container, "Source Position" );
		tCharMarks.lbl_position.SetRect( Rect( 0.06f, 0.21f, 0.2f, 0.027f ) );
		tCharMarks.fld_position = gui->CreateVector3dPicker( tCharMarks.container );
		tCharMarks.fld_position.SetRect( Rect( 0.04f,0.25f,0.25f,0.04f ) );
		tCharMarks.lbl_tattoo_facing = gui->CreateText( tCharMarks.container, "Paste Direction" );
		tCharMarks.lbl_tattoo_facing.SetRect( Rect( 0.06f, 0.29f, 0.2f, 0.027f ) );
		tCharMarks.ddl_tattoo_facing = gui->CreateDropdownList( tCharMarks.container );
		tCharMarks.ddl_tattoo_facing.SetRect( Rect( 0.06f, 0.33f, 0.21f, 0.04f ) );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "On Character", 0 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "Align to front", 1 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "Align to side", 2 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_facing, "Align horizontally", 3 );
		
		tCharMarks.chk_mirrored = gui->CreateCheckbox( tCharMarks.container );
		tCharMarks.chk_mirrored.SetRect( Rect( 0.06f,0.39f,0.25f,0.04f ) );
		//tCharMarks.chk_mirrored.SetText( "Place Symmetrically" );
		tCharMarks.lbl_mirrored = gui->CreateText( tCharMarks.container, "Place Symmetrically" );
		tCharMarks.lbl_mirrored.SetRect( Rect( 0.09f,0.38f,0.2f,0.027f ) );

		tCharMarks.lbl_scale = gui->CreateText( tCharMarks.container, "Mark Size" );
		tCharMarks.lbl_scale.SetRect( Rect( 0.06f, 0.42f,0.2f, 0.027f ) );
		tCharMarks.sdr_scale = gui->CreateSlider( tCharMarks.container );
		tCharMarks.sdr_scale.SetRect( Rect( 0.06f, 0.46f,0.21f,0.04f ) );
		gui->SetSliderMinMax( tCharMarks.sdr_scale, 0.1f, 2.0f );

		tCharMarks.chk_flip = gui->CreateCheckbox( tCharMarks.container );
		tCharMarks.chk_flip.SetRect( Rect( 0.06f,0.52f,0.25f,0.04f ) );
		tCharMarks.lbl_flip = gui->CreateText( tCharMarks.container, "Flip image" );
		tCharMarks.lbl_flip.SetRect( Rect( 0.09f,0.51f,0.2f,0.027f ) );

		tCharMarks.lbl_curvature = gui->CreateText( tCharMarks.container, "Angle" );
		tCharMarks.lbl_curvature.SetRect( Rect( 0.06f, 0.55f,0.2f, 0.027f ) );
		tCharMarks.sdr_curvature = gui->CreateSlider( tCharMarks.container );
		tCharMarks.sdr_curvature.SetRect( Rect( 0.06f, 0.59f,0.21f,0.04f ) );
		gui->SetSliderMinMax( tCharMarks.sdr_curvature, -180.0f,180.0f );



		tCharMarks.lbl_tattoo_type = gui->CreateText( tCharMarks.container, "Type" );
		tCharMarks.lbl_tattoo_type.SetRect( Rect( 0.06f, 0.21f+0.44f, 0.2f ,0.027f ) );
		tCharMarks.ddl_tattoo_type = gui->CreateDropdownList( tCharMarks.container );
		tCharMarks.ddl_tattoo_type.SetRect( Rect( 0.06f, 0.25f+0.44f, 0.21f, 0.04f ) );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Clan Markings", 0 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Tattoos", 1 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Scars", 2 );
		gui->AddDropdownOption( tCharMarks.ddl_tattoo_type, "Fluxxmarks", 3 );
		tCharMarks.lbl_tattoo_index = gui->CreateText( tCharMarks.container, "Pattern" );
		tCharMarks.lbl_tattoo_index.SetRect( Rect( 0.06f, 0.29f+0.44f, 0.2f ,0.027f ) );
		tCharMarks.ddl_tattoo_index = gui->CreateDropdownList( tCharMarks.container );
		tCharMarks.ddl_tattoo_index.SetRect( Rect( 0.06f, 0.33f+0.44f, 0.21f, 0.04f ) );



		tCharMarks.lbl_blend = gui->CreateText( tCharMarks.container, "Blending Color" );
		tCharMarks.lbl_blend.SetRect( Rect( 0.04f, 0.85f, 0.1f, 0.027f ) );
		tCharMarks.clr_blend = gui->CreateColorPicker( tCharMarks.container );
		tCharMarks.clr_blend.SetRect( Rect( 0.21f,0.85f,0.07f,0.04f ) );

		tCharMarks.vTattooPosition = Vector3d( 0,0,24 );
		tCharMarks.fTattooScaling = 0.5f;

		tCharMarks.need_redraw_tattoos = true;

		tCharMarks.reinitialize = true;
	}
}

void CTattooTester::DoGUIWork ( void )
{
	CPlayer* pPlayer = (CPlayer*)CGameState::Active()->FindFirstObjectWithTypename( "CPlayer" );
	if ( gui->GetMouseInGUI() ) {
		pPlayer->bHasInput = false;
	}
	else {
		pPlayer->bHasInput = true;
	}


	if ( pPlayer->bHasInput ) {
		//if ( pPlayer->GetInputControl()->
		if ( Input::MouseDown(Input::MBRight) ) {
			//tattoo0.projection_src_angle = CCamera::activeCamera->transform.rotation;
			tattoo0.projection_dir = CCamera::activeCamera->transform.rotation * Vector3d(1,0,0);
			//tattoo0.projection_rot = CCamera::activeCamera->transform.rotation;
			tattoo0.projection_pos = CCamera::activeCamera->transform.position;
			tattoo0.projection_scale = Vector3d(0.4f,0.4f,1);
			//tattoo0.valid = true;
			std::cout << "New projection set." << std::endl;
		}
	}

	tCharMarks.container.SetVisible( true );

	// Now, GUI shit
	int prev_loaded_tattoo = tCharMarks.current_loaded_tattoo;
	if ( tCharMarks.btn_add_tattoo.GetButtonClicked() )
	{
		tCharMarks.current_loaded_tattoo = pl_stats->race_stats->tattooList.size();

		CRacialStats::tattoo_t newTat;
		newTat.color = Color( 0.5f,0.5f,0.5f,1.0f );
		newTat.pattern = "clan_laak";
		newTat.projection_dir = Vector3d( 0,1,0 );
		newTat.projection_pos = Vector3d( 0,-1.25f,0.5f );
		newTat.projection_angle = 0;
		newTat.projection_scale = Vector3d( 0.5f,0.5f,0.5f );
		newTat.mirror = false;
		newTat.type = 0;
		pl_stats->race_stats->tattooList.push_back( newTat );

		TattooGUILoadTattoo( tCharMarks.current_loaded_tattoo );
	}
	else if ( tCharMarks.btn_delete_tattoo.GetButtonClicked() )
	{
		pl_stats->race_stats->tattooList.erase( pl_stats->race_stats->tattooList.begin() + tCharMarks.current_loaded_tattoo );
		tCharMarks.current_loaded_tattoo -= 1;
	}
	if ( pl_stats->race_stats->tattooList.empty() )
	{
		tCharMarks.btn_next_tattoo.SetVisible( false );
		tCharMarks.btn_prev_tattoo.SetVisible( false );
	}
	else
	{
		tCharMarks.btn_next_tattoo.SetVisible( true );
		tCharMarks.btn_prev_tattoo.SetVisible( true );

		if ( tCharMarks.btn_next_tattoo.GetButtonClicked() ) {
			tCharMarks.current_loaded_tattoo += 1;
		}
		else if ( tCharMarks.btn_prev_tattoo.GetButtonClicked() ) {
			tCharMarks.current_loaded_tattoo -= 1;
		}
		if ( tCharMarks.current_loaded_tattoo < 0 ) {
			tCharMarks.current_loaded_tattoo = pl_stats->race_stats->tattooList.size()-1;
		}
		else if ( tCharMarks.current_loaded_tattoo == pl_stats->race_stats->tattooList.size() ) {
			tCharMarks.current_loaded_tattoo = 0;
		}

		if (( tCharMarks.reinitialize )||( prev_loaded_tattoo != tCharMarks.current_loaded_tattoo )) {
			TattooGUILoadTattoo( tCharMarks.current_loaded_tattoo );
			tCharMarks.reinitialize = false;
		}
	}
	
	if ( !pl_stats->race_stats->tattooList.empty() )
	{
		CRacialStats::tattoo_t prevTat = pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo];
		// place the tattoo
		gui->UpdateVector3dPicker( tCharMarks.fld_position, tCharMarks.vTattooPosition );
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos = tCharMarks.vTattooPosition / 12.0f;
		// scale the tattoo
		gui->UpdateSlider( tCharMarks.sdr_scale, tCharMarks.fTattooScaling );
		gui->UpdateCheckbox( tCharMarks.chk_flip, tCharMarks.cur_flipped );
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_scale = Vector3d((tCharMarks.cur_flipped)?-1.0f:1.0f,1,1) * tCharMarks.fTattooScaling;
		// align the tattoo
		int alignment = gui->GetDropdownOption( tCharMarks.ddl_tattoo_facing );
		if ( alignment == 0 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = -pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos;
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir.z *= 0.2f;
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir.normalize();
		}
		else if ( alignment == 2 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = Vector3d( (pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos.x > 0) ? -1.0f : 1.0f ,0,0 );
		}
		else if ( alignment == 1 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = Vector3d( 0,(pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos.y > 0) ? -1.0f : 1.0f ,0 );
		}
		else if ( alignment == 3 ) {
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir = Vector3d( 0,0, (pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos.z > 0) ? -1.0f : 1.0f );
		}
		// angle the tattoo
		gui->UpdateSlider( tCharMarks.sdr_curvature, pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_angle );
		// color the tattoo
		gui->UpdateColorPicker( tCharMarks.clr_blend, pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].color );
		// mirror the tattoo
		gui->UpdateCheckbox( tCharMarks.chk_mirrored, pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].mirror );
		// set tattoo type
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
		// set tattoo name
		pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern = tCharMarks.vTattooNames[gui->GetDropdownOption( tCharMarks.ddl_tattoo_index )].c_str();
		// tattoo limiting!
		TattooGUILimitTattoo( pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo] );

		// check memory, update if modified
		//if ( memcmp( (void*)&prevTat, (void*)&(tattoos[tCharMarks.current_loaded_tattoo]), sizeof( tattoo_t ) ) != 0 ) {
		//	cout << memcmp( (void*)&prevTat, (void*)&(tattoos[tCharMarks.current_loaded_tattoo]), sizeof( tattoo_t ) );
		//	cout << " " << memcmp( (void*)&(tattoos[tCharMarks.current_loaded_tattoo]), (void*)&(tattoos[tCharMarks.current_loaded_tattoo]), sizeof( tattoo_t ) ) << endl;
		//if ( prevTat.projection_angle != 
		//if ( memcmp( (void*)(&prevTat.color), (void*)&(tattoos[tCharMarks.current_loaded_tattoo].color), sizeof( ftype )*14 ) != 0 ) {
			//cout << memcmp( (void*)(&prevTat.color), (void*)&(tattoos[tCharMarks.current_loaded_tattoo].color), sizeof( ftype )*14 ) << endl;
		//if ( prevTat.color != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].color ) cout << "colour" << endl;
		//if ( prevTat.mirror != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].mirror ) cout << "mirror" << endl;
		//if ( prevTat.pattern != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern ) cout << "pattern" << endl;
		//cout << "-" << prevTat.pattern.c_str() << "- -" << pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern.c_str() << "- " << endl;
		/*if ( fabs(prevTat.projection_angle-pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_angle) > FTYPE_PRECISION ) cout << "pangle" << endl;
		if ( prevTat.projection_dir != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir ) cout << "pdir" << endl;
		if ( prevTat.projection_pos != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos ) cout << "ppos" << endl;
		if ( prevTat.projection_scale != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_scale ) cout << "pscale" << endl;
		if ( prevTat.type != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].type ) cout << "type" << endl;*/

		if (( prevTat.color != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].color )
			||( prevTat.mirror != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].mirror )
			||( prevTat.pattern != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].pattern )
			||( fabs(prevTat.projection_angle-pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_angle) > FTYPE_PRECISION )
			||( prevTat.projection_dir != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir )
			||( prevTat.projection_pos != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos )
			||( prevTat.projection_scale != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_scale )
			||( prevTat.type != pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].type ))
		{
			tCharMarks.need_redraw_tattoos = true;
		}
		else {
			tCharMarks.need_redraw_tattoos = false;
		}

		// Draw projection pos and such too
		Debug::Drawer->DrawLine(
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos + Vector3d(0,0,3),
			pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_pos + Vector3d(0,0,3) + pl_stats->race_stats->tattooList[tCharMarks.current_loaded_tattoo].projection_dir );
	}

	int old_tattoo_type = tCharMarks.cur_tattoo_type;
	tCharMarks.cur_tattoo_type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
	if ( tCharMarks.cur_tattoo_type != old_tattoo_type ) { 
		TattooGUIGenerateDropdown( tCharMarks.cur_tattoo_type );
	}

}

void CTattooTester::TattooGUIRefreshList ( void )
{

}


void CTattooTester::TattooGUIGenerateDropdown ( int typeList )
{
	gui->ClearDropdownList( tCharMarks.ddl_tattoo_index );
	tCharMarks.vTattooNames.clear();

	string target_section;
	int selection = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );

	if ( selection == 0 ) {
		target_section = "clanmark";
	}
	else if ( selection == 1 ) {
		target_section = "tattoo";
	}
	else if ( selection == 2 ) {
		target_section = "scar";
	}
	else if ( selection == 3 ) {
		target_section = "fluxxglows";
	}

	// Open .res/system/tattoo_manifest.txt and find target section
	std::ifstream tattoo_manifest ( ".res/system/tattoo_manifest.txt" );
	if ( !tattoo_manifest.is_open() ) {
		Debug::Console->PrintError( "Could not read manifest!\n" );
	}
	string tempRdString;
	bool found_section = false;
	while ( !found_section && tattoo_manifest.good() ) {
		tattoo_manifest >> tempRdString;
		tempRdString = StringUtils::FullTrim( tempRdString );
		if ( tempRdString[0] == '$' ) {
			if ( tempRdString.find( target_section ) != string::npos ) {
				found_section = true;
			}
		}
	}

	// Now, read in line combos
	char	cmd [256];
	string	sName, sFn;
	int i, readamt;
	while ( tattoo_manifest.good() && found_section )
	{
		// Load in the next line
		memset( cmd, 0, 256 );
		tattoo_manifest.getline( cmd, 256 );
		if ( !tattoo_manifest.good() ) { continue; }
		readamt = strlen( cmd );
		
		// Look for quote and load in until quote hit
		sName.clear();
		i = 0;
		while ( cmd[i] != '"' && (i<readamt) ) {
			if ( cmd[i] == '$' ) {
				found_section = false;
			}
			++i;
		}
		++i;
		while ( cmd[i] != '"' && (i<readamt) ) {
			sName += cmd[i];
			++i;
		}
		++i;

		// Skip spaces
		while ( (cmd[i] == ' ' || cmd[i] == '\t') && (i<readamt) ) {
			++i;
		}

		// Now get the filename
		sFn.clear();
		while (( i < readamt )&&( cmd[i] != '\n' ))
		{
			sFn += cmd[i];
			++i;
		}

		// Now, open the .res/textures/tats/ folder and find files with matching name
		// Or, you know, add it to a list. Because that's easier.
		if ( sFn.length() > 0 ) {
			gui->AddDropdownOption( tCharMarks.ddl_tattoo_index, sName, tCharMarks.vTattooNames.size() );
			tCharMarks.vTattooNames.push_back( sFn );
		}
	}

}


void CTattooTester::TattooGUILoadTattoo( int listIndex )
{
	tCharMarks.current_loaded_tattoo = listIndex;

	char temp_str [512];
	sprintf( temp_str, "%d/%d", listIndex+1, pl_stats->race_stats->tattooList.size() );
	tCharMarks.lbl_current_tattoo.SetText( temp_str );

	// load tat
	tCharMarks.vTattooPosition = pl_stats->race_stats->tattooList[listIndex].projection_pos * 12;
	tCharMarks.fTattooScaling = fabs(pl_stats->race_stats->tattooList[listIndex].projection_scale.x);

	// get tat type from name
	/*string tatName = string(tattoos[listIndex].pattern.c_str());
	int type = 0;
	if ( tatName.find( "clan_" ) != string::npos ) {
		type = 0;
	}
	else if ( tatName.find( "tatt_" ) != string::npos ) {
		type = 1;
	}
	else if ( tatName.find( "scar_" ) != string::npos ) {
		type = 2;
	}
	else if ( tatName.find( "glow_" ) != string::npos ) {
		type = 3;
	}*/
	// get tat type from tat type
	int type = pl_stats->race_stats->tattooList[listIndex].type;

	// set type lists
	gui->SetDropdownOption( tCharMarks.ddl_tattoo_type, type );
	tCharMarks.cur_tattoo_type = gui->GetDropdownOption( tCharMarks.ddl_tattoo_type );
	TattooGUIGenerateDropdown( type );
	// find selection in list
	string tatName = string(pl_stats->race_stats->tattooList[listIndex].pattern.c_str());
	for ( uint i = 0; i < tCharMarks.vTattooNames.size(); ++i )
	{
		if ( tCharMarks.vTattooNames[i] == tatName ) {
			gui->SetDropdownOption( tCharMarks.ddl_tattoo_index, i );
			break;
		}
	}

	// figure out alignement with dot products
	int projection_type = 0;
	if ( fabs(pl_stats->race_stats->tattooList[listIndex].projection_dir.dot( Vector3d(0,1,1) )) < 1e-6 ) {
		projection_type = 2;
	}
	else if ( fabs(pl_stats->race_stats->tattooList[listIndex].projection_dir.dot( Vector3d(1,0,1) )) < 1e-6 ) {
		projection_type = 1;
	}
	else if ( fabs(pl_stats->race_stats->tattooList[listIndex].projection_dir.dot( Vector3d(1,1,0) )) < 1e-6 ) {
		projection_type = 3;
	}
	gui->SetDropdownOption( tCharMarks.ddl_tattoo_facing, projection_type );

	// Set flipped
	if ( pl_stats->race_stats->tattooList[listIndex].projection_scale.x * pl_stats->race_stats->tattooList[listIndex].projection_scale.y >= 0 ) {
		tCharMarks.cur_flipped = false;
	}
	else {
		tCharMarks.cur_flipped = true;
	}


}

void CTattooTester::TattooGUILimitTattoo ( CRacialStats::tattoo_t& tattoo )
{
	switch ( tattoo.type )
	{
	case TATT_CLANMARK:
	case TATT_TATTOO:
		tattoo.color.red = tattoo.color.red<0.5 ? tattoo.color.red : 0.5f;
		tattoo.color.green = tattoo.color.green<0.5 ? tattoo.color.green : 0.5f;
		tattoo.color.blue = tattoo.color.blue<0.5 ? tattoo.color.blue : 0.5f;
		break;
	case TATT_SCAR:
		//tattoo.color = Color( 0.5f,0.5f,0.5f,tattoo.color.alpha );
		tattoo.color.red = 0.5f;
		tattoo.color.green = 0.5f;
		tattoo.color.blue = 0.5f;
		break;
	case TATT_FLUXXGLOW:
		tattoo.color.red = tattoo.color.red>0.5 ? tattoo.color.red : 0.5f;
		tattoo.color.green = tattoo.color.green>0.5 ? tattoo.color.green : 0.5f;
		tattoo.color.blue = tattoo.color.blue>0.5 ? tattoo.color.blue : 0.5f;
		break;
	};
}