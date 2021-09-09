#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "engine2d/entities/map/TileMap.h"

#include "gpuw/Device.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/Material.h"
#include "renderer/object/immediate/immediate.h"

#include "TileSelectorUI.h"

using namespace M04;

TileSelectorUI::TileSelectorUI ( void )
	: RrRenderObject(), m_tileset(NULL)
{
	// Pass 0 is the tileset
	RrPass tilesetPass;
	tilesetPass.utilSetupAsDefault();
	tilesetPass.utilSetupAs2D();
	tilesetPass.m_layer = renderer::kRenderLayerV2D;
	tilesetPass.m_type = kPassTypeForward;
	tilesetPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	tilesetPass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	tilesetPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	renderer::shader::Location t_vspecA[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	tilesetPass.setVertexSpecificationByCommonList(t_vspecA, 3);
	tilesetPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	PassInitWithInput(0, &tilesetPass);

	// Pass 1 is the UI overlay
	RrPass uiPass;
	uiPass.utilSetupAsDefault();
	uiPass.utilSetupAs2D();
	uiPass.m_layer = renderer::kRenderLayerV2D;
	uiPass.m_type = kPassTypeForward;
	uiPass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	uiPass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	uiPass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) );
	renderer::shader::Location t_vspecB[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	uiPass.setVertexSpecificationByCommonList(t_vspecB, 3);
	uiPass.m_primitiveType = gpu::kPrimitiveTopologyTriangleList;
	PassInitWithInput(1, &uiPass);

	// ui value setup
	ui_columns = 4;
	ui_scale = Vector2f( 32,32 );
	ui_base_offset = Vector2f( 0,40 );
}

TileSelectorUI::~TileSelectorUI ( void )
{
	mesh_buffer_tiles.FreeMeshBuffers();
	mesh_buffer_ui.FreeMeshBuffers();
}

//		SetTileMap ( )
// Sets the tilemap containing the tileset data that need to pull from
void TileSelectorUI::SetTileMap ( Engine2D::TileMap* target )
{
	SetTileset( target->m_tileset );

	// TODO: Convert the texture. For now, set the material based on the input file.
	PassAccess(0).setTexture( TEX_DIFFUSE, RrTexture::Load(target->m_sprite_file.c_str()) );

	// ui value setup
	ui_columns = 5;
	ui_spacing = 2;
	ui_scale = Vector2f( (Real)m_tileset->tilesize_x, (Real)m_tileset->tilesize_y );
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
	Vector2f t_mousepos ( core::Input::MouseX(), core::Input::MouseY() );

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

bool TileSelectorUI::CreateConstants ( rrCameraPass* pass ) 
{
	PushCbufferPerObject(this->transform.world, pass);
	return true;
}

bool TileSelectorUI::EndRender ( void )
{
	rrMeshBuilder2D builder(core::GetFocusedScreen().GetSize(), (uint16_t)(4 + 4 * m_tileset->tiles.size()));
	rrMeshBuilder2D builderLn(core::GetFocusedScreen().GetSize(), 16);

	// render the selection shit
	{
		// Get background width
		Vector2f ui_size ( (Real) m_tileset->tilesize_x+ui_spacing, (Real) m_tileset->tilesize_y+ui_spacing );
		ui_size.x *= ui_columns;
		ui_size.y *= std::ceilf( (m_tileset->tiles.size() / (Real)ui_columns) );
		Vector2f ui_offset = ui_base_offset;

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
		Vector2f ui_offset = Vector2f( (Real) (i%ui_columns) * (m_tileset->tilesize_x+ui_spacing), (Real) (i/ui_columns) * (m_tileset->tilesize_y+ui_spacing) ) + ui_base_offset;

		// Generate the UV coordinate basics
		Vector2f tile_scale ( 1.0f / m_tileset->tilecount_x, 1.0f / m_tileset->tilecount_y );
		Vector2f tile_offset = Vector2f( tile.atlas_x, tile.atlas_y ).mulComponents(tile_scale);

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
		Vector2f ui_offset = Vector2f( (Real) (i%ui_columns) * (m_tileset->tilesize_x+ui_spacing), (Real) (i/ui_columns) * (m_tileset->tilesize_y+ui_spacing) ) + ui_base_offset;

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

	arModelData l_tilesModel = builder.getModelData();
	arModelData l_uiModel = builderLn.getModelData();

	mesh_index_count_tiles = l_tilesModel.indexNum;
	mesh_index_count_ui = l_uiModel.indexNum;

	mesh_buffer_tiles.InitMeshBuffers(&l_tilesModel);
	mesh_buffer_ui.InitMeshBuffers(&l_uiModel);

	return true;
}

bool TileSelectorUI::Render ( const rrRenderParams* params )
{
	rrMeshBuffer* l_currentMeshBuffer = (params->pass == 0) ? &mesh_buffer_tiles : &mesh_buffer_ui;
	uint* l_currentIndexCount = (params->pass == 0) ? &mesh_index_count_tiles : &mesh_index_count_ui;

	if (*l_currentIndexCount > 0)
	{
		if ( !l_currentMeshBuffer->m_mesh_uploaded )
			return true; // Only render when have a valid mesh and rendering enabled

		gpu::GraphicsContext* gfx = params->context->context_graphics;

		gpu::Pipeline* pipeline = GetPipeline( params->pass );
		gfx->setPipeline(pipeline);
		// Set up the material helper...
		renderer::Material(this, params->context, params, pipeline)
			// set the depth & blend state registers
			.setDepthStencilState()
			.setRasterizerState()
			// bind the samplers & textures
			.setBlendState()
			.setTextures();
		// bind the vertex buffers
		for (int i = 0; i < renderer::shader::kVBufferSlotMaxCount; ++i)
			if (l_currentMeshBuffer->m_bufferEnabled[i])
				gfx->setVertexBuffer(i, &l_currentMeshBuffer->m_buffer[i], 0);
		// bind the index buffer
		gfx->setIndexBuffer(&l_currentMeshBuffer->m_indexBuffer, gpu::kIndexFormatUnsigned16);
		// bind the cbuffers
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_MATRICES, &m_cbufPerObjectMatrices);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_OBJECT_EXTENDED, &m_cbufPerObjectSurfaces[params->pass]);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_CAMERA_INFORMATION, params->cbuf_perCamera);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_PASS_INFORMATION, params->cbuf_perPass);
		gfx->setShaderCBuffer(gpu::kShaderStageVs, renderer::CBUFFER_PER_FRAME_INFORMATION, params->cbuf_perFrame);
		// draw now
		gfx->drawIndexed(*l_currentIndexCount, 0, 0);
	}
	return true;
}