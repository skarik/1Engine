#ifndef M04_EDITOR_CUTSCENE_EDITOR_H
#define M04_EDITOR_CUTSCENE_EDITOR_H

#include "engine/behavior/CGameBehavior.h"

#include <vector>

//=========================================//
// Prototypes
//=========================================//

namespace engine
{
	namespace cts
	{
		class Node;
	}
}

//=========================================//
// Class definition
//=========================================//

namespace M04
{
	struct EditorNode 
	{
		engine::cts::Node*	node;
		Vector2d			position;
	};

	class CutsceneEditor : public CGameBehavior
	{
		class CLargeTextRenderer;
		class CNormalTextRenderer;
		class CGeometryRenderer;

		friend CLargeTextRenderer;
		friend CNormalTextRenderer;
		friend CGeometryRenderer;

	public:
		explicit	CutsceneEditor ( void );
					~CutsceneEditor ( void );

		void		Update ( void ) override;

	private:
		//		doViewNavigationDrag () : view navigation
		// move the map around when middle button pressed
		void		doViewNavigationDrag ( void );

	private:
		Vector3d				m_target_camera_position;
		bool					m_preclude_navigation;
		bool					m_navigation_busy;

		CLargeTextRenderer*		m_largeTextRenderer;
		CNormalTextRenderer*	m_normalTextRenderer;

		std::vector<EditorNode>	m_nodes;
	};
}

#endif//M04_EDITOR_CUTSCENE_EDITOR_H