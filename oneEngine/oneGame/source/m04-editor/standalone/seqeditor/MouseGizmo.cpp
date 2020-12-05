#include "MouseGizmo.h"

#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"

m04::editor::sequence::MouseGizmo::MouseGizmo ( ui::eventide::UserInterface* ui )
	: Element( ui )
{
	m_frameUpdate = FrameUpdate::kPerFrame;
	m_mouseInteract = MouseInteract::kNone;

	m_texture = LoadTexture("textures/editor/sequenceElements.png");
}

m04::editor::sequence::MouseGizmo::~MouseGizmo ( void )
{
	ReleaseTexture(m_texture);
}

void m04::editor::sequence::MouseGizmo::BuildMesh ( void )
{
	ParamsForQuad quadParams;
	quadParams.position = GetBBoxAbsolute().GetCenterPoint();
	quadParams.uvs = Rect(0.0F, 0.0F, 64.0F / 1024, 64.0F / 1024);
	quadParams.texture = &m_texture;

	// Move quad towards camera slightly
	Vector3f deltaToCamera = RrCamera::activeCamera->transform.position - quadParams.position;
	quadParams.position += deltaToCamera.normal() * 2.0F;

	buildQuad(quadParams);
}

void m04::editor::sequence::MouseGizmo::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	Vector3f centerPosition = m_ui->GetMousePosition();
	SetBBox(core::math::BoundingBox(Rotator(), centerPosition, Vector3f(10, 10, 1)));

	// Now that we're at a new position, update the menu
	RequestUpdateMesh();
}