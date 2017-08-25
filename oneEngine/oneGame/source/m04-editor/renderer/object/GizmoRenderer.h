
#ifndef _2D_GIZMO_RENDERER_H_
#define _2D_GIZMO_RENDERER_H_

#include "renderer/object/CRenderableObject.h"

namespace M04
{
	class EditorObject;
}

namespace M04
{
	class GizmoRenderer : public CRenderableObject
	{
	public:
		GizmoRenderer ( void );
		~GizmoRenderer( void );

		bool Render ( const char pass ) override;

	public:
		EditorObject*	m_target_glow;
		EditorObject*	m_target_selection;
	};
}


#endif//_2D_AREA_RENDERER_H_