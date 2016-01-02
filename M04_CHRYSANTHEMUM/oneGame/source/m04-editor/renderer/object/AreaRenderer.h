
#ifndef _2D_AREA_RENDERER_H_
#define _2D_AREA_RENDERER_H_

#include "renderer/object/CRenderableObject.h"

namespace M04
{
	class AreaRenderer : public CRenderableObject
	{
	public:
		AreaRenderer ( void );
		~AreaRenderer( void );

		bool Render ( const char pass ) override;
	};
}


#endif//_2D_AREA_RENDERER_H_