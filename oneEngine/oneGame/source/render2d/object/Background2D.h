//===============================================================================================//
//	class Background2D
//
// Class for rendering a map layer
//
//
//===============================================================================================//

#ifndef _RENDER2D_C_BACKGROUND_2D_H_
#define _RENDER2D_C_BACKGROUND_2D_H_

#include "core/types/ModelData.h"
#include "render2d/object/CRenderable2D.h"

namespace renderer
{
	class Background2D : public CRenderable2D
	{
	public:
		RENDER2D_API explicit	Background2D ( void );
		RENDER2D_API			~Background2D ();

		RENDER2D_API bool		Render ( const char pass ) override;
	};
}

#endif//_RENDER2D_C_BACKGROUND_2D_H_