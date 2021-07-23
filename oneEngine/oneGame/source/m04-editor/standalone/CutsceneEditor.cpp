#include "CutsceneEditor.h"
#include "CutsceneEditor.Renderers.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/FileUtils.h"
#include "core/system/Screen.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/object/CRenderable3D.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/texture/RrFontTexture.h"
#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"

//#include "renderer/resource/CResourceManager.h"

#include "engine-common/cutscene/EditorNode.h"
#include "engine-common/cutscene/Node.h"

// Include all the needed node types:

#include "engine-common/cutscene/NodeStart.h"
#include "engine-common/cutscene/NodeChoicebox.h"
#include "engine-common/cutscene/NodeEnd.h"
#include "engine-common/cutscene/NodeMoveCharacterM04.h"
#include "engine-common/cutscene/NodeTalkbox.h"
#include "engine-common/cutscene/NodeWait.h"

using namespace M04;

CutsceneEditor::CutsceneEditor ( void )
	: CGameBehavior()
{
	m_target_camera_position = Vector3f(0, 0, -10);
	m_preclude_navigation = false;
	m_navigation_busy = false;

	m_geometryRenderer		= new CGeometryRenderer(this);
	m_largeTextRenderer		= new CLargeTextRenderer(this);
	m_normalTextRenderer	= new CNormalTextRenderer(this);

	m_geometryUpdater		= new CGeometryUpdater(this);

	//m_nodes.push_back(EditorNode({NULL, Vector2f(0,0)}));
	//m_nodes.push_back(EditorNode({NULL, Vector2f(0,128)}));
}

CutsceneEditor::~CutsceneEditor ( void )
{
	delete_safe(m_geometryUpdater);
	delete_safe(m_geometryRenderer);
	delete_safe(m_largeTextRenderer);
	delete_safe(m_normalTextRenderer);
}

void CutsceneEditor::Update ( void )
{
	doViewNavigationDrag();
	doEditorUpdateMouseOver(); // the following rely on the mouseover state
	doEditorDragNodes();
	doEditorConnectNodes();
	doEditorContextMenu();

	/*m_geometryRenderer->UpdateMesh();
	m_largeTextRenderer->UpdateMesh();
	m_normalTextRenderer->UpdateMesh();*/
}


void CutsceneEditor::doViewNavigationDrag ( void )
{
	if ( m_preclude_navigation == false )
	{
		m_navigation_busy = false;
		// Drag navigation
		if ( core::Input::Mouse( core::kMBMiddle ) || ( core::Input::Mouse( core::kMBRight ) && core::Input::Key( core::kVkControl ) ) )
		{
			m_navigation_busy = true;
			m_target_camera_position -= Vector2f(
				core::Input::DeltaMouseX(), core::Input::DeltaMouseY()
			);
		}
		if ( core::Input::DeltaMouseScroll() != 0 )
		{
			m_target_camera_position.y += core::Input::DeltaMouseScroll() * 0.05f;
		}
		// Navigation via arrow keys for bonus usability
		//if ( Input::Key( core::kVkLeft ) )  m_target_camera_position.x -= m_tilemap->m_tileset->tilesize_x * Time::deltaTime * 4.0F;
		//if ( Input::Key( core::kVkRight ) ) m_target_camera_position.x += m_tilemap->m_tileset->tilesize_x * Time::deltaTime * 4.0F;
		//if ( Input::Key( core::kVkUp ) )	m_target_camera_position.y -= m_tilemap->m_tileset->tilesize_y * Time::deltaTime * 4.0F;
		//if ( Input::Key( core::kVkDown ) )	m_target_camera_position.y += m_tilemap->m_tileset->tilesize_y * Time::deltaTime * 4.0F;

		// Update camera position
		//m_target_camera->transform.position = m_target_camera_position;
	}
}

//		uiGetCurrentMouse() : Get vmouse position
Vector3f CutsceneEditor::uiGetCurrentMouse ( void )
{
	return Vector3f( core::Input::MouseX(), core::Input::MouseY(), 0.0F )
		+ m_target_camera_position
		- Vector3f((Real)Screen::Info.width, (Real)Screen::Info.height, 0.0F) * 0.5F;
}
//		uiGetNodeRect ( EditorNode* node ) : Gets node rect
Rect CutsceneEditor::uiGetNodeRect( common::cts::EditorNode* node )
{
	// For now, let's just use some constant size for all nodes!
	return Rect(node->position, Vector2f(128, 32));
}
//		uiGetNodeOutputPosition ( EditorNode* node, int index ) : Gets node output pos
Vector3f CutsceneEditor::uiGetNodeOutputPosition ( common::cts::EditorNode* node, const int index )
{
	const Real	kConnectorRadius	(8.0F);
	const Real	kConnectorSpacing	(12.0F);

	Rect draw_rect = uiGetNodeRect(node);
	Real center_x = draw_rect.pos.x + draw_rect.size.x * 0.5F;
	center_x = std::min(center_x, draw_rect.pos.x + 64.0F); // Limit center_x for hella wide nodes

	if (node->node != NULL)
	{
		const int connect_count = node->node->GetOutputNodeCount();
		const Real div_x = (kConnectorRadius * 2.0F) + kConnectorSpacing;
		const Real start_x = center_x - div_x * ((Real)connect_count - 1.0F) * 0.5F;

		return Vector2f(
			start_x + div_x * index,
			draw_rect.pos.y + draw_rect.size.y);
	}
	return Vector2f(center_x, draw_rect.pos.y + draw_rect.size.y);
}
//		uiGetNodeInputPosition ( EditorNode* node, int index ) : Gets node input pos
Vector3f CutsceneEditor::uiGetNodeInputPosition ( common::cts::EditorNode* node )
{
	const Real	kConnectorRadius	(8.0F);
	const Real	kConnectorSpacing	(12.0F);

	Rect draw_rect = uiGetNodeRect(node);
	Real center_x = draw_rect.pos.x + draw_rect.size.x * 0.5F;
	center_x = std::min(center_x, draw_rect.pos.x + 64.0F); // Limit center_x for hella wide nodes

	return Vector2f(center_x, draw_rect.pos.y);
}


void CutsceneEditor::doEditorUpdateMouseOver ( void )
{
	const Real	kConnectorRadius	(8.0F);

	if ( m_contextMenu_visible == false )
	{
		// Reset node state
		m_mouseover_node = false;
		m_mouseover_connector_input = false;
		m_mouseover_connector_output = false;

		// Perform mousepos checks
		Vector3f l_mousepos = uiGetCurrentMouse();

		for (size_t i = 0; i < m_nodes.size(); ++i)
		{
			auto& node = m_nodes[i];

			Rect node_rect = uiGetNodeRect(&node);
			Rect expanded_node_rect = Rect(
				node_rect.pos - Vector2f(kConnectorRadius, kConnectorRadius),
				node_rect.size + Vector2f(kConnectorRadius, kConnectorRadius) * 2.0F);

			// Check if mosue in the area of the node
			if (expanded_node_rect.Contains(l_mousepos))
			{
				if (node.node != NULL)
				{
					// Check if mouse in the input connector
					Vector2f delta_input = uiGetNodeInputPosition(&node) - l_mousepos;
					if (node.node->GetNodeType() != common::cts::kNodeTypeStart
						&& (delta_input).sqrMagnitude() < kConnectorRadius * kConnectorRadius)
					{
						m_mouseover_connector_input = true;
						m_mouseover_index = i;
						break;
					}
					// Check if mouse in any of the output connectors
					const int connect_count = node.node->GetOutputNodeCount();
					for (int connecti = 0; connecti < connect_count; ++connecti)
					{
						Vector2f delta_output = uiGetNodeOutputPosition(&node, connecti) - l_mousepos;
						if ((delta_output).sqrMagnitude() < kConnectorRadius * kConnectorRadius)
						{
							m_mouseover_connector_output = true;
							m_mouseover_connectorindex = (size_t)connecti;
							m_mouseover_index = i;
							break;
						}
					}
					if (m_mouseover_connector_output == true) break;
				}
				// Check if mouse is in the node
				if (node_rect.Contains(l_mousepos))
				{
					// We have found a node!
					m_mouseover_node = true;
					m_mouseover_index = i;
					break;
				}
			}
		}
	}
}

//		doEditorDragNodes () : drag update (node)
void CutsceneEditor::doEditorDragNodes ( void )
{
	if ( m_contextMenu_visible ) return;
	if ( m_connecting_node ) return;

	if ( !m_dragging_node )
	{
		// Check for begin drag
		if ( m_mouseover_node && core::Input::MouseDown(core::kMBLeft) )
		{
			m_dragging_node = true;
			m_dragging_index = m_mouseover_index;
			m_dragging_reference = uiGetCurrentMouse();
			m_dragging_startpos = m_nodes[m_dragging_index].position;
		}
	}
	else
	{
		// Update drag
		m_nodes[m_dragging_index].position = 
			m_dragging_startpos + uiGetCurrentMouse() - m_dragging_reference;

		// Release drag
		if ( core::Input::MouseUp(core::kMBLeft) )
		{
			m_dragging_node = false;
		}
	}
}

//		doEditorConnectNodes () : connect update
void CutsceneEditor::doEditorConnectNodes ( void )
{
	if ( m_contextMenu_visible ) return;
	if ( m_dragging_node ) return;

	if ( !m_connecting_node )
	{
		// Check for begin drag connection
		if ( m_mouseover_connector_input && core::Input::MouseDown(core::kMBLeft) )
		{
			m_connecting_node = true;
			m_connecting_index_input = m_mouseover_index;
			m_connecting_index_output = kInvalidIndex;
			m_connecting_connectorindex = kInvalidIndex;
		}
		else if ( m_mouseover_connector_output && core::Input::MouseDown(core::kMBLeft) )
		{
			m_connecting_node = true;
			m_connecting_index_input = kInvalidIndex;
			m_connecting_index_output = m_mouseover_index;
			m_connecting_connectorindex = m_mouseover_connectorindex;
		}
	}
	else
	{
		if ( core::Input::MouseUp(core::kMBLeft) )
		{
			bool valid_make = false;

			// If mouse up, check if released over the proper node type
			if ( m_connecting_index_input == kInvalidIndex && m_mouseover_connector_input )
			{
				m_connecting_index_input = m_mouseover_index;
				valid_make = true;
			}
			else if ( m_connecting_index_output == kInvalidIndex && m_mouseover_connector_output )
			{
				m_connecting_index_output = m_mouseover_index;
				m_connecting_connectorindex = m_mouseover_connectorindex;
				valid_make = true;
			}

			// If valid make, make a connection!
			if (valid_make)
			{
				// Need to work with the output node (we have 1-way linked lists)
				auto& start_node = m_nodes[m_connecting_index_output];
				auto& end_node = m_nodes[m_connecting_index_input];

				// Make the new connection
				if (start_node.node != NULL && end_node.node != NULL && start_node.node != end_node.node)
				{
					start_node.node->IOSetOutputNode((int)m_connecting_connectorindex, end_node.node);
				}
			}

			// End connecting mode
			m_connecting_node = false;
		}
	}
}

void CutsceneEditor::doEditorContextMenu ( void )
{
	if ( core::Input::MouseDown( core::kMBRight ) )
	{
		// Force mouseover update if pressing right click while menu is already up.
		if ( m_contextMenu_visible )
		{
			doEditorUpdateMouseOver();
		}

		// Stop dragging on right click
		m_dragging_node = false;
		m_connecting_node = false;

		// Set menu type & build menu
		m_contextMenu_entries.clear();
		if (m_mouseover_connector_input)
		{
			m_contextMenu_type = kContextMenuConnectionInput;
			m_contextMenu_entries.push_back(grContextPair("Break All", kContextValueBreakConnection));
		}
		else if (m_mouseover_connector_output)
		{
			m_contextMenu_type = kContextMenuConnectionOutput;
			m_contextMenu_entries.push_back(grContextPair("Break", kContextValueBreakConnection));
		}
		else if (m_mouseover_node)
		{
			m_contextMenu_type = kContextMenuEditNode;
			m_contextMenu_entries.push_back(grContextPair("Delete Node", kContextValueDeleteNode));
			m_contextMenu_entries.push_back(grContextPair("Break All", kContextValueBreakConnection));
		}
		else
		{
			m_contextMenu_type = kContextMenuNewNode;
			for ( int i = 1; i < common::cts::kNodeType_MAX; ++i )
			{
				m_contextMenu_entries.push_back(grContextPair(common::cts::kNodeTypeInfo[(common::cts::ENodeType)i].readable, i));
			}
		}

		// Bring up the context menu
		m_contextMenu_visible = true;
		m_contextMenu_position = uiGetCurrentMouse();

		m_contextMenu_spacing = 18.0F;
		m_contextMenu_size = Vector3f(128, m_contextMenu_spacing * (common::cts::kNodeType_MAX - 1), 0.0F);
	}

	if ( m_contextMenu_visible )
	{
		// Update the clicked index:
		Vector3f l_mousepos = uiGetCurrentMouse();
		// Get the index that was clicked
		int l_clickedindex = (int)((l_mousepos.y - m_contextMenu_position.y) / m_contextMenu_spacing);
	
		// Update the mouseover the for menu
		if (l_clickedindex >= 0 &&
			l_clickedindex < (int)m_contextMenu_entries.size())
		{
			m_mouseover_context_prev = (size_t)l_clickedindex;
		}
		else
		{
			m_mouseover_context_prev = kInvalidIndex;
		}

		if ( core::Input::MouseDown( core::kMBLeft ) )
		{
			// Is the mouse even in range?
			if (m_mouseover_context_prev != kInvalidIndex
				&& Rect(m_contextMenu_position, m_contextMenu_size).Contains(l_mousepos))
			{
				// Perform menu logic:
				if ( m_contextMenu_type == kContextMenuNewNode )
				{
					// Get type of node clicked on
					common::cts::ENodeType l_nodeType = (common::cts::ENodeType)(m_contextMenu_entries[l_clickedindex].value);
				
					common::cts::EditorNode newnode = {NULL, m_contextMenu_position};

					switch (l_nodeType)
					{
					case common::cts::kNodeTypeStart:
						newnode.node = new common::cts::NodeStart();
						break;
					case common::cts::kNodeTypeTalkbox:
						newnode.node = new common::cts::NodeTalkbox();
						break;
					case common::cts::kNodeTypeChoicebox:
						newnode.node = new common::cts::NodeChoicebox();
						break;
					case common::cts::kNodeTypeWait:
						newnode.node = new common::cts::NodeWait();
						break;
					case common::cts::kNodeTypeMoveCharacterM04:
						newnode.node = new common::cts::NodeMoveCharacterM04();
						break;
					case common::cts::kNodeTypeEnd:
						newnode.node = new common::cts::NodeEnd();
						break;
					}
					m_nodes.push_back(newnode);
				}
				else if ( m_contextMenu_type == kContextMenuEditNode && m_mouseover_node )
				{
					int menu_value = m_contextMenu_entries[l_clickedindex].value;
					if ( menu_value == kContextValueDeleteNode )
					{
						common::cts::EditorNode to_remove = m_nodes[m_mouseover_index];
						m_nodes.erase(m_nodes.begin() + m_mouseover_index);
						delete to_remove.node;
					}
					else if ( menu_value == kContextValueBreakConnection )
					{
						breakAllConnectionsFrom(&m_nodes[m_mouseover_index]);
						breakConnectionsTo(&m_nodes[m_mouseover_index]);
					}
				}
				else if ( m_contextMenu_type == kContextMenuConnectionInput )
				{
					int menu_value = m_contextMenu_entries[l_clickedindex].value;
					if ( menu_value == kContextValueBreakConnection && m_mouseover_connector_input )
					{
						breakConnectionsTo(&m_nodes[m_mouseover_index]);
					}
				}
				else if ( m_contextMenu_type == kContextMenuConnectionOutput )
				{
					int menu_value = m_contextMenu_entries[l_clickedindex].value;
					if ( menu_value == kContextValueBreakConnection && m_mouseover_connector_output )
					{
						breakConnectionFrom(&m_nodes[m_mouseover_index], (int)m_mouseover_connectorindex);
					}
				}

				// Hide menu after click
				m_contextMenu_visible = false;	
			}
			else
			{
				// Hide menu if click outside
				m_contextMenu_visible = false;
			}
		}
	}
}


//		breakConnectionsTo ( EditorNode* node ) : Breaks all connections to this node.
void CutsceneEditor::breakConnectionsTo( common::cts::EditorNode* node )
{
	// Don't waste time on invalid connection breaking.
	if (node->node == NULL)
	{
		return;
	}

	// Loop through all the editor nodes:
	for (auto& node_itr : m_nodes)
	{
		if (node_itr.node != NULL)
		{
			const int connect_count = node_itr.node->GetOutputNodeCount();
			for (int connecti = 0; connecti < connect_count; ++connecti)
			{
				// Get the node we're connected to:
				if (node_itr.node->GetOutputNode(connecti) == node->node)
				{	// If it's the one we're trying to orphan, then orphan it!
					node_itr.node->IOSetOutputNode(connecti, NULL);
				}
			}
		}
	}
}
//		breakConnectionFrom ( EditorNode* node, int index ) : Breaks one connections from this node.
void CutsceneEditor::breakConnectionFrom( common::cts::EditorNode* node, const int index )
{
	// Don't waste time on invalid connection breaking.
	if (node->node != NULL)
	{
		node->node->IOSetOutputNode(index, NULL);
	}
}
//		breakConnectionsFrom ( EditorNode* node ) : Breaks all connections from this node.
void CutsceneEditor::breakAllConnectionsFrom( common::cts::EditorNode* node )
{
	// Don't waste time on invalid connection breaking.
	if (node->node != NULL)
	{
		const int connect_count = node->node->GetOutputNodeCount();
		for (int connecti = 0; connecti < connect_count; ++connecti)
		{
			node->node->IOSetOutputNode(connecti, NULL);
		}
	}
}