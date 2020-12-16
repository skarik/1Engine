#include "SequenceEditor.h"

#include "core/debug/Console.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "renderer/camera/RrCamera.h"

#include "m04/eventide/UserInterface.h"
#include "m04/eventide_test/CubicLabel.h"
#include "m04/eventide/elements/Button.h"
#include "m04/eventide/elements/ListMenu.h"

class DraggableButtonTest : public ui::eventide::elements::Button
{
public:
	DraggableButtonTest ( ui::eventide::UserInterface* ui = NULL )
		: Button(ui)
	{
		;
	}

	virtual void			OnEventMouse ( const EventMouse& mouse_event ) override
	{
		if (mouse_event.type == EventMouse::Type::kDragged)
		{
			core::math::BoundingBox bbox = GetBBox();
			bbox.m_M.translate(mouse_event.velocity_world);
			bbox.m_MInverse = bbox.m_M.inverse();
			SetBBox( bbox );
			RequestUpdateMesh();
		}
		else
		{
			if (mouse_event.type == EventMouse::Type::kClicked)
			{
				m_dragging = true;
				m_ui->LockMouse();
			}
			else if (mouse_event.type == EventMouse::Type::kReleased)
			{
				m_dragging = false;
				m_ui->UnlockMouse();
			}

			Button::OnEventMouse(mouse_event);
		}
	}

private:
	bool			m_dragging = false;
};

m04::editor::SequenceEditor::SequenceEditor ( void )
	: CGameBehavior()
{
	dusk_interface = new dusk::UserInterface();
	user_interface = new ui::eventide::UserInterface(dusk_interface, NULL);

	top_menu = new m04::editor::sequence::TopMenu(dusk_interface);
	mouse_gizmo = new m04::editor::sequence::MouseGizmo(user_interface);

	/*test_element = new ETCubicLabel();
	test_element->SetBBox( core::math::BoundingBox( Matrix4x4(), Vector3f(100, 100, 0), Vector3f(64, 64, 4) ) );

	auto button = new ui::eventide::elements::Button();
	button->SetBBox( core::math::BoundingBox( Rotator(), Vector3f(-50, -10, 0), Vector3f(64, 32, 4) ) );
	button->m_contents = "Press me.";

	auto list = new ui::eventide::elements::ListMenu();
	list->SetBBox( core::math::BoundingBox( Rotator(), Vector3f(-100, 200, 0), Vector3f() ) );
	list->SetListChoices({"Option 1", "Option 2", "Eggs", "Murder", "Milk"});*/

	/*auto draggable = new DraggableButtonTest();
	draggable->SetBBox( core::math::BoundingBox( Rotator(), Vector3f(120, -10, 0), Vector3f(64, 32, 4) ) );
	draggable->m_contents = "Drag me.";*/

	RrCamera* camera = new RrCamera();
	camera->SetActive();
	// Override certain camera aspects to get the right projection
	camera->transform.rotation = Rotator( 0.0, -90, -90 );
	camera->transform.position.z = 800;
	camera->zNear = 1;
	camera->zFar = 4000;
	camera->fieldOfView = 40;


	// Create board state
	board_state = new m04::editor::sequence::NodeBoardState(this);
}
m04::editor::SequenceEditor::~SequenceEditor ( void )
{
	delete_safe(board_state);



	if (right_click_menu != NULL)
	{
		right_click_menu->Destroy();
		right_click_menu = NULL;
	}
	//test_element->Destroy();
	delete_safe_decrement(user_interface);
	delete_safe_decrement(dusk_interface);

	debug::Console->PrintMessage("SequenceEditor shutdown.\n");
}

void m04::editor::SequenceEditor::Update ( void )
{
	// Do right click menu first, before all other actions.
	UpdateRightClickMenu();

	// Camera control last - low priority action.
	UpdateCameraControl();
}


void m04::editor::SequenceEditor::UpdateCameraControl ( void )
{
	// We cannot do camera control if the mouse is locked
	if (user_interface->IsMouseLocked()
		// Don't do camera control if the right-click menu is up
		|| right_click_menu != NULL)
	{
		// Need to notify mouse gizmo to change to normal pointer state.
	}
	else
	{
		const Vector2f mouseScreenPosition (Input::MouseX() / Screen::Info.width, Input::MouseY() / Screen::Info.height);
		const Ray mouseRay = Ray(
			RrCamera::activeCamera->transform.position,
			RrCamera::activeCamera->ScreenToWorldDir(mouseScreenPosition)
			);

		if (!dragging_view && !zooming_view)
		{
			if (Input::MouseDown(Input::MBMiddle))
			{
				if (Input::Key(VK_MENU))
				{
					if (!user_interface->IsMouseInside())
					{
						// On button press, lock. Begin drag mode.
						user_interface->LockMouse();
						zooming_view = true;
					}
				}
				else
				{
					if (!user_interface->IsMouseInside())
					{
						// On button press, lock. Begin drag mode.
						user_interface->LockMouse();
						dragging_view = true;

						// Let's lock a specific position down on the XY plane.
						float hit_distance = 0.0F;
						if (core::math::Plane(Vector3f(), Vector3f(0, 0, 1)).Raycast(mouseRay, hit_distance))
						{
							// This position is what we're going to be dragging against. We want this position to stay under the mouse.
							dragging_reference_position = mouseRay.pos + mouseRay.dir * hit_distance;
						}
						else
						{
							ARCORE_ASSERT(false);
						}
					}
				}
			}
		}
		else if (dragging_view)
		{
			// Let's find the current position of the mouse on the XY plane.
			float hit_distance = 0.0F;
			if (core::math::Plane(Vector3f(), Vector3f(0, 0, 1)).Raycast(mouseRay, hit_distance))
			{
				Vector3f current_3d_mouse_position = mouseRay.pos + mouseRay.dir * hit_distance;
			
				// We need to make a translation now in world space. This is 1-to-1 with the camera delta that needs to happen.
				Vector3f delta_3d_position = current_3d_mouse_position - dragging_reference_position;
			
				// TODO: Toggle for smooth scrolling. Needs to have a time-based acceleration and decceleration.
				RrCamera::activeCamera->transform.position -= delta_3d_position;
			}
			else
			{
				ARCORE_ASSERT(false);
			}

			if (Input::MouseUp(Input::MBMiddle))
			{
				// On button release, unlock. End drag mode.
				user_interface->UnlockMouse();
				dragging_view = false;
			}
		}
		else if (zooming_view)
		{
			RrCamera::activeCamera->transform.position += Vector3f(0, 0, Input::DeltaMouseY());

			if (Input::MouseUp(Input::MBMiddle))
			{
				// On button release, unlock. End drag mode.
				user_interface->UnlockMouse();
				zooming_view = false;
			}
		}
	}
}

void m04::editor::SequenceEditor::UpdateRightClickMenu ( void )
{
	if (right_click_menu != NULL)
	{
		// If menu is up, check if we want to destroy it
		if (right_click_menu->m_losingFocus)
		{
			right_click_menu->Destroy();
			right_click_menu = NULL;
		}
	}

	if (Input::MouseUp(Input::MBRight))
	{
		if (right_click_menu == NULL
			|| !right_click_menu->GetMouseInside())
		{
			if (right_click_menu != NULL)
			{
				right_click_menu->Destroy();
				right_click_menu = NULL;
			}

			right_click_menu = new m04::editor::sequence::RightClickListMenu(this);
			right_click_menu->SetBBox(core::math::BoundingBox(Rotator(), mouse_gizmo->GetBBox().GetCenterPoint(), Vector3f()));
		}
	}
}