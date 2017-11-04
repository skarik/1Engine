#ifndef RENDERER_C_DEBUG_DRAWER_H_
#define RENDERER_C_DEBUG_DRAWER_H_

#include "core/math/Ray.h"
#include "core/math/Line.h"
#include "core/math/Color.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include <vector>

namespace debug
{
	class CDebugDrawer : public CStreamedRenderable3D
	{
	public:
		explicit	CDebugDrawer ( void );
					~CDebugDrawer ( void );

		bool		PreRender ( void ) override;

	public:
		RENDER_API static void DrawLine ( const Line&, const Color& = Color(1,1,0,1) );
		RENDER_API static void DrawLine ( const Vector3d&, const Vector3d&, const Color& = Color(1,1,0,1) );
		RENDER_API static void DrawRay ( const Ray&, const Color& = Color(1,1,0,1) );

	private:
		std::vector<Line>  avLineList;
		std::vector<Color> avColorList;

		std::vector<arModelVertex>		m_vertices;
		std::vector<arModelTriangle>	m_tris;

		static CDebugDrawer* mActive;
	};

	// Global pointer to the current DebugDrawer instance.
	// Totally usable for visual debugging anywhere in 1Engine!
	RENDER_API extern CDebugDrawer* Drawer;
};

#endif//RENDERER_C_DEBUG_DRAWER_H_
