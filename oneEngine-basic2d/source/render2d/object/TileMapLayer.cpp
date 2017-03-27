
#include "TileMapLayer.h"

renderer::TileMapLayer::TileMapLayer ( void )
	: CRenderable2D(), source_layer_id(0)
{
	;
}
renderer::TileMapLayer::~TileMapLayer ( void )
{
	;
}

void renderer::TileMapLayer::SetLayer( const ModelData* model_info )
{
	m_modeldata = *model_info;
	PushModeldata();
}