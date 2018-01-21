#include "CutsceneEditor.h"
#include "CutsceneEditor.Renderers.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/FileUtils.h"
#include "core/system/Screen.h"

#include "renderer/camera/CCamera.h"
#include "renderer/object/CRenderable3D.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"

#include "renderer/resource/CResourceManager.h"

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
	m_target_camera_position = Vector3d(0, 0, -10);
	m_preclude_navigation = false;
	m_navigation_busy = false;

	m_geometryRenderer		= new CGeometryRenderer(this);
	m_largeTextRenderer		= new CLargeTextRenderer(this);
	m_normalTextRenderer	= new CNormalTextRenderer(this);

	//m_nodes.push_back(EditorNode({NULL, Vector2d(0,0)}));
	//m_nodes.push_back(EditorNode({NULL, Vector2d(0,128)}));
}

CutsceneEditor::~CutsceneEditor ( void )
{
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

	m_geometryRenderer->UpdateMesh();
	m_largeTextRenderer->UpdateMesh();
	m_normalTextRenderer->UpdateMesh();
}


void CutsceneEditor::doViewNavigationDrag ( void )
{
	if ( m_preclude_navigation == false )
	{
		m_navigation_busy = false;
		// Drag navigation
		if ( Input::Mouse( Input::MBMiddle ) || ( Input::Mouse( Input::MBRight ) && Input::Key( Keys.Control ) ) )
		{
			m_navigation_busy = true;
			m_target_camera_position -= Vector2d(
				Input::DeltaMouseX(), Input::DeltaMouseY()
			);
		}
		if ( Input::DeltaMouseW() != 0 )
		{
			m_target_camera_position.y += Input::DeltaMouseW() * 0.05f;
		}
		// Navigation via arrow keys for bonus usability
		//if ( Input::Key( Keys.Left ) )  m_target_camera_position.x -= m_tilemap->m_tileset->tilesize_x * Time::deltaTime * 4.0F;
		//if ( Input::Key( Keys.Right ) ) m_target_camera_position.x += m_tilemap->m_tileset->tilesize_x * Time::deltaTime * 4.0F;
		//if ( Input::Key( Keys.Up ) )	m_target_camera_position.y -= m_tilemap->m_tileset->tilesize_y * Time::deltaTime * 4.0F;
		//if ( Input::Key( Keys.Down ) )	m_target_camera_position.y += m_tilemap->m_tileset->tilesize_y * Time::deltaTime * 4.0F;

		// Update camera position
		//m_target_camera->transform.position = m_target_camera_position;
	}
}

//		uiGetCurrentMouse() : Get vmouse position
Vector3d CutsceneEditor::uiGetCurrentMouse ( void )
{
	return Vector3d( Input::MouseX(), Input::MouseY(), 0.0F )
		+ m_target_camera_position
		- Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, 0.0F) * 0.5F;
}
//		uiGetNodeRect ( EditorNode* node ) : Gets node rect
Rect CutsceneEditor::uiGetNodeRect( common::cts::EditorNode* node )
{
	// For now, let's just use some constant size for all nodes!
	return Rect(node->position, Vector2d(128, 32));
}
//		uiGetNodeOutputPosition ( EditorNode* node, int index ) : Gets node output pos
Vector3d CutsceneEditor::uiGetNodeOutputPosition ( common::cts::EditorNode* node, const int index )
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

		return Vector2d(
			start_x + div_x * index,
			draw_rect.pos.y + draw_rect.size.y);
	}
	return Vector2d(center_x, draw_rect.pos.y + draw_rect.size.y);
}
//		uiGetNodeInputPosition ( EditorNode* node, int index ) : Gets node input pos
Vector3d CutsceneEditor::uiGetNodeInputPosition ( common::cts::EditorNode* node )
{
	const Real	kConnectorRadius	(8.0F);
	const Real	kConnectorSpacing	(12.0F);

	Rect draw_rect = uiGetNodeRect(node);
	Real center_x = draw_rect.pos.x + draw_rect.size.x * 0.5F;
	center_x = std::min(center_x, draw_rect.pos.x + 64.0F); // Limit center_x for hella wide nodes

	return Vector2d(center_x, draw_rect.pos.y);
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
		Vector3d l_mousepos = uiGetCurrentMouse();

		for (size_t i = 0; i < m_nodes.size(); ++i)
		{
			auto& node = m_nodes[i];

			Rect node_rect = uiGetNodeRect(&node);
			Rect expanded_node_rect = Rect(
				node_rect.pos - Vector2d(kConnectorRadius, kConnectorRadius),
				node_rect.size + Vector2d(kConnectorRadius, kConnectorRadius) * 2.0F);

			// Check if mosue in the area of the node
			if (expanded_node_rect.Contains(l_mousepos))
			{
				if (node.node != NULL)
				{
					// Check if mouse in the input connector
					Vector2d delta_input = uiGetNodeInputPosition(&node) - l_mousepos;
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
						Vector2d delta_output = uiGetNodeOutputPosition(&node, connecti) - l_mousepos;
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
		if ( m_mouseover_node && Input::MouseDown(Input::MBLeft) )
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
		if ( Input::MouseUp(Input::MBLeft) )
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
		if ( m_mouseover_connector_input && Input::MouseDown(Input::MBLeft) )
		{
			m_connecting_node = true;
			m_connecting_index_input = m_mouseover_index;
			m_connecting_index_output = kInvalidIndex;
			m_connecting_connectorindex = kInvalidIndex;
		}
		else if ( m_mouseover_connector_output && Input::MouseDown(Input::MBLeft) )
		{
			m_connecting_node = true;
			m_connecting_index_input = kInvalidIndex;
			m_connecting_index_output = m_mouseover_index;
			m_connecting_connectorindex = m_mouseover_connectorindex;
		}
	}
	else
	{
		if ( Input::MouseUp(Input::MBLeft) )
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
	if ( Input::MouseDown( Input::MBRight ) )
	{
		// Force mouseover update if pressing right click while menu is already up.
		if ( m_contextMenu_visible )
		{
			doEditorUpdateMouseOver();
		}

		// Stop dragging on right click
		m_dragging_node = false;
		m_connecting_node = false;

		// Set menu type
		if (!m_mouseover_node)
		{
			m_contextMenu_type = kContextMenuNewNode;
		}
		else
		{
			m_contextMenu_type = kContextMenuEditNode;
		}

		// Bring up the context menu
		m_contextMenu_visible = true;
		m_contextMenu_position = uiGetCurrentMouse();

		m_contextMenu_spacing = 18.0F;
		m_contextMenu_size = Vector3d(128, m_contextMenu_spacing * (common::cts::kNodeType_MAX - 1), 0.0F);
	}

	if ( m_contextMenu_visible )
	{
		if ( Input::MouseDown( Input::MBLeft ) )
		{
			Vector3d l_mousepos = uiGetCurrentMouse();

			if (Rect(m_contextMenu_position, m_contextMenu_size).Contains(l_mousepos))
			{
				// Otherwise get the index that was clicked
				int l_clickedindex = (int)((l_mousepos.y - m_contextMenu_position.y) / m_contextMenu_spacing);

				if ( m_contextMenu_type == kContextMenuNewNode )
				{
					// Get type of node clicked on
					common::cts::ENodeType l_nodeType = (common::cts::ENodeType)(l_clickedindex + 1);
				
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
				else if ( m_contextMenu_type == kContextMenuEditNode )
				{
					if ( l_clickedindex == 0 && m_mouseover_node )
					{
						common::cts::EditorNode to_remove = m_nodes[m_mouseover_index];
						m_nodes.erase(m_nodes.begin() + m_mouseover_index);
						delete to_remove.node;
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