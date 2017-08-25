
#ifndef _C_DEBUG_DRAWER_H_
#define _C_DEBUG_DRAWER_H_

// Includes
#include <vector>

#include "renderer/object/CRenderableObject.h"
struct Line;
struct Ray;

namespace debug
{
	// Class Definition
	class CDebugDrawer : public CRenderableObject
	{
	public:
		// Constructor and destructor
		explicit	CDebugDrawer ( void );
					~CDebugDrawer ( void );
	public:
		// Drawing
		bool Render ( const char pass );

		bool EndRender ( void );

	public:
		RENDER_API static void DrawLine ( const Line&, const Color& = Color(1,1,0,1) );
		RENDER_API static void DrawLine ( const Vector3d&, const Vector3d&, const Color& = Color(1,1,0,1) );
		RENDER_API static void DrawRay ( const Ray&, const Color& = Color(1,1,0,1) );

	private:
		std::vector<Line>  avLineList;
		std::vector<Color> avColorList;

		static CDebugDrawer* mActive;
	};

	// Typedef for ease of use
	//typedef CDebugDrawer DebugD;
	RENDER_API extern CDebugDrawer* Drawer;

};

#endif
