#ifndef M04_EDITOR_CUTSCENE_EDITOR_H
#define M04_EDITOR_CUTSCENE_EDITOR_H

#include "core/math/Rect.h"
#include "engine/behavior/CGameBehavior.h"

#include <vector>

//=========================================//
// Prototypes
//=========================================//

namespace common
{
	namespace cts
	{
		class Node;
		class EditorNode;
	}
}

//=========================================//
// Class definition
//=========================================//

namespace M04
{
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

		//		doEditorUpdateMouseOver () : mouseover updates
		// Updates mouseover status & what is being moused over.
		// The editor is very context sensitive based on where the mouse is
		void		doEditorUpdateMouseOver ( void );

		//		doEditorDragNodes () : drag update (node)
		// Drags nodes around!
		void		doEditorDragNodes ( void );
		//		doEditorConnectNodes () : connect update
		// Connects nodes when you click on shit! Goddamn!
		void		doEditorConnectNodes ( void );

		//		doEditorContextMenu () : creates context menu
		// Updates and acts upon a right click context menu.
		void		doEditorContextMenu ( void );

		//		uiGetCurrentMouse() : Get vmouse position
		// Applies virtual screen offsets
		Vector3d	uiGetCurrentMouse ( void );
		//		uiGetNodeRect ( EditorNode* node ) : Gets node rect
		// Used for both display and hitbox.
		// Will take the node position into account
		Rect		uiGetNodeRect( common::cts::EditorNode* node );
		//		uiGetNodeOutputPosition ( EditorNode* node, int index ) : Gets node output pos
		// Used for both display and hitbox.
		// Will take the node position and rect into account.
		Vector3d	uiGetNodeOutputPosition ( common::cts::EditorNode* node, const int index );
		//		uiGetNodeInputPosition ( EditorNode* node, int index ) : Gets node input pos
		// Used for both display and hitbox.
		// Will take the node position and rect into account.
		Vector3d	uiGetNodeInputPosition ( common::cts::EditorNode* node );

		//		breakConnectionsTo ( EditorNode* node ) : Breaks all connections to this node.
		void		breakConnectionsTo( common::cts::EditorNode* node );
		//		breakConnectionFrom ( EditorNode* node, int index ) : Breaks one connections from this node.
		void		breakConnectionFrom( common::cts::EditorNode* node, const int index );
		//		breakAllConnectionsFrom ( EditorNode* node ) : Breaks all connections from this node.
		void		breakAllConnectionsFrom( common::cts::EditorNode* node );

	public:
		enum eContextMenu
		{
			kContextMenuNewNode,
			kContextMenuEditNode,
			kContextMenuConnectionInput,
			kContextMenuConnectionOutput,
		};
		struct grContextPair
		{
			arstring64 label;
			int value;

			grContextPair(const char* str, int val) 
				: label(str), value(val) {}
		};
		enum eContextValues : int
		{
			kContextValueDeleteNode,
			kContextValueBreakConnection,
		};

		static const size_t		kInvalidIndex = (size_t)(-1);

	private:
		Vector3d				m_target_camera_position;
		bool					m_preclude_navigation;
		bool					m_navigation_busy;

		bool					m_mouseover_node;
		bool					m_mouseover_connector_output;
		bool					m_mouseover_connector_input;
		size_t					m_mouseover_index; // node index
		size_t					m_mouseover_connectorindex;
		size_t					m_mouseover_context_prev;

		bool					m_dragging_node;
		size_t					m_dragging_index; // node index
		Vector3d				m_dragging_reference;
		Vector3d				m_dragging_startpos;

		bool					m_connecting_node;
		size_t					m_connecting_index_input; // node index
		size_t					m_connecting_index_output; // node index
		size_t					m_connecting_connectorindex;

		bool					m_contextMenu_visible;
		Vector3d				m_contextMenu_position;
		Vector3d				m_contextMenu_size;
		Real					m_contextMenu_spacing;
		eContextMenu			m_contextMenu_type;
		std::vector<grContextPair>
								m_contextMenu_entries;

		CLargeTextRenderer*		m_largeTextRenderer;
		CNormalTextRenderer*	m_normalTextRenderer;
		CGeometryRenderer*		m_geometryRenderer;

		std::vector<common::cts::EditorNode>	m_nodes;
	};
}

#endif//M04_EDITOR_CUTSCENE_EDITOR_H