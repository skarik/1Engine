#include "CutsceneEditor.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/FileUtils.h"

#include "core/system/Screen.h"

#include "engine/cutscene/Node.h"
#include "engine/cutscene/NodeChoicebox.h"
#include "engine/cutscene/NodeEnd.h"
#include "engine/cutscene/NodeMoveCharacterM04.h"
#include "engine/cutscene/NodeTalkbox.h"
#include "engine/cutscene/NodeWait.h"

#include "renderer/camera/CCamera.h"

#include "renderer/object/CRenderable3D.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/RrMaterial.h"

#include "renderer/resource/CResourceManager.h"

#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"

using namespace M04;

static const char* _GetNodeEnumString ( engine::cts::eNodeType node_type )
{
	switch (node_type)
	{
	case engine::cts::kNodeTypeTalkbox:
		return "Talkbox";
	case engine::cts::kNodeTypeChoicebox:
		return "Choicebox";
	case engine::cts::kNodeTypeWait:
		return "Wait";
	case engine::cts::kNodeTypeMoveCharacterM04:
		return "Move Character (M04)";
	case engine::cts::kNodeTypeEnd:
		return "End";

	default: return "Invalid";
	}
}

static Vector2d _PixelRoundPosition ( const Vector2d vect )
{
	return Vector2d(
		1.0F * math::round( vect.x ),
		1.0F * math::round( vect.y )
	);
}

class CutsceneEditor::CLargeTextRenderer : public CStreamedRenderable3D
{
private:
	CutsceneEditor*		m_owner;
	CBitmapFont*		m_font_texture;

public:
	explicit CLargeTextRenderer ( CutsceneEditor* owner )
		: m_owner(owner), CStreamedRenderable3D()
	{
		m_font_texture = new CBitmapFont("YanoneKaffeesatz-R.otf", 24, FW_BOLD);
		m_font_texture->SetFilter( SamplingLinear );

		// Use a default 2D material
		m_material = new RrMaterial();
		m_material->setTexture( TEX_DIFFUSE, core::Orphan(new CTexture("null")) );
		m_material->setTexture( TEX_SURFACE, renderer::Resources::GetTexture(renderer::TextureBlack) );

		m_material->passinfo.push_back( RrPassForward() );
		m_material->passinfo[0].shader = new RrShader( "shaders/fws/fullbright.glsl" );
		m_material->passinfo[0].m_lighting_mode = renderer::LI_NONE;
		m_material->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
		m_material->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;

		m_material->setTexture( TEX_DIFFUSE, m_font_texture );
		m_font_texture->RemoveReference();

		m_modeldata.triangles = new arModelTriangle [2048];
		m_modeldata.vertices = new arModelVertex [4096];
	}
	~CLargeTextRenderer ( void )
		{ }

	void UpdateMesh ( void )
	{
		// Set up screenmap
		core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
			Vector3f(-1.0F, -1.0F, -1.0F),
			Vector3f(+1.0F, +1.0F, +1.0F)
		);
		screenMapping.position.y = 1.0F;
		screenMapping.size.y = -2.0F;

		if ( CCamera::activeCamera )
		{	// Modify console size based on render scale so it is always legible!
			screenMapping.position += screenMapping.size * 0.5F;
			screenMapping.size *= CCamera::activeCamera->render_scale;
			screenMapping.position -= screenMapping.size * 0.5F;
		}

		// Create builder
		rrTextBuilder2D builder (m_font_texture, screenMapping, &m_modeldata);
		m_modeldata.triangleNum = 0;
		m_modeldata.vertexNum = 0;

		// Create the visual text
		Vector2d screenOrigin (-640,-360 + 20);
		Vector2d generalMargins (4, 4);

		builder.addText(
			screenOrigin + Vector2d(10,10),
			Color(1.0F, 1.0F, 1.0F, 1.0F),
			"Cutscene Editor" );

		// Now with the mesh built, push it to the modeldata :)
		this->StreamLockModelData();
	}
};

class CutsceneEditor::CNormalTextRenderer : public CStreamedRenderable3D
{
private:
	CutsceneEditor*		m_owner;
	CBitmapFont*		m_font_texture;

public:
	explicit CNormalTextRenderer ( CutsceneEditor* owner )
		: m_owner(owner), CStreamedRenderable3D()
	{
		m_font_texture = new CBitmapFont("YanoneKaffeesatz-R.otf", 18, FW_BOLD);
		m_font_texture->SetFilter( SamplingLinear );

		// Use a default 2D material
		m_material = new RrMaterial();
		m_material->setTexture( TEX_DIFFUSE, core::Orphan(new CTexture("null")) );
		m_material->setTexture( TEX_SURFACE, renderer::Resources::GetTexture(renderer::TextureBlack) );

		m_material->passinfo.push_back( RrPassForward() );
		m_material->passinfo[0].shader = new RrShader( "shaders/fws/fullbright.glsl" );
		m_material->passinfo[0].m_lighting_mode = renderer::LI_NONE;
		m_material->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
		m_material->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;

		m_material->setTexture( TEX_DIFFUSE, m_font_texture );
		m_font_texture->RemoveReference();

		m_modeldata.triangles = new arModelTriangle [2048];
		m_modeldata.vertices = new arModelVertex [4096];
	}
	~CNormalTextRenderer ( void )
		{ }

	void UpdateMesh ( void )
	{
		// Set up screenmap
		core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
			Vector3f(-1.0F, -1.0F, -1.0F),
			Vector3f(+1.0F, +1.0F, +1.0F)
		);
		screenMapping.position.y = 1.0F;
		screenMapping.size.y = -2.0F;

		if ( CCamera::activeCamera )
		{	// Modify console size based on render scale so it is always legible!
			screenMapping.position += screenMapping.size * 0.5F;
			screenMapping.size *= CCamera::activeCamera->render_scale;
			screenMapping.position -= screenMapping.size * 0.5F;
		}

		// Create builder
		rrTextBuilder2D builder (m_font_texture, screenMapping, &m_modeldata);
		m_modeldata.triangleNum = 0;
		m_modeldata.vertexNum = 0;

		// Create the visual text
		{
			Vector2d screenOrigin (-640,-360 + 20);
			Vector2d generalMargins (4, 4);

			builder.addText(
				screenOrigin + Vector2d(10, 40),
				Color(1.0F, 1.0F, 1.0F, 1.0F),
				"node count: %d" );

			// Loop through all the nodes
			for (auto& nodes : m_owner->m_nodes)
			{
				engine::cts::eNodeType nodeType = engine::cts::kNodeType_INVALID;
				if (nodes.node != NULL)
				{
					nodeType = nodes.node->GetNodeType();
				}
				const char* l_strNodeType = _GetNodeEnumString(nodeType);

				// Display node type
				builder.addText(
					_PixelRoundPosition(generalMargins + nodes.position - m_owner->m_target_camera_position),
					Color(1.0F, 1.0F, 1.0F, 1.0F),
					l_strNodeType );
			}
		}

		// Draw the context menu
		if ( m_owner->m_contextMenu_visible )
		{
			if ( m_owner->m_contextMenu_type == kContextMenuNewNode )
			{
				for ( int i = 1; i < engine::cts::kNodeType_MAX; ++i )
				{
					builder.addText(_PixelRoundPosition(
						m_owner->m_contextMenu_position
						+ Vector2d(0, m_owner->m_contextMenu_spacing + m_owner->m_contextMenu_spacing * (i-1))
						- m_owner->m_target_camera_position),
						Color(1.0F, 1.0F, 1.0F, 1.0F),
						_GetNodeEnumString((engine::cts::eNodeType)i));
				}
			}
			else if ( m_owner->m_contextMenu_type == kContextMenuEditNode )
			{
				for ( int i = 0; i < 1; ++i )
				{
					builder.addText(_PixelRoundPosition(
						m_owner->m_contextMenu_position
						+ Vector2d(0, m_owner->m_contextMenu_spacing + m_owner->m_contextMenu_spacing * i)
						- m_owner->m_target_camera_position),
						Color(1.0F, 1.0F, 1.0F, 1.0F),
						"Delete Node");
				}
			}
		}

		// Now with the mesh built, push it to the modeldata :)
		this->StreamLockModelData();
	}
};


CutsceneEditor::CutsceneEditor ( void )
	: CGameBehavior()
{
	m_target_camera_position = Vector3d(0, 0, -10);
	m_preclude_navigation = false;
	m_navigation_busy = false;

	m_largeTextRenderer = new CLargeTextRenderer(this);
	m_normalTextRenderer = new CNormalTextRenderer(this);

	//m_nodes.push_back(EditorNode({NULL, Vector2d(0,0)}));
	//m_nodes.push_back(EditorNode({NULL, Vector2d(0,128)}));
}

CutsceneEditor::~CutsceneEditor ( void )
{
	delete_safe(m_largeTextRenderer);
	delete_safe(m_normalTextRenderer);
}

void CutsceneEditor::Update ( void )
{
	doViewNavigationDrag();
	doEditorUpdateMouseOver(); // the following rely on the mouseover state
	doEditorDragNodes();
	doEditorContextMenu();

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

Vector3d CutsceneEditor::uiGetCurrentMouse ( void )
{
	return Vector3d( Input::MouseX(), Input::MouseY(), 0.0F )
		+ m_target_camera_position
		- Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, 0.0F) * 0.5F;
}

void CutsceneEditor::doEditorUpdateMouseOver ( void )
{
	if ( m_contextMenu_visible == false )
	{
		// Reset node state
		m_mouseover_node = false;

		// Perform mousepos checks
		Vector3d l_mousepos = uiGetCurrentMouse();

		for (size_t i = 0; i < m_nodes.size(); ++i)
		{
			// Check if mouse is in the node
			if (Rect(m_nodes[i].position, Vector2d(128, 32)).Contains(l_mousepos))
			{
				// We have found a node!
				m_mouseover_node = true;
				m_mouseover_index = i;
				break;
			}
		}
	}
}

void CutsceneEditor::doEditorDragNodes ( void )
{
	if ( m_contextMenu_visible ) return;

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
		m_contextMenu_size = Vector3d(128, m_contextMenu_spacing * (engine::cts::kNodeType_MAX - 1), 0.0F);
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
					engine::cts::eNodeType l_nodeType = (engine::cts::eNodeType)(l_clickedindex + 1);
				
					EditorNode newnode = {NULL, m_contextMenu_position};

					switch (l_nodeType)
					{
					case engine::cts::kNodeTypeTalkbox:
						newnode.node = new engine::cts::NodeTalkbox();
						break;
					case engine::cts::kNodeTypeChoicebox:
						newnode.node = new engine::cts::NodeChoicebox();
						break;
					case engine::cts::kNodeTypeWait:
						newnode.node = new engine::cts::NodeWait();
						break;
					case engine::cts::kNodeTypeMoveCharacterM04:
						newnode.node = new engine::cts::NodeMoveCharacterM04();
						break;
					case engine::cts::kNodeTypeEnd:
						newnode.node = new engine::cts::NodeEnd();
						break;
					}
					m_nodes.push_back(newnode);
				}
				else if ( m_contextMenu_type == kContextMenuEditNode )
				{
					if ( l_clickedindex == 0 && m_mouseover_node )
					{
						EditorNode to_remove = m_nodes[m_mouseover_index];
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