
#include "TileMapLayer.h"

Renderer::TileMapLayer::TileMapLayer ( void )
	: CRenderable2D(), source_layer_id(0)
{
	;
}
Renderer::TileMapLayer::~TileMapLayer ( void )
{
	;
}

void Renderer::TileMapLayer::SetLayer( const ModelData* model_info )
{
	m_modeldata = *model_info;
	PushModeldata();
}