#include "CutsceneEditor.h"

#include "core/input/CInput.h"
#include "core/math/Math.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/FileUtils.h"

#include "core/system/Screen.h"

#include "engine/cutscene/Node.h"

#include "renderer/camera/CCamera.h"

#include "renderer/object/CRenderable3D.h"
#include "renderer/object/CStreamedRenderable3D.h"
#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/RrMaterial.h"

#include "renderer/resource/CResourceManager.h"

#include "renderer/utils/rrMeshBuilder2D.h"
#include "renderer/utils/rrTextBuilder2D.h"

using namespace M04;

class CutsceneEditor::CLargeTextRenderer : public CRenderable3D
{
private:
	CutsceneEditor*		m_owner;
	CBitmapFont*		m_font_texture;

public:
	explicit CLargeTextRenderer ( CutsceneEditor* owner )
		: m_owner(owner), CRenderable3D()
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
		PushModeldata();
	}
};

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

class CutsceneEditor::CNormalTextRenderer : public CRenderable3D
{
private:
	CutsceneEditor*		m_owner;
	CBitmapFont*		m_font_texture;

public:
	explicit CNormalTextRenderer ( CutsceneEditor* owner )
		: m_owner(owner), CRenderable3D()
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
					;
				}
				const char* l_strNodeType = _GetNodeEnumString(nodeType);

				// Display node type
				builder.addText(
					m_owner->m_target_camera_position + generalMargins + nodes.position,
					Color(1.0F, 1.0F, 1.0F, 1.0F),
					l_strNodeType );
			}
		}

		// Now with the mesh built, push it to the modeldata :)
		PushModeldata();
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

	m_nodes.push_back(EditorNode({NULL, Vector2d(0,0)}));
	m_nodes.push_back(EditorNode({NULL, Vector2d(0,128)}));
}

CutsceneEditor::~CutsceneEditor ( void )
{
	delete_safe(m_largeTextRenderer);
	delete_safe(m_normalTextRenderer);
}

void CutsceneEditor::Update ( void )
{
	doViewNavigationDrag();

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