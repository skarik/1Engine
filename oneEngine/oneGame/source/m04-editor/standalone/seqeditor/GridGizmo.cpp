#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "renderer/camera/RrCamera.h"
#include "m04/eventide/UserInterface.h"
#include "./Constants.h"

#include "GridGizmo.h"

m04::editor::sequence::GridGizmo::GridGizmo ( ui::eventide::UserInterface* ui, m04::editor::SequenceEditor* editor )
	: Element( ui )
	, main_editor(editor)
{
	m_frameUpdate = FrameUpdate::kPerFrame;
	m_mouseInteract = MouseInteract::kNone;

	m_texture = LoadTexture("textures/editor/grid.png");
}

m04::editor::sequence::GridGizmo::~GridGizmo ( void )
{
	ReleaseTexture(m_texture);
}

void m04::editor::sequence::GridGizmo::BuildMesh ( void )
{
	//ParamsForQuad quadParams;
	//quadParams.position = GetBBoxAbsolute().GetCenterPoint();
	//quadParams.color = Color(1, 1, 1, 1);
	////quadParams.uvs = Rect(0.0F, 0.0F, 1.0F, 1.0F);
	////quadParams.texture = &m_texture;

	//// Need camera delta for next
	//const Vector3f deltaToCamera = RrCamera::activeCamera->transform.position - quadParams.position;

	//// Make size based on camera distance to keep screen-size constant
	//quadParams.size = Vector2f(10, 10) * (deltaToCamera.magnitude() / 600.0F);

	//// Move quad towards camera slightly
	//quadParams.position += deltaToCamera.normal();// * 2.0F;

	//buildQuad(quadParams);

	// Build out vertical lines
	
	Real gridSize = 32.0F;
	Real workGridHeight = -3.0F;
	Vector3f planeNormal = {0.0F, 0.0F, 1.0F};
	Vector3f gridOffsetMultiplier = {
		fabsf(planeNormal.x) < 0.707F ? 1.0F : 0.0F,
		fabsf(planeNormal.y) < 0.707F ? 1.0F : 0.0F,
		fabsf(planeNormal.z) < 0.707F ? 1.0F : 0.0F};

	RrCamera* camera = RrCamera::activeCamera;

	// Get the min & max positions of where to build the grid
	Vector3f rayTopLeft = camera->ScreenToWorldDir(Vector2f(0.0F, 0.0F));
	Vector3f rayBottomRight = camera->ScreenToWorldDir(Vector2f(1.0F, 1.0F));

	// Trace down to the lowest position of the working area
	core::math::Plane workPlane({0.0F, 0.0F, workGridHeight}, planeNormal);
	Real distanceTopLeft = 0.0F;
	Real distanceBottomRight = 0.0F;
	if (workPlane.Raycast(Ray(camera->transform.position, rayTopLeft), distanceTopLeft)
		&& workPlane.Raycast(Ray(camera->transform.position, rayBottomRight), distanceBottomRight))
	{
		Vector3f workTopLeft = camera->transform.position + rayTopLeft * distanceTopLeft;
		Vector3f workBottomRight = camera->transform.position + rayBottomRight * distanceBottomRight;

		// Ensure the work area values are in the right order
		for (int componentIndex = 0; componentIndex < 3; ++componentIndex)
		{
			if (workTopLeft[componentIndex] > workBottomRight[componentIndex])
			{
				std::swap(workTopLeft[componentIndex], workBottomRight[componentIndex]);
			}
		}

		// Now that we have the work area, we need to make sure we're aligned to the grid
		Vector3f cornerStart = workTopLeft - Vector3f(gridSize, gridSize, gridSize).mulComponents(gridOffsetMultiplier);
		Vector3f cornerEnd = workBottomRight + Vector3f(gridSize, gridSize, gridSize).mulComponents(gridOffsetMultiplier);
		for (int componentIndex = 0; componentIndex < 3; ++componentIndex)
		{
			cornerStart[componentIndex] = math::round(cornerStart[componentIndex] / gridSize) * gridSize;
			cornerEnd[componentIndex] = math::round(cornerEnd[componentIndex] / gridSize) * gridSize;
		}

		// Loop and build quads horizontally
		for (Real virtualXPosition = cornerStart.x; virtualXPosition <= cornerEnd.x; virtualXPosition += gridSize)
		{
			ParamsForQuad quadParams;
			quadParams.position = Vector3f(virtualXPosition, (cornerStart.y + cornerEnd.y) * 0.5F, (cornerStart.z + cornerEnd.z) * 0.5F);
			quadParams.color = Color(0.15F, 0.15F, 0.15F, 1.0F);
			if (math::round(virtualXPosition / gridSize) % 10 == 0)
			{
				quadParams.color = Color(0.15F, 0.25F, 0.35F, 1.0F);
			}

			const Vector3f deltaToCamera = RrCamera::activeCamera->transform.position - quadParams.position;
			quadParams.size = Vector2f(0.5F * (deltaToCamera.magnitude() / 800.0F), (cornerEnd.y - cornerStart.y) * 0.5F);

			buildQuad(quadParams);
		}
		// Loop and build quads vertically
		for (Real virtualYPosition = cornerStart.y; virtualYPosition <= cornerEnd.y; virtualYPosition += gridSize)
		{
			ParamsForQuad quadParams;
			quadParams.position = Vector3f((cornerStart.x + cornerEnd.x) * 0.5F, virtualYPosition, (cornerStart.z + cornerEnd.z) * 0.5F);
			quadParams.color = Color(0.15F, 0.15F, 0.15F, 1.0F);
			if (math::round(virtualYPosition / gridSize) % 10 == 0)
			{
				quadParams.color = Color(0.15F, 0.25F, 0.35F, 1.0F);
			}

			const Vector3f deltaToCamera = RrCamera::activeCamera->transform.position - quadParams.position;
			quadParams.size = Vector2f((cornerEnd.x - cornerStart.x) * 0.5F, 0.5F * (deltaToCamera.magnitude() / 800.0F));

			buildQuad(quadParams);
		}
	}
}

void m04::editor::sequence::GridGizmo::OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame )
{
	Vector3f centerPosition = m_ui->GetMousePosition();

	// We need to project onto a plane in order to have a good spot for the cursor
	if (m_ui->GetMouseHit() == NULL)
	{
		const Vector2f mouseScreenPosition (core::Input::MouseX() / Screen::Info.width, core::Input::MouseY() / Screen::Info.height);
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