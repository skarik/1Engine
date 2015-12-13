
#include "TileMapLayer.h"

Renderer::TileMapLayer::TileMapLayer ( void )
	: CRenderable2D()
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