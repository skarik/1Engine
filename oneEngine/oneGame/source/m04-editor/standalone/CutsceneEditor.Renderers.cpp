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

static Vector2d _PixelRoundPosition ( const Vector2d vect )
{
	return Vector2d(
		1.0F * math::round( vect.x ),
		1.0F * math::round( vect.y )
	);
}

CutsceneEditor::CLargeTextRenderer::CLargeTextRenderer ( CutsceneEditor* owner )
	: m_owner(owner), CStreamedRenderable3D()
{
	m_font_texture = new CBitmapFont("YanoneKaffeesatz-B.otf", 24, FW_BOLD);
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
CutsceneEditor::CLargeTextRenderer::~CLargeTextRenderer ( void )
{ }

void CutsceneEditor::CLargeTextRenderer::UpdateMesh ( void )
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


CutsceneEditor::CNormalTextRenderer::CNormalTextRenderer ( CutsceneEditor* owner )
		: m_owner(owner), CStreamedRenderable3D()
{
	m_font_texture = new CBitmapFont("YanoneKaffeesatz-B.otf", 18, FW_BOLD);
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
CutsceneEditor::CNormalTextRenderer::~CNormalTextRenderer ( void )
{ }

void CutsceneEditor::CNormalTextRenderer::UpdateMesh ( void )
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
			common::cts::ENodeType nodeType = common::cts::kNodeType_INVALID;
			if (nodes.node != NULL)
			{
				nodeType = nodes.node->GetNodeType();
			}
			const char* l_strNodeType = common::cts::kNodeTypeInfo[nodeType].readable;

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
		for ( size_t i = 0; i < m_owner->m_contextMenu_entries.size(); ++i )
		{
			builder.addText(_PixelRoundPosition(
				m_owner->m_contextMenu_position
				+ Vector2d(0, m_owner->m_contextMenu_spacing + m_owner->m_contextMenu_spacing * i)
				- m_owner->m_target_camera_position),
				(i == m_owner->m_mouseover_context_prev) ? Color(1.0F, 1.0F, 1.0F, 1.0F) : Color(0.5F, 0.5F, 0.5F, 1.0F),
				m_owner->m_contextMenu_entries[i].label.c_str());
		}
	}

	// Now with the mesh built, push it to the modeldata :)
	this->StreamLockModelData();
}




CutsceneEditor::CGeometryRenderer::CGeometryRenderer ( CutsceneEditor* owner )
	: m_owner(owner), CStreamedRenderable3D()
{
	m_texture = renderer::Resources::GetTexture( renderer::TextureWhite );

	// Use a default 2D material
	m_material = new RrMaterial();
	m_material->setTexture( TEX_DIFFUSE, core::Orphan(new CTexture("null")) );
	m_material->setTexture( TEX_SURFACE, renderer::Resources::GetTexture(renderer::TextureBlack) );

	m_material->passinfo.push_back( RrPassForward() );
	m_material->passinfo[0].shader = new RrShader( "shaders/fws/fullbright.glsl" );
	m_material->passinfo[0].m_lighting_mode = renderer::LI_NONE;
	m_material->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	m_material->passinfo[0].m_face_mode = renderer::FM_FRONTANDBACK;

	m_material->setTexture( TEX_DIFFUSE, m_texture );
	//m_texture->RemoveReference(); // TODO: Check if need remove this ref

	m_modeldata.triangles = new arModelTriangle [2048];
	m_modeldata.vertices = new arModelVertex [4096];
}
CutsceneEditor::CGeometryRenderer::~CGeometryRenderer ( void )
{ }

void CutsceneEditor::CGeometryRenderer::UpdateMesh ( void )
{
	// Set up screenmap
	core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
		Vector3f(-1.0F, -1.0F, -1.0F),
		Vector3f(+1.0F, +1.0F, +1.0F)
	);
	screenMapping.position.y = 1.0F;
	screenMapping.size.y = -2.0F;

	// Create builder
	rrMeshBuilder2D builder (screenMapping, &m_modeldata);
	m_modeldata.triangleNum = 0;
	m_modeldata.vertexNum = 0;

	// Set up colors
	const Color kNodeBgColor		(0.35, 0.35, 0.35, 1.0);
	const Color kNodeBgOutline		(0.65, 0.65, 0.65, 1.0);
	const Color kNodeFocusOutline	(1.0, 1.0, 1.0, 1.0);

	const Color kConnectorColor		(kNodeBgColor);
	const Color kConnectorOutline	(kNodeBgOutline);
	const Color kConnectorFocusOutline	(kNodeFocusOutline);
	const Real	kConnectorRadius	(8.0F);
	const Real	kConnectorSpacing	(12.0F);

	// Create all the geometry for nodes!
	{
		Rect		draw_rect;
		Vector2d	draw_pos;

		// Loop through all the nodes to draw connections
		// TODO: Precache these links so as to make it not slow as fuck
		for (auto& node : m_owner->m_nodes)
		{
			if (node.node != NULL)
			{
				const int connect_count = node.node->GetOutputNodeCount();
				for (int connecti = 0; connecti < connect_count; ++connecti)
				{
					// Get the node we're connected to:
					auto node_output = node.node->GetOutputNode(connecti);

					// Loop through all the editor nodes and find the matching one:
					for (size_t nodesearch_i = 0; nodesearch_i < m_owner->m_nodes.size(); ++nodesearch_i)
					{
						// Is this the correct node?
						auto& dest_node = m_owner->m_nodes[nodesearch_i];
						if (dest_node.node == node_output)
						{
							// Draw from node to this node
							Vector2d source_pos = m_owner->uiGetNodeOutputPosition(&node, connecti);
							Vector2d dest_pos = m_owner->uiGetNodeInputPosition(&dest_node);

							source_pos = _PixelRoundPosition(source_pos - m_owner->m_target_camera_position);
							dest_pos = _PixelRoundPosition(dest_pos - m_owner->m_target_camera_position);

							builder.addLine(source_pos, dest_pos, kConnectorFocusOutline);

							break;
						}
					}
				}
			}
		}

		// Loop through all the nodes to draw the nodes
		for (auto& node : m_owner->m_nodes)
		{
			draw_rect = m_owner->uiGetNodeRect(&node);
			draw_rect.pos = _PixelRoundPosition(draw_rect.pos - m_owner->m_target_camera_position),

			// Draw the background
			builder.addRect(draw_rect, kNodeBgColor, false);
			builder.addRect(draw_rect, kNodeBgOutline, true);

			// Draw the connection on top
			if (node.node != NULL && node.node->GetNodeType() != common::cts::kNodeTypeStart)
			{
				draw_pos = _PixelRoundPosition(m_owner->uiGetNodeInputPosition(&node) - m_owner->m_target_camera_position);
				builder.addCircle(draw_pos, kConnectorRadius, kConnectorColor, 10, false);
				builder.addCircle(draw_pos, kConnectorRadius, kConnectorOutline, 10, true);
			}

			// Draw the connections on the bottom
			if (node.node != NULL)
			{
				const int connect_count = node.node->GetOutputNodeCount();
				for (int connecti = 0; connecti < connect_count; ++connecti)
				{
					draw_pos = _PixelRoundPosition(m_owner->uiGetNodeOutputPosition(&node, connecti) - m_owner->m_target_camera_position);
					builder.addCircle(draw_pos, kConnectorRadius, kConnectorColor, 10, false);
					builder.addCircle(draw_pos, kConnectorRadius, kConnectorOutline, 10, true);
				}
			}
		}

		// Draw the focus rect over the mouseover node
		if (m_owner->m_mouseover_node
			&& m_owner->m_mouseover_index >= 0 && m_owner->m_mouseover_index < m_owner->m_nodes.size())
		{
			auto& node = m_owner->m_nodes[m_owner->m_mouseover_index];
			draw_rect = m_owner->uiGetNodeRect(&node);
			draw_rect.pos = _PixelRoundPosition(draw_rect.pos - m_owner->m_target_camera_position),

			// Draw the focus border.
			builder.addRect(draw_rect, kNodeFocusOutline, true);
		}

		if (m_owner->m_mouseover_connector_input
			&& m_owner->m_mouseover_index >= 0 && m_owner->m_mouseover_index < m_owner->m_nodes.size())
		{
			auto& node = m_owner->m_nodes[m_owner->m_mouseover_index];
			draw_pos = _PixelRoundPosition(
				m_owner->uiGetNodeInputPosition(&node)
				- m_owner->m_target_camera_position);

			// Draw the focus border.
			builder.addCircle(draw_pos, kConnectorRadius, kConnectorFocusOutline, 10, true);
		}

		if (m_owner->m_mouseover_connector_output
			&& m_owner->m_mouseover_index >= 0 && m_owner->m_mouseover_index < m_owner->m_nodes.size())
		{
			auto& node = m_owner->m_nodes[m_owner->m_mouseover_index];
			draw_pos = _PixelRoundPosition(
				m_owner->uiGetNodeOutputPosition(&node, (int)m_owner->m_mouseover_connectorindex)
				- m_owner->m_target_camera_position);

			// Draw the focus border.
			builder.addCircle(draw_pos, kConnectorRadius, kConnectorFocusOutline, 10, true);
		}
	}

	// TODO: Draw the context menu background

	// Now with the mesh built, push it to the modeldata :)
	this->StreamLockModelData();
}