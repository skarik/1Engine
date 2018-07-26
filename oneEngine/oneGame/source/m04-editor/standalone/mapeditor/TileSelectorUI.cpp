
#include "TileSelectorUI.h"
#include "engine2d/entities/map/TileMap.h"

#include "core/system/Screen.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/texture/RrTexture.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "renderer/object/immediate/immediate.h"

#include "core/input/CInput.h"

using namespace M04;

TileSelectorUI::TileSelectorUI ( void )
	: CRenderableObject(), m_tileset(NULL)
{
	// Sys drawing
	renderLayer = renderer::kRLV2D;

	// Mat init
	RrMaterial* draw_mat = new RrMaterial;
	draw_mat->m_diffuse = Color( 1,1,1,1 );
	draw_mat->setTexture( TEX_DIFFUSE, new RrTexture("null") );
	draw_mat->passinfo.push_back( RrPassForward() );
	draw_mat->passinfo[0].shader = new RrShader( ".res/shaders/v2d/default.glsl" );
	draw_mat->passinfo[0].set2DCommon();
	SetMaterial( draw_mat );
	draw_mat->removeReference();

	RrMaterial* ui_mat = new RrMaterial;
	ui_mat->m_diffuse = Color( 1,1,1,1 );
	ui_mat->setTexture( TEX_DIFFUSE, new RrTexture( "textures/white.jpg" ) );
	ui_mat->passinfo.push_back( RrPassForward() );
	ui_mat->passinfo[0].shader = new RrShader( ".res/shaders/v2d/default.glsl" );
	ui_mat->passinfo[0].set2DCommon();
	ui_material = ui_mat;

	// ui value setup
	ui_columns = 4;
	ui_scale = Vector2d( 32,32 );
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
		TEX_MAIN,
		new RrTexture (
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

bool TileSelectorUI::PreRender ( void ) 
{
	m_material->prepareShaderConstants();
	ui_material->prepareShaderConstants();
	return true;
}
bool TileSelectorUI::Render ( const char pass )
{
	rrMeshBuilder2D builder((uint16_t)(4 + 4 * m_tileset->tiles.size()));
	rrMeshBuilder2D builderLn(16);

	// render the selection shit
	{
		// Get background width
		Vector2d ui_size ( (Real) m_tileset->tilesize_x+ui_spacing, (Real) m_tileset->tilesize_y+ui_spacing );
		ui_size.x *= ui_columns;
		ui_size.y *= std::ceilf( (m_tileset->tiles.size() / (Real)ui_columns) );
		Vector2d ui_offset = ui_base_offset;

		// Draw the background for the selection box
		builder.addRectTex(
			Rect(ui_offset, ui_size),
			Rect(0.5F, 0.5F, 0.0F, 0.0),
			Color(0.5F,0.5F,0.5F,1.0F),
			false
		);
	}
	for ( int i = 0; i < (int)m_tileset->tiles.size(); ++i )
	{
		const tilesetEntry_t& tile = m_tileset->tiles[i];
		
		// Generate position to put the tile
		Vector2d ui_offset = Vector2d( (Real) (i%ui_columns) * (m_tileset->tilesize_x+ui_spacing), (Real) (i/ui_columns) * (m_tileset->tilesize_y+ui_spacing) ) + ui_base_offset;

		// Generate the UV coordinate basics
		Vector2d tile_scale ( 1.0f / m_tileset->tilecount_x, 1.0f / m_tileset->tilecount_y );
		Vector2d tile_offset = Vector2d( tile.atlas_x, tile.atlas_y ).mulComponents(tile_scale);

		builder.addRectTex(
			Rect(ui_offset, Vector2f(tile.atlas_w, tile.atlas_h).mulComponents(ui_scale)),
			Rect(tile_offset, Vector2f(tile.atlas_w, tile.atlas_h).mulComponents(tile_scale)),
			Color(1.0F, 1.0F, 1.0F, 1.0F),
			false
		);
	}

	// render the selection box
	if ( ui_mouseover >= 0 )
	{
		const tilesetEntry_t& tile = m_tileset->tiles[ui_mouseover];
		int i = ui_mouseover;

		// Generate position to put the tile
		Vector2d ui_offset = Vector2d( (Real) (i%ui_columns) * (m_tileset->tilesize_x+ui_spacing), (Real) (i/ui_columns) * (m_tileset->tilesize_y+ui_spacing) ) + ui_base_offset;

		// Create border value
		const Real f = 1.0F / m_tileset->tilesize_x;

		// Create selection box
		for ( int l = -1; l <= 1; ++l )
		{
			float l_r = (Real)l;
			builderLn.addRect(
				Rect( ui_offset + Vector2f(-l_r,-l_r), Vector2f(tile.atlas_w, tile.atlas_h).mulComponents(ui_scale) + Vector2f(l_r,l_r) * 2.0F ),
				// Make center of the border black
				(l == 0) ? Color(0,0,0,1.0F) : Color(1,1,1,1.0F),
				true );
		}
	}

	RrScopedMeshRenderer renderer;
	renderer.render(this, m_material, 0, builder);
	renderer.render(this, ui_material, 0, builderLn);

	return true;
}