
#include "TileSelectorUI.h"
#include "engine2d/entities/map/TileMap.h"

#include "core/system/Screen.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "core/input/CInput.h"

using namespace M04;

TileSelectorUI::TileSelectorUI ( void )
	: CRenderableObject(), m_tileset(NULL)
{
	// Sys drawing
	renderType = Renderer::V2D;

	// Mat init
	glMaterial* draw_mat = new glMaterial;
	draw_mat->m_diffuse = Color( 1,1,1,1 );
	draw_mat->setTexture( 0, new CTexture("null") );
	draw_mat->passinfo.push_back( glPass() );
	draw_mat->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	draw_mat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	draw_mat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	draw_mat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	SetMaterial( draw_mat );
	draw_mat->removeReference();

	glMaterial* ui_mat = new glMaterial;
	ui_mat->m_diffuse = Color( 1,1,1,1 );
	ui_mat->setTexture( 0, new CTexture( "textures/white.jpg" ) );
	ui_mat->passinfo.push_back( glPass() );
	ui_mat->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	ui_mat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	ui_mat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	ui_mat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	ui_material = ui_mat;

	// ui value setup
	ui_columns = 4;
	ui_scale = Vector2d( 32,32 );
	ui_scale = ui_scale.mulComponents( Vector2d( 1.0F / Screen::Info.width, 1.0F / Screen::Info.height ) );
	ui_base_offset = Vector2d( 0,40 );
}

TileSelectorUI::~TileSelectorUI ( void )
{
	ui_material->removeReference();
	delete_safe( ui_material );
}

//		SetTileMap ( )
// Sets the tilemap containing the tileset data that need to pull from
void TileSelectorUI::SetTileMap ( Engine2D::TileMap* target )
{
	SetTileset( target->m_tileset );

	// TODO: Convert the texture. For now, set the material based on the input file.
	m_material->setTexture(
		0,
		new CTexture (
			target->m_sprite_file.c_str(), 
			Texture2D, RGBA8,
			1024,1024, Clamp,Clamp,
			MipmapNone,SamplingPoint
		)
	);

	// ui value setup
	ui_columns = 5;
	ui_spacing = 2;
	ui_scale = Vector2d( (Real)m_tileset->tilesize_x, (Real)m_tileset->tilesize_y );
	ui_scale = ui_scale.mulComponents( Vector2d( 1.0f / Screen::Info.width, 1.0f / Screen::Info.height ) );
}
//		SetTileset ( )
// Sets the tileset to pull data from
void TileSelectorUI::SetTileset ( Engine2D::Tileset* target )
{
	m_tileset = target;
}

//		UIMouseoverTiletype ( )
// Get the tileset tile type the mouse is currently over.
// Returns negative value when below 
int TileSelectorUI::UIMouseoverTiletype ( void )
{
	Vector2d t_mousepos ( Input::MouseX(), Input::MouseY() );

	int ix = (int)( (t_mousepos.x-ui_base_offset.x) / (m_tileset->tilesize_x+ui_spacing) );
	int iy = (int)( (t_mousepos.y-ui_base_offset.y) / (m_tileset->tilesize_y+ui_spacing) );

	if ( iy >= 0 && ix >= 0 && ix < ui_columns )
	{
		ui_mouseover = ix + iy * ui_columns;
		if ( ui_mouseover >= (int)m_tileset->tiles.size() ) {
			ui_mouseover = -1;
		}
	}
	else
	{
		ui_mouseover = -1;
	}

	return ui_mouseover;
}
void TileSelectorUI::UISetSelection ( int mouseover )
{
	ui_mouseover = mouseover;
}

bool TileSelectorUI::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS

	GL.prepareDraw();
	GL.beginOrtho( 0,0, 1,1, -45,45 );
	GLd.DrawSet2DScaleMode();

	m_material->bindPass(0);
	m_material->setShaderConstants( this );

	// render the selection shit
	GLd.BeginPrimitive( GL_TRIANGLES );
	GLd.P_PushColor( Color(0.5F,0.5F,0.5F,1) );
	{
		// Get background width
		Vector2d ui_size ( (Real) m_tileset->tilesize_x+ui_spacing, (Real) m_tileset->tilesize_y+ui_spacing );
		ui_size.x *= ui_columns;
		ui_size.y *= std::ceilf( (m_tileset->tiles.size() / (Real)ui_columns) );
		ui_size = ui_size.mulComponents( Vector2d( 1.0f / Screen::Info.width, 1.0f / Screen::Info.height ) );
		Vector2d ui_offset = ui_base_offset;
		ui_offset = ui_offset.mulComponents( Vector2d( 1.0f / Screen::Info.width, 1.0f / Screen::Info.height ) );
		//ui_size.divComponents( Vector2d( (Real)m_tileset->tilesize_x, (Real)m_tileset->tilesize_y ) );
		//ui_size = ui_size.mulComponents( Vector2d( 1.0f / Screen::Info.width, 1.0f / Screen::Info.height ) );

		// Draw the background for the selection box
		GLd.P_PushTexcoord( Vector2d(0.5F,0.5F) );
		// Create the quad
		GLd.P_AddVertex( Vector2d(0,0) + ui_offset );
		GLd.P_AddVertex( Vector2d(ui_size.x,0) + ui_offset );
		GLd.P_AddVertex( Vector2d(0,ui_size.y) + ui_offset );

		GLd.P_AddVertex( Vector2d(0,ui_size.y) + ui_offset );
		GLd.P_AddVertex( Vector2d(ui_size.x,0) + ui_offset );
		GLd.P_AddVertex( Vector2d(ui_size.x,ui_size.y) + ui_offset );
	}
	GLd.P_PushColor( Color(1,1,1,1) );
	for ( int i = 0; i < (int)m_tileset->tiles.size(); ++i )
	{
		const tilesetEntry_t& tile = m_tileset->tiles[i];
		
		// Generate position to put the tile
		Vector2d ui_offset = Vector2d( (Real) (i%ui_columns) * (m_tileset->tilesize_x+ui_spacing), (Real) (i/ui_columns) * (m_tileset->tilesize_y+ui_spacing) ) + ui_base_offset;
		ui_offset = ui_offset.mulComponents( Vector2d( 1.0f / Screen::Info.width, 1.0f / Screen::Info.height ) );

		// Generate the UV coordinate basics
		Vector2d tile_scale ( 1.0f / m_tileset->tilecount_x, 1.0f / m_tileset->tilecount_y );
		Vector2d tile_offset = Vector2d( tile.atlas_x, tile.atlas_y ).mulComponents(tile_scale);

		// Create quad
		GLd.P_PushTexcoord( Vector2d(0,0).mulComponents(tile_scale) + tile_offset );
		GLd.P_AddVertex( Vector2d(0,0).mulComponents(ui_scale) + ui_offset );
		GLd.P_PushTexcoord( Vector2d(tile.atlas_w,0).mulComponents(tile_scale) + tile_offset );
		GLd.P_AddVertex( Vector2d(tile.atlas_w,0).mulComponents(ui_scale) + ui_offset );
		GLd.P_PushTexcoord( Vector2d(0,tile.atlas_h).mulComponents(tile_scale) + tile_offset );
		GLd.P_AddVertex( Vector2d(0,tile.atlas_h).mulComponents(ui_scale) + ui_offset );

		GLd.P_PushTexcoord( Vector2d(0,tile.atlas_h).mulComponents(tile_scale) + tile_offset );
		GLd.P_AddVertex( Vector2d(0,tile.atlas_h).mulComponents(ui_scale) + ui_offset );
		GLd.P_PushTexcoord( Vector2d(tile.atlas_w,0).mulComponents(tile_scale) + tile_offset );
		GLd.P_AddVertex( Vector2d(tile.atlas_w,0).mulComponents(ui_scale) + ui_offset );
		GLd.P_PushTexcoord( Vector2d(tile.atlas_w,tile.atlas_h).mulComponents(tile_scale) + tile_offset );
		GLd.P_AddVertex( Vector2d(tile.atlas_w,tile.atlas_h).mulComponents(ui_scale) + ui_offset );
	}
	GLd.EndPrimitive();

	ui_material->bindPass(0);
	ui_material->setShaderConstants( this );
	// render the selection box
	GLd.BeginPrimitive( GL_LINES );
	GLd.P_PushTexcoord( 0,0 );
	if ( ui_mouseover >= 0 )
	{
		const tilesetEntry_t& tile = m_tileset->tiles[ui_mouseover];
		int i = ui_mouseover;

		// Generate position to put the tile
		Vector2d ui_offset = Vector2d( (Real) (i%ui_columns) * (m_tileset->tilesize_x+ui_spacing), (Real) (i/ui_columns) * (m_tileset->tilesize_y+ui_spacing) ) + ui_base_offset;
		ui_offset = ui_offset.mulComponents( Vector2d( 1.0f / Screen::Info.width, 1.0f / Screen::Info.height ) );

		// Create border value
		const Real f = 1.0F / m_tileset->tilesize_x;

		// Create selection box
		for ( int l = -1; l <= 1; ++l )
		{
			// Make center of the border black
			if ( l == 0 )	GLd.P_PushColor( Color(0,0,0,1.0F) );
			else			GLd.P_PushColor( Color(1,1,1,1.0F) );
			// Place box
			GLd.P_AddVertex( Vector2d(-f*l,-f*l).mulComponents(ui_scale) + ui_offset );
			GLd.P_AddVertex( Vector2d(tile.atlas_w+f*l,-f*l).mulComponents(ui_scale) + ui_offset );

			GLd.P_AddVertex( Vector2d(tile.atlas_w+f*l,-f*l).mulComponents(ui_scale) + ui_offset );
			GLd.P_AddVertex( Vector2d(tile.atlas_w+f*l,tile.atlas_h+f*l).mulComponents(ui_scale) + ui_offset );

			GLd.P_AddVertex( Vector2d(tile.atlas_w+f*l,tile.atlas_h+f*l).mulComponents(ui_scale) + ui_offset );
			GLd.P_AddVertex( Vector2d(-f*l,tile.atlas_h+f*l).mulComponents(ui_scale) + ui_offset );

			GLd.P_AddVertex( Vector2d(-f*l,tile.atlas_h+f*l).mulComponents(ui_scale) + ui_offset );
			GLd.P_AddVertex( Vector2d(-f*l,-f*l).mulComponents(ui_scale) + ui_offset );
		}
	}
	GLd.EndPrimitive();

	GL.endOrtho();
	GL.cleanupDraw();

	return true;
}