#ifndef EDITOR_2D_GIZMO_RENDERER_H_
#define EDITOR_2D_GIZMO_RENDERER_H_

#include "renderer/object/CStreamedRenderable3D.h"

namespace M04
{
	class EditorObject;
}

namespace M04
{
	class GizmoRenderer : public CStreamedRenderable3D
	{
	public:
								GizmoRenderer ( void );
								~GizmoRenderer( void );

		bool					EndRender ( void ) override;

	public:
		EditorObject*		m_target_glow;
		EditorObject*		m_target_selection;
	};
}


#endif//EDITOR_2D_GIZMO_RENDERER_H_