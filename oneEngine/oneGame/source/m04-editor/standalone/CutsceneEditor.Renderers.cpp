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
#include "renderer/material/RrShaderProgram.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/Material.h"
#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"

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

static Vector2f _PixelRoundPosition ( const Vector2f vect )
{
	return Vector2f(
		1.0F * math::round( vect.x ),
		1.0F * math::round( vect.y )
	);
}

CutsceneEditor::CGeometryUpdater::CGeometryUpdater ( CutsceneEditor* owner )
	: m_owner(owner), RrLogicObject()
{

}

void CutsceneEditor::CGeometryUpdater::PostStep ( void )
{
	m_owner->m_geometryRenderer->UpdateMesh();
	m_owner->m_largeTextRenderer->UpdateMesh();
	m_owner->m_normalTextRenderer->UpdateMesh();
}

CutsceneEditor::CLargeTextRenderer::CLargeTextRenderer ( CutsceneEditor* owner )
	: m_owner(owner), CStreamedRenderable3D()
{
	// Bring up
	transform.world.position.z = -1;

	m_font_texture = RrFontTexture::Load("YanoneKaffeesatz-B.otf", 24, FW_BOLD);
	//m_font_texture->SetFilter( SamplingLinear );

	// Use a default 2D material
	RrPass spritePass;
	spritePass.utilSetupAsDefault();
	spritePass.m_type = kPassTypeForward;
	spritePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	spritePass.m_cullMode = gpu::kCullModeNone;
	spritePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	spritePass.m_orderOffset = 2;
	spritePass.setTexture( TEX_DIFFUSE, m_font_texture );
	spritePass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
	spritePass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	spritePass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	spritePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	spritePass.setVertexSpecificationByCommonList(t_vspec, 3);
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &spritePass);

	m_modeldata.indices = new uint16_t [2048 * 3];
	m_modeldata.position = new Vector3f [4096];
	m_modeldata.color = new Vector4f [4096];
	m_modeldata.texcoord0 = new Vector3f [4096];
	m_modeldata.normal = new Vector3f [4096];
}
CutsceneEditor::CLargeTextRenderer::~CLargeTextRenderer ( void )
{ 
	if (m_font_texture)
	{
		m_font_texture->RemoveReference();
	}

	delete_safe_array(m_modeldata.indices);
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.normal);
}

void CutsceneEditor::CLargeTextRenderer::UpdateMesh ( void )
{
	// Set up screenmap
	core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
		Vector3f(-1.0F, -1.0F, -1.0F),
		Vector3f(+1.0F, +1.0F, +1.0F)
	);
	screenMapping.position.y = 1.0F;
	screenMapping.size.y = -2.0F;

	if ( RrCamera::activeCamera )
	{	// Modify console size based on render scale so it is always legible!
		screenMapping.position += screenMapping.size * 0.5F;
		screenMapping.size *= RrCamera::activeCamera->renderScale;
		screenMapping.position -= screenMapping.size * 0.5F;
	}

	// Create builder
	rrTextBuilder2D builder (m_font_texture, screenMapping, &m_modeldata);
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;

	// Create the visual text
	Vector2f screenOrigin (-640,-360 + 20);
	Vector2f generalMargins (4, 4);

	builder.addText(
		screenOrigin + Vector2f(10,10),
		Color(1.0F, 1.0F, 1.0F, 1.0F),
		"Cutscene Editor" );

	// Now with the mesh built, push it to the modeldata :)
	this->StreamLockModelData();
}


CutsceneEditor::CNormalTextRenderer::CNormalTextRenderer ( CutsceneEditor* owner )
		: m_owner(owner), CStreamedRenderable3D()
{
	m_font_texture = RrFontTexture::Load("YanoneKaffeesatz-B.otf", 18, FW_BOLD);
	//m_font_texture->SetFilter( SamplingLinear );

	// Use a default 2D material
	RrPass spritePass;
	spritePass.utilSetupAsDefault();
	spritePass.m_type = kPassTypeForward;
	spritePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	spritePass.m_cullMode = gpu::kCullModeNone;
	spritePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	spritePass.m_orderOffset = 1;
	spritePass.setTexture( TEX_DIFFUSE, m_font_texture );
	spritePass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
	spritePass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	spritePass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	spritePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	spritePass.setVertexSpecificationByCommonList(t_vspec, 3);
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &spritePass);

	m_modeldata.indices = new uint16_t [2048 * 3];
	m_modeldata.position = new Vector3f [4096];
	m_modeldata.color = new Vector4f [4096];
	m_modeldata.texcoord0 = new Vector3f [4096];
	m_modeldata.normal = new Vector3f [4096];
}
CutsceneEditor::CNormalTextRenderer::~CNormalTextRenderer ( void )
{
	if (m_font_texture)
	{
		m_font_texture->RemoveReference();
	}

	delete_safe_array(m_modeldata.indices);
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.normal);
}

void CutsceneEditor::CNormalTextRenderer::UpdateMesh ( void )
{
	// Set up screenmap
	core::math::Cubic screenMapping = core::math::Cubic::FromPosition(
		Vector3f(-1.0F, -1.0F, -1.0F),
		Vector3f(+1.0F, +1.0F, +1.0F)
	);
	screenMapping.position.y = 1.0F;
	screenMapping.size.y = -2.0F;

	if ( RrCamera::activeCamera )
	{	// Modify console size based on render scale so it is always legible!
		screenMapping.position += screenMapping.size * 0.5F;
		screenMapping.size *= RrCamera::activeCamera->renderScale;
		screenMapping.position -= screenMapping.size * 0.5F;
	}

	// Create builder
	rrTextBuilder2D builder (m_font_texture, screenMapping, &m_modeldata);
	m_modeldata.indexNum = 0;
	m_modeldata.vertexNum = 0;

	// Create the visual text
	{
		Vector2f screenOrigin (-640,-360 + 20);
		Vector2f generalMargins (4, 4);

		builder.addText(
			screenOrigin + Vector2f(10, 40),
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
				+ Vector2f(0.0F, m_owner->m_contextMenu_spacing + m_owner->m_contextMenu_spacing * i)
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
	// Push down
	transform.world.position.z = 1;

	// Use a default 2D material
	RrPass spritePass;
	spritePass.utilSetupAsDefault();
	spritePass.m_type = kPassTypeForward;
	spritePass.m_alphaMode = renderer::kAlphaModeTranslucent;
	spritePass.m_cullMode = gpu::kCullModeNone;
	spritePass.m_surface.diffuseColor = Color(1.0F, 1.0F, 1.0F, 1.0F);
	spritePass.m_orderOffset = 0;
	spritePass.setTexture( TEX_DIFFUSE, RrTexture::Load(renderer::kTextureWhite) );
	spritePass.setTexture( TEX_NORMALS, RrTexture::Load(renderer::kTextureNormalN0) );
	spritePass.setTexture( TEX_SURFACE, RrTexture::Load(renderer::kTextureBlack) );
	spritePass.setTexture( TEX_OVERLAY, RrTexture::Load(renderer::kTextureGrayA0) );
	spritePass.setProgram( RrShaderProgram::Load(rrShaderProgramVsPs{"shaders/sys/fullbright_vv.spv", "shaders/sys/fullbright_p.spv"}) );
	renderer::shader::Location t_vspec[] = {renderer::shader::Location::kPosition,
											renderer::shader::Location::kUV0,
											renderer::shader::Location::kColor};
	spritePass.setVertexSpecificationByCommonList(t_vspec, 3);
	spritePass.m_primitiveType = gpu::kPrimitiveTopologyTriangleStrip;
	PassInitWithInput(0, &spritePass);

	m_modeldata.indices = new uint16_t [2048 * 3];
	m_modeldata.position = new Vector3f [4096];
	m_modeldata.color = new Vector4f [4096];
	m_modeldata.texcoord0 = new Vector3f [4096];
	m_modeldata.normal = new Vector3f [4096];
}
CutsceneEditor::CGeometryRenderer::~CGeometryRenderer ( void )
{ 
	delete_safe_array(m_modeldata.indices);
	delete_safe_array(m_modeldata.position);
	delete_safe_array(m_modeldata.color);
	delete_safe_array(m_modeldata.texcoord0);
	delete_safe_array(m_modeldata.normal);
}

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
	m_modeldata.indexNum = 0;
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
		Vector2f	draw_pos;

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
							Vector2f source_pos = m_owner->uiGetNodeOutputPosition(&node, connecti);
							Vector2f dest_pos = m_owner->uiGetNodeInputPosition(&dest_node);

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