#ifndef EDITOR_2D_AREA_RENDERER_H_
#define EDITOR_2D_AREA_RENDERER_H_

#include "renderer/object/CStreamedRenderable3D.h"

namespace Engine2D
{
	class Area2DBase;
}

namespace M04
{
	class AreaRenderer : public CStreamedRenderable3D
	{
	public:
								AreaRenderer ( void );
								~AreaRenderer( void );

		//	EndRender() : Post-render work.
		// Used to recreate the rendered mesh at the last minute.
		bool					EndRender ( void ) override;

	public:
		Engine2D::Area2DBase*
							m_target_glow;
		Engine2D::Area2DBase*
							m_target_selection;
		int					m_target_corner;
	};
}


#endif//EDITOR_2D_AREA_RENDERER_H_