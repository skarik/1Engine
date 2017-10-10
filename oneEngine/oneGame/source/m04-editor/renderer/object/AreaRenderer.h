
#ifndef _2D_AREA_RENDERER_H_
#define _2D_AREA_RENDERER_H_

#include "renderer/object/CRenderableObject.h"
#include "renderer/object/shapes/CStreamedRenderable3D.h"

namespace Engine2D
{
	class Area2DBase;
}

namespace M04
{
	class AreaRenderer : public CRenderableObject
	{
	public:
		AreaRenderer ( void );
		~AreaRenderer( void );

		bool Render ( const char pass ) override;

	public:
		Engine2D::Area2DBase*	m_target_glow;
		Engine2D::Area2DBase*	m_target_selection;
		int					m_target_corner;
	};
}


#endif//_2D_AREA_RENDERER_H_