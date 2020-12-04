#include "SequenceEditor.h"

#include "core/debug/Console.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"

#include "renderer/camera/RrCamera.h"

#include "m04/eventide/UserInterface.h"
#include "m04/eventide_test/CubicLabel.h"
#include "m04/eventide/elements/Button.h"
#include "m04/eventide/elements/ListMenu.h"

#include "engine-common/dusk/controls/Panel.h"
#include "engine-common/dusk/controls/DropdownMenu.h"
#include "engine-common/dusk/layouts/Horizontal.h"

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
	bool			m_dragging;
};

m04::editor::SequenceEditor::SequenceEditor ( void )
	: CGameBehavior()
{
	dusk_interface = new dusk::UserInterface();
	user_interface = new ui::eventide::UserInterface(dusk_interface, NULL);

	{
		auto topPanel = dusk_interface->Add<dusk::elements::Panel>(dusk::ElementCreationDescription(NULL, Rect(0, 0, 256, 32)));
		auto topLayout = dusk_interface->Add<dusk::layouts::Horizontal>(dusk::LayoutCreationDescription(topPanel));

		auto fileMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
		fileMenu->m_contents = "File";
		fileMenu->Add("New", 'N', true, [](){});
		fileMenu->Add("Save", 'S', true, [](){});
		fileMenu->Add("Save As", 0, true, [](){});
		fileMenu->Add("Open", 'O', true, [](){});
		fileMenu->Add("Quit", 'Q', true, [](){});

		auto EditMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
		EditMenu->m_contents = "Edit";
		EditMenu->Add("Undo", 'U', true, [](){});
		EditMenu->Add("Redo", 'R', true, [](){});
		EditMenu->Add("Clone", 'L', true, [](){});
		EditMenu->Add("Copy", 'C', true, [](){});
		EditMenu->Add("Cut", 'T', true, [](){});
		EditMenu->Add("Paste", 'P', true, [](){});

		auto ViewMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
		ViewMenu->m_contents = "View";
		ViewMenu->Add("Reset", 'R', true, [](){});

		auto HelpMenu = dusk_interface->Add<dusk::elements::DropdownMenu>(dusk::ElementCreationDescription(topLayout, Rect()));
		HelpMenu->m_contents = "Help";
		HelpMenu->Add("About", 'A', true, [](){});
	}

	test_element = new ETCubicLabel();
	test_element->SetBBox( core::math::BoundingBox( Matrix4x4(), Vector3f(100, 100, 0), Vector3f(64, 64, 4) ) );

	auto button = new ui::eventide::elements::Button();
	button->SetBBox( core::math::BoundingBox( Rotator(), Vector3f(-50, -10, 0), Vector3f(64, 32, 4) ) );
	button->m_contents = "Press me.";

	auto list = new ui::eventide::elements::ListMenu();
	list->SetBBox( core::math::BoundingBox( Rotator(), Vector3f(-100, 200, 0), Vector3f() ) );
	list->SetListChoices({"Option 1", "Option 2", "Eggs", "Murder", "Milk"});

	auto draggable = new DraggableButtonTest();
	draggable->SetBBox( core::math::BoundingBox( Rotator(), Vector3f(120, -10, 0), Vector3f(64, 32, 4) ) );
	draggable->m_contents = "Drag me.";

	RrCamera* camera = new RrCamera();
	camera->SetActive();
	// Override certain camera aspects to get the right projection
	camera->transform.rotation = Rotator( 0.0, -90, -90 );
	camera->transform.position.z = 500;
	camera->zNear = 1;
	camera->zFar = 1000;
	camera->fieldOfView = 40;
}
m04::editor::SequenceEditor::~SequenceEditor ( void )
{
	delete_safe(test_element);
	delete_safe_decrement(user_interface);

	debug::Console->PrintMessage("SequenceEditor shutdown.\n");
}

void m04::editor::SequenceEditor::Update ( void )
{
	UpdateCameraControl();
}


void m04::editor::SequenceEditor::UpdateCameraControl ( void )
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