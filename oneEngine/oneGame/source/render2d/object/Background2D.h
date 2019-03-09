//===============================================================================================//
//
//	class Background2D
//
// Renders a 2D quad in the background.
//
//===============================================================================================//

#ifndef RENDER2D_C_BACKGROUND_2D_H_
#define RENDER2D_C_BACKGROUND_2D_H_

#include "core/types/ModelData.h"
#include "render2d/object/CRenderable2D.h"

namespace renderer
{
	//	Background2D
	// Renders a 2D quad in the background
	class Background2D : public CRenderable2D
	{
	public:
		RENDER2D_API explicit	Background2D ( void );
		RENDER2D_API			~Background2D ();

		RENDER2D_API bool		PreRender ( rrCameraPass* cameraPass ) override;
		RENDER2D_API bool		Render ( const rrRenderParams* params ) override;
	};
}

#endif//RENDER2D_C_BACKGROUND_2D_H_