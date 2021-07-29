#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"
#include "./Constants.h"

#include "MouseGizmo.h"

m04::editor::sequence::MouseGizmo::MouseGizmo ( ui::eventide::UserInterface* ui )
	: Element( ui )
{
	m_frameUpdate = FrameUpdate::kPerFrame;
	m_mouseInteract = MouseInteract::kNone;

	m_texture = LoadTexture(m04::editor::sequence::gFilenameGUIElementTexture);
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

	// Need camera delta for next
	const Vector3f deltaToCamera = RrCamera::activeCamera->transform.position - quadParams.position;

	// Make size based on camera distance to keep screen-size constant
	quadParams.size = Vector2f(10, 10) * (deltaToCamera.magnitude() / 600.0F);

	// Move quad towards camera slightly
	quadParams.position += deltaToCamera.normal();// * 2.0F;

	buildQuad(quadParams);
}

void m04::editor::sequence::MouseGizmo::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	Vector3f centerPosition = m_ui->GetMousePosition();

	// We need to project onto a plane in order to have a good spot for the cursor
	if (m_ui->GetMouseHit() == NULL)
	{
		const Vector2f mouseScreenPosition (core::Input::MouseX() / core::GetFocusedScreen().GetWidth(), core::Input::MouseY() / core::GetFocusedScreen().GetHeight());
		const Ray mouseRay = Ray(
			RrCamera::activeCamera->transform.position,
			RrCamera::activeCamera->ScreenToWorldDir(mouseScreenPosition)
		);

		// Let's look for a specific position on an XY plane at the current mouse Z
		float hit_distance = 0.0F;
		if (core::math::Plane(centerPosition, Vector3f(0, 0, 1)).Raycast(mouseRay, hit_distance))
		{
			// Save new center position in this case
			centerPosition = mouseRay.pos + mouseRay.dir * hit_distance;
		}
	}

	// Set the bbox on the new center position
	SetBBox(core::math::BoundingBox(Rotator(), centerPosition, Vector3f(10, 10, 1)));

	// Now that we're at a new position, update the menu
	RequestUpdateMesh();
}