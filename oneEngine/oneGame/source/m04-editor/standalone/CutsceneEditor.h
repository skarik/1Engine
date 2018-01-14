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

		void		doEditorUpdateMouseOver ( void );

		void		doEditorDragNodes ( void );

		void		doEditorContextMenu ( void );

		//		uiGetCurrentMouse() : Get vmouse position
		// Applies virtual screen offsets
		Vector3d	uiGetCurrentMouse ( void );

	public:
		enum eContextMenu
		{
			kContextMenuNewNode,
			kContextMenuEditNode,
		};

	private:
		Vector3d				m_target_camera_position;
		bool					m_preclude_navigation;
		bool					m_navigation_busy;

		bool					m_mouseover_node;
		size_t					m_mouseover_index;

		bool					m_dragging_node;
		size_t					m_dragging_index;
		Vector3d				m_dragging_reference;
		Vector3d				m_dragging_startpos;

		bool					m_contextMenu_visible;
		Vector3d				m_contextMenu_position;
		Vector3d				m_contextMenu_size;
		Real					m_contextMenu_spacing;
		eContextMenu			m_contextMenu_type;

		CLargeTextRenderer*		m_largeTextRenderer;
		CNormalTextRenderer*	m_normalTextRenderer;

		std::vector<EditorNode>	m_nodes;
	};
}

#endif//M04_EDITOR_CUTSCENE_EDITOR_H