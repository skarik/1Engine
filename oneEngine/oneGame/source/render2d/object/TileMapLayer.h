//===============================================================================================//
//	class TileMapLayer
//
// Class for rendering a map layer
//
//
//===============================================================================================//

#ifndef _RENDER2D_C_TILEMAPLAYER_H_
#define _RENDER2D_C_TILEMAPLAYER_H_

#include "core/types/ModelData.h"
#include "render2d/object/CRenderable2D.h"

namespace renderer
{
	class TileMapLayer : public CRenderable2D
	{
	public:
		RENDER2D_API explicit	TileMapLayer ( void );
		RENDER2D_API			~TileMapLayer ();

		RENDER2D_API void		SetLayer ( const arModelData* model_info );

	public:
		int		source_layer_id;
	};
}

#endif//_RENDER2D_C_RENDERABLE_2D_H_