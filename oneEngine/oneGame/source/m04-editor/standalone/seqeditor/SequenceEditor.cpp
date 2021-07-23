#include "SequenceEditor.h"

#include "core/debug/Console.h"
#include "core/system/Screen.h"
#include "core/input/CInput.h"
#include "core/utils/string.h"

#include "core-ext/system/io/Resources.h"
#include "core-ext/containers/arStringEnum.h"
#include "core-ext/system/io/osf.h"
#include "core-ext/settings/PersistentSettings.h"

#include "renderer/camera/RrCamera.h"
#include "renderer/windowing/RrWindow.h"

#include "m04/eventide/UserInterface.h"
#include "m04/eventide_test/CubicLabel.h"
#include "m04/eventide/elements/Button.h"
#include "m04/eventide/elements/ListMenu.h"

static core::settings::PersistentSetting<std::string> gPSetLastSavedTarget (
	"seqedit_lastSavedTarget",
	"",
	core::settings::PersistentSettingGroup::kUser);

//class DraggableButtonTest : public ui::eventide::elements::Button
//{
//public:
//	DraggableButtonTest ( ui::eventide::UserInterface* ui = NULL )
//		: Button(ui)
//	{
//		;
//	}
//
//	virtual void			OnEventMouse ( const EventMouse& mouse_event ) override
//	{
//		if (mouse_event.type == EventMouse::Type::kDragged)
//		{
//			core::math::BoundingBox bbox = GetBBox();
//			bbox.m_M.translate(mouse_event.velocity_world);
//			bbox.m_MInverse = bbox.m_M.inverse();
//			SetBBox( bbox );
//			RequestUpdateMesh();
//		}
//		else
//		{
//			if (mouse_event.type == EventMouse::Type::kClicked)
//			{
//				m_dragging = true;
//				m_ui->LockMouse();
//			}
//			else if (mouse_event.type == EventMouse::Type::kReleased)
//			{
//				m_dragging = false;
//				m_ui->UnlockMouse();
//			}
//
//			Button::OnEventMouse(mouse_event);
//		}
//	}
//
//private:
//	bool			m_dragging = false;
//};

m04::editor::SequenceEditor::SequenceEditor ( void )
	: CGameBehavior()
{
	dusk_interface = new dusk::UserInterface();
	user_interface = new ui::eventide::UserInterface(dusk_interface, NULL);

	top_menu = new m04::editor::sequence::TopMenu(dusk_interface, this);
	mouse_gizmo = new m04::editor::sequence::MouseGizmo(user_interface);
	grid_gizmo = new m04::editor::sequence::GridGizmo(user_interface, this);

	RrCamera* camera = new RrCamera();
	camera->SetActive();
	// Override certain camera aspects to get the right projection
	camera->transform.rotation = Rotator( 0.0, -90, -90 );
	camera->transform.position.z = 800;
	camera->zNear = 1;
	camera->zFar = 4000;
	camera->fieldOfView = 40;

	// Update windowing options
	RrWindow::Main()->SetWantsClipCursor(false);
	RrWindow::Main()->SetWantsHideCursor(true);
	RrWindow::Main()->SetWantsSystemCursor(true);
	RrWindow::Main()->SetZeroInputOnLoseFocus(true);

	// Create board state
	board_state = new m04::editor::sequence::NodeBoardState(this);

	// Initialize with "system/kingfisher.sel" for now
	LoadSequenceEditorListing("system/kingfisher.sel");

	// Load prefs
	save_target_filename = gPSetLastSavedTarget;
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

void m04::editor::SequenceEditor::LoadSequenceEditorListing( const char* sel_path )
{
	// first find the file via resources
	FILE* sel_fp = core::Resources::Open(sel_path, "rb");
	if (sel_fp == NULL)
	{
		throw core::InvalidArgumentException();
		return;
	}
	
	// create OSF for reading
	io::OSFReader reader (sel_fp);

	// the sel has enum types and node types.
	// for now we skip the node types because that's a pain to define properly

	// read in the osf entry-by-entry
	io::OSFEntryInfo entry;
	do
	{
		reader.GetNext(entry);
		if (entry.type == io::kOSFEntryTypeObject)
		{
			if (entry.name.compare("enumtype"))
			{
				printf("SEL: found enumtype \"%s\"\n", entry.value.c_str());

				std::string enumtypeName = entry.value;
				core::utils::string::ToLower(enumtypeName);

				std::vector<arStringEnumDefinition::NameValue> nameValues;
				reader.GoInto(entry);
				do
				{
					reader.GetNext(entry);
					if (entry.type == io::kOSFEntryTypeNormal)
					{
						// The first value is the value of the enum entry
						int32_t value = atoi(entry.name);
					
						std::string parsed_nameEntry, parsed_readableEntry;
						std::string unparsed_nameEntry = entry.value;
						// Split on the first space
						auto unparsed_firstSpace = unparsed_nameEntry.find_first_of(core::utils::string::kWhitespace, 0);
						if (unparsed_firstSpace != string::npos)
						{	
							// If space, then we have a readable name
							parsed_nameEntry = unparsed_nameEntry.substr(0, unparsed_firstSpace);
							parsed_readableEntry = unparsed_nameEntry.substr(unparsed_firstSpace + 1);
							parsed_readableEntry = core::utils::string::FullTrim(parsed_readableEntry);
						}
						// No readable name yet?
						if (unparsed_firstSpace == string::npos || parsed_readableEntry.size() <= 0)
						{	
							parsed_nameEntry = unparsed_nameEntry;
							parsed_readableEntry = unparsed_nameEntry;
						}
						// Modify first level of the readable name to be capital.
						ARCORE_ASSERT(parsed_readableEntry.size() > 0);
						parsed_readableEntry[0] = ::toupper(parsed_readableEntry[0]);

						printf("SEL: %d -> \"%s\", readable \"%s\"\n", value, parsed_nameEntry.c_str(), parsed_readableEntry.c_str());

						nameValues.push_back({parsed_nameEntry, value});
					}
				}
				while (entry.type != io::kOSFEntryTypeEnd);

				// Save new enumtype
				enum_definitions[enumtypeName.c_str()] = arStringEnumDefinition::CreateNew(nameValues);
			}
		}
	}
	while (entry.type != io::kOSFEntryTypeEoF);

	// close file
	fclose(sel_fp);
}

void m04::editor::SequenceEditor::Update ( void )
{
	// Do right click menu first, before all other actions.
	UpdateRightClickMenu();

	// Update hotkeys in top menu
	top_menu->UpdateHotkeys();

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
		const Vector2f mouseScreenPosition (core::Input::MouseX() / Screen::Info.width, core::Input::MouseY() / Screen::Info.height);
		const Ray mouseRay = Ray(
			RrCamera::activeCamera->transform.position,
			RrCamera::activeCamera->ScreenToWorldDir(mouseScreenPosition)
			);

		if (!dragging_view && !zooming_view)
		{
			// TODO: make a better peek & camera control option
			if (core::Input::Key(core::kVkAlt) && core::Input::Key(core::kVkControl))
			{
				RrCamera::activeCamera->transform.rotation = Rotator( 0.0, -90, -90 ) * Rotator(0, -(mouseScreenPosition.y - 0.5) * 45.0, (mouseScreenPosition.x - 0.5) * 45.0);
			}
			else
			{
				RrCamera::activeCamera->transform.rotation = Rotator( 0.0, -90, -90 );
			}

			// Use the zoom control to...zoom in and out around the mouse
			if (core::Input::DeltaMouseZoom() != 0)
			{
				float delta_zoom = core::Input::DeltaMouseZoom() / 120.0F * RrCamera::activeCamera->transform.position.z * 0.1F;
				RrCamera::activeCamera->transform.position += Vector3f(0, 0, delta_zoom);
			}

			// Limit camera Z
			const Real kMaxCameraZ = RrCamera::activeCamera->zFar * 0.95F;
			if (RrCamera::activeCamera->transform.position.z > kMaxCameraZ)
			{
				RrCamera::activeCamera->transform.position.z = kMaxCameraZ;
			}

			// Use middle mouse to drag view around
			if (core::Input::MouseDown(core::kMBMiddle))
			{
				if (core::Input::Key(core::kVkAlt))
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

			if (core::Input::MouseUp(core::kMBMiddle))
			{
				// On button release, unlock. End drag mode.
				user_interface->UnlockMouse();
				dragging_view = false;
			}
		}
		else if (zooming_view)
		{
			RrCamera::activeCamera->transform.position += Vector3f(0, 0, core::Input::DeltaMouseY());

			if (core::Input::MouseUp(core::kMBMiddle))
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

	if (!dusk_interface->IsMouseInside() && core::Input::MouseUp(core::kMBRight))
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

Vector3f m04::editor::SequenceEditor::GetMousePosition3D ( void )
{
	if (mouse_gizmo)
	{
		return mouse_gizmo->GetBBox().GetCenterPoint();
	}
	return Vector3f();
}

void m04::editor::SequenceEditor::SetSaveTargetFilename ( const char* filename )
{
	save_target_filename = filename;
	gPSetLastSavedTarget = save_target_filename;
	RrWindow::Main()->SetTitle(("Sequence Editor: " + save_target_filename + (workspace_dirty ? "*" : "")).c_str()); //TODO: move elsewhere
	core::settings::Persistent::Save(); // Save current settings.
}