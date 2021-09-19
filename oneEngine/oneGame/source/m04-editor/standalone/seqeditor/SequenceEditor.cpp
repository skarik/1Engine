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
#include "renderer/state/RrRenderer.h"

#include "m04/eventide/UserInterface.h"
#include "m04/eventide_test/CubicLabel.h"
#include "m04/eventide/elements/Button.h"
#include "m04/eventide/elements/ListMenu.h"

static core::settings::PersistentSetting<std::string> gPSetLastSavedTarget (
	"seqedit_lastSavedTarget",
	"",
	core::settings::PersistentSettingGroup::kUser);

static core::settings::PersistentSetting<int32> gPSetWidowWidth (
	"seqedit_window_width",
	1280,
	core::settings::PersistentSettingGroup::kUser);
static core::settings::PersistentSetting<int32> gPSetWindowHeight (
	"seqedit_window_height",
	720,
	core::settings::PersistentSettingGroup::kUser);

m04::editor::SequenceEditor::SequenceEditor ( void )
	: CGameBehavior()
{
	// Set up window and output for this
	window = new RrWindow(RrRenderer::Active, NIL);
	window->Resize(gPSetWidowWidth, gPSetWindowHeight);

	editor_world = new RrWorld();
	RrRenderer::Active->AddWorld(editor_world);

	RrOutputInfo output (editor_world, window);
	output.name = "SequenceEditor";
	{
		editor_camera = new RrCamera(false);
		// Override certain camera aspects to get the right projection
		editor_camera->transform.rotation = Rotator( 0.0, -90, -90 );
		editor_camera->transform.position.z = 800;
		editor_camera->zNear = 1;
		editor_camera->zFar = 4000;
		editor_camera->fieldOfView = 40;
	}
	output.camera = editor_camera;
	output.update_interval_when_not_focused = 10;

	// Add the output to the renderer now that it's somewhat ready.
	RrRenderer::Active->AddOutput(output);

	dusk_interface = new dusk::UserInterface(window, editor_world);
	user_interface = new ui::eventide::UserInterface(window, dusk_interface, NULL, editor_world, editor_camera);

	top_menu = new m04::editor::sequence::TopMenu(dusk_interface, this);
	mouse_gizmo = new m04::editor::sequence::MouseGizmo(user_interface);
	grid_gizmo = new m04::editor::sequence::GridGizmo(user_interface, this);

	// Update windowing options
	window->SetWantsClipCursor(false);
	window->SetWantsHideCursor(false);
	window->SetWantsSystemCursor(true);
	window->SetZeroInputOnLoseFocus(true);

	// Create board state
	board_state = new m04::editor::sequence::NodeBoardState(this);

	// Initialize with "system/kingfisher.sel" for now
	LoadSequenceEditorListing("system/kingfisher.sel");

	// Load prefs
	save_target_filename = gPSetLastSavedTarget;

	// Everything is set up, show the window now.
	window->Show();
	window->SetTitle("Sequence Editor");
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
	//user_interface->RemoveReference();
	delete_safe_decrement(user_interface);
	//dusk_interface->RemoveReference();
	delete_safe_decrement(dusk_interface);

	RrRenderer::Active->RemoveOutput(RrRenderer::Active->FindOutputWithTarget(window));
	RrRenderer::Active->RemoveWorld(editor_world);
	window->Close();
	delete window;

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

	// Check if we want closing
	if (window->IsDone() || window->WantsClose())
	{
		DeleteObject(this);
	}
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
		const Vector2f mouseScreenPosition (core::Input::MouseX() / GetScreen().GetWidth(), core::Input::MouseY() / GetScreen().GetHeight());
		const Ray mouseRay = Ray(
			editor_camera->transform.position,
			editor_camera->ScreenToWorldDir(mouseScreenPosition)
			);

		if (!dragging_view && !zooming_view)
		{
			// TODO: make a better peek & camera control option
			if (core::Input::Key(core::kVkAlt) && core::Input::Key(core::kVkControl))
			{
				editor_camera->transform.rotation = Rotator( 0.0, -90, -90 ) * Rotator(0, -(mouseScreenPosition.y - 0.5) * 45.0, (mouseScreenPosition.x - 0.5) * 45.0);
			}
			else
			{
				editor_camera->transform.rotation = Rotator( 0.0, -90, -90 );
			}

			// Use the zoom control to...zoom in and out around the mouse
			if (core::Input::DeltaMouseZoom() != 0)
			{
				float delta_zoom = -core::Input::DeltaMouseZoom() / 120.0F * editor_camera->transform.position.z * 0.1F;

				// We want to get the position the mouse is on the grid and move forward & back against that.
				float hit_distance = 0.0F;
				if (core::math::Plane(Vector3f(), Vector3f(0, 0, 1)).Raycast(mouseRay, hit_distance))
				{
					Vector3f reference_position = mouseRay.pos + mouseRay.dir * hit_distance;
					editor_camera->transform.position += (editor_camera->transform.position - reference_position).normal() * delta_zoom;
				}
				else
				{
					editor_camera->transform.position += Vector3f(0, 0, delta_zoom);
				}
			}
			// Use the scrolls to scroll
			if (core::Input::DeltaMouseScroll() != 0)
			{
				float delta_scroll = core::Input::DeltaMouseScroll() * editor_camera->transform.position.z / 1000.0F;
				editor_camera->transform.position += Vector3f(0, delta_scroll, 0);
			}
			if (core::Input::DeltaMouseHScroll() != 0)
			{
				float delta_scroll = core::Input::DeltaMouseHScroll() * editor_camera->transform.position.z / 1000.0F;
				editor_camera->transform.position += Vector3f(delta_scroll, 0, 0);
			}

			// Limit camera Z
			const Real kMaxCameraZ = editor_camera->zFar * 0.95F;
			if (editor_camera->transform.position.z > kMaxCameraZ)
			{
				editor_camera->transform.position.z = kMaxCameraZ;
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
				editor_camera->transform.position -= delta_3d_position;
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
			editor_camera->transform.position += Vector3f(0, 0, core::Input::DeltaMouseY());

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
	window->SetTitle(("Sequence Editor: " + save_target_filename + (workspace_dirty ? "*" : "")).c_str()); //TODO: move elsewhere
	core::settings::Persistent::Save(); // Save current settings.
}

const ArScreen& m04::editor::SequenceEditor::GetScreen ( void )
{
	return window->GetScreen();
}