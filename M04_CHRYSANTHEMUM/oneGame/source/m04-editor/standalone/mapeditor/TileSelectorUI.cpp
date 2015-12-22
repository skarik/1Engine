
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

	// ui value setup
	const int columns = 4;
	Vector2d ui_scale ( m_tileset->tilesize_x, m_tileset->tilesize_y );
	Vector2d ui_offset = Vector2d( (i%columns) * (ui_scale.x+1), (i/columns) * (ui_scale.y+1) );
	ui_scale = ui_scale.mulComponents( Vector2d( 1.0 / Screen::Info.width, 1.0 / Screen::Info.height ) );
	ui_offset = ui_offset.mulComponents( Vector2d( 1.0 / Screen::Info.width, 1.0 / Screen::Info.height ) );
}

TileSelectorUI::~TileSelectorUI ( void )
{
	;
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

	int ix = (int)( t_mousepos.x * ui_scale.x + ui_offset.x );
	int iy = (int)( t_mousepos.y * ui_scale.y + ui_offset.y );

	return -1;
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
	for ( int i = 0; i < m_tileset->tiles.size(); ++i )
	{
		const tilesetEntry_t& tile = m_tileset->tiles[i];
		
		GLd.P_PushColor( Color(1,1,1,1) );

		Vector2d tile_scale ( 1.0f / m_tileset->tilecount_x, 1.0f / m_tileset->tilecount_y );
		Vector2d tile_offset = Vector2d( tile.atlas_x, tile.atlas_y ).mulComponents(tile_scale);

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

	GL.endOrtho();
	GL.cleanupDraw();

	return true;
}