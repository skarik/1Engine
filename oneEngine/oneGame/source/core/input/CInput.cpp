#include "CInput.h"
#include "CXboxController.h"
#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"

#include <vector>

//===============================================================================================//

static std::vector<core::internal_input::InputState> g_InputStatesPerWindow;

// Window uses RrWindow::m_windowListIndex to access.
// Minimum of 1, the default index is 0 there is no window.

//===============================================================================================//

static void InputState_Reset ( core::internal_input::InputState* input_state )
{
	for ( int i = 0; i < core::kVkCount; ++i )
	{
		input_state->m_keys[i] = core::internal_input::KeyState();
	}
	for ( int i = 0; i < core::kMBCount; ++i )
	{
		input_state->m_mouseButtons[i] = core::internal_input::KeyState();
	}
}

//===============================================================================================//

// Clears out mouse state for next frame.
static void InputState_PreUpdate_Mouse ( core::internal_input::InputState* input_state )
{
	input_state->m_rawDeltaMouse = {0, 0};

	for ( int i = 0; i < core::kMBCount; ++i )
	{
		input_state->m_mouseButtons[i].PreUpdate();
	}
}

// Updates the mouse logic for the current frame.
static void InputState_Update_Mouse (
	core::internal_input::InputState* input_state,
	core::Input* input
	)
{
	input_state->m_prevRawDeltaMouse = input_state->m_rawDeltaMouse;

	input_state->m_deltaMouse.x = input_state->m_rawDeltaMouse.x * input->m_mouseSensitivity.x;
	input_state->m_deltaMouse.y = input_state->m_rawDeltaMouse.y * input->m_mouseSensitivity.y;
	
	// offset them real mouse posses, limiting to window
	input_state->m_mouse += input_state->m_deltaMouse.mulComponents(input->m_mouseSensitivity);
	if (core::GetScreenExists())
	{
		input_state->m_mouse.x = math::clamp<Real>(input_state->m_mouse.x, 0.0F, (Real)core::GetFocusedScreen().GetWidth());
		input_state->m_mouse.y = math::clamp<Real>(input_state->m_mouse.y, 0.0F, (Real)core::GetFocusedScreen().GetHeight());
	}

	// update the mouse wheel changes
	if ( !input_state->m_deltaMouseScrollChange )
		input_state->m_deltaMouseScroll = 0;
	input_state->m_deltaMouseScrollChange = false;

	if ( !input_state->m_deltaMouseZoomChange )
		input_state->m_deltaMouseZoom = 0;
	input_state->m_deltaMouseZoomChange = false;

	if ( !input_state->m_deltaMouseHScrollChange )
		input_state->m_deltaMouseHScroll = 0;
	input_state->m_deltaMouseHScrollChange = false;

	if ( !input_state->m_deltaMouseHZoomChange )
		input_state->m_deltaMouseHZoom = 0;
	input_state->m_deltaMouseHZoomChange = false;

	// If we need to sync up the system mouse tho, we will need to override some values
	if (input_state->m_syncRawAndSystemMouse)
	{
		input_state->m_mouse = {(Real)input_state->m_sysMouse.x, (Real)input_state->m_sysMouse.y};
	}
}

//===============================================================================================//

// Clears out keyboard state for next frame.
static void InputState_PreUpdate_Keyboard ( core::internal_input::InputState* input_state )
{
	for ( int i = 0; i < core::kVkCount; ++i )
	{
		input_state->m_keys[i].PreUpdate();
	}

	input_state->m_keypressesLastFrame.clear();
	input_state->m_stringLastFrame.clear();
}

// Takes keypresses and converts them to an input string
static void InputState_Update_Keyboard ( core::internal_input::InputState* input_state )
{
	// loop thru the inputs

	// are we currently in Shift state?
	bool bInShiftState = input_state->m_keys[core::kVkShift].prev;
	for (const auto& key : input_state->m_keypressesLastFrame)
	{
		if (key.code == core::kVkShift)
		{
			bInShiftState = key.isMake;
			continue;
		}

		auto& stringLastFrame = input_state->m_stringLastFrame;

		// Start with simple Ascii mappings.
		unsigned char mappedAscii = core::VkToAsciiTable[key.code];
		if (mappedAscii != 0 && key.isMake)
		{
			// Do not shift numpad inputs. Push their direct mappings.
			if ( ( key.code >= core::kVkNumpad0 && key.code <= core::kVkNumpad9 ) 
				|| ( key.code == core::kVkNumpadAdd || key.code == core::kVkNumpadSubtract
					|| key.code == core::kVkNumpadMultiply || key.code == core::kVkNumpadDivide || key.code == core::kVkNumpadDecimal )
				)
			{
				stringLastFrame += mappedAscii;
				continue;
			}

			if (!bInShiftState)
			{
				if (mappedAscii >= 'A' && mappedAscii <= 'Z')
				{
					stringLastFrame += 'a' + (mappedAscii - 'A');
					continue;
				}

				// Everything else we throw in unmodified.
				if (mappedAscii)
				{
					stringLastFrame += mappedAscii;
				}
				continue;
			}
			else
			{
				if (mappedAscii >= 'A' && mappedAscii <= 'Z')
				{
					stringLastFrame += mappedAscii;
					continue;
				}

				// US keyboard character mappings.
				switch ( mappedAscii )
				{
					case '1': stringLastFrame += '!'; break;
					case '2': stringLastFrame += '@'; break;
					case '3': stringLastFrame += '#'; break;
					case '4': stringLastFrame += '$'; break;
					case '5': stringLastFrame += '%'; break;
					case '6': stringLastFrame += '^'; break;
					case '7': stringLastFrame += '&'; break;
					case '8': stringLastFrame += '*'; break;
					case '9': stringLastFrame += '('; break;
					case '0': stringLastFrame += ')'; break;

					case '[': stringLastFrame += '{'; break;
					case ']': stringLastFrame += '}'; break;
					case '\\': stringLastFrame += '|'; break;
					case ';': stringLastFrame += ':'; break;
					case '\'': stringLastFrame += '"'; break;
					case ',': stringLastFrame += '<'; break;
					case '.': stringLastFrame += '>'; break;
					case '/': stringLastFrame += '?'; break;

					case '-': stringLastFrame += '_'; break;
					case '=': stringLastFrame += '+'; break;
					case '+': stringLastFrame += '+'; break;

					default: 
					{
						if (mappedAscii)
						{
							stringLastFrame += mappedAscii; break;
						}
					}
				}

				continue;
			}
		}

		// End loop thru keypresses
	}
}

//===============================================================================================//
// Accessors (Keyboard)

//	Key(virtual_keycode) : Checks if the key is currently down.
bool core::Input::Key ( const unsigned char vkeycode_ascii, const int windowIndex )
{
	ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_keys[vkeycode_ascii].key;
}
//	Keydown(virtual_keycode) : Checks if the key was pressed in the previous frame.
bool core::Input::Keydown ( const unsigned char vkeycode_ascii, const int windowIndex )
{
	ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_keys[vkeycode_ascii].down;
}
//	Keydown(virtual_keycode) : Checks if the key was released in the previous frame.
bool core::Input::Keyup ( const unsigned char vkeycode_ascii, const int windowIndex )
{
	ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_keys[vkeycode_ascii].up;
}

//	KeypressAny() : Checks if any key is currently down.
bool core::Input::KeypressAny ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	for (int keyIndex = 0; keyIndex < kVkCount; ++keyIndex)
	{
		if (g_InputStatesPerWindow[state_index].m_keys[keyIndex].key) return true;
	}
	return false;
}
//	KeydownAny() : Checks if any key was pressed in the previous frame.
bool core::Input::KeydownAny ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	for (int keyIndex = 0; keyIndex < kVkCount; ++keyIndex)
	{
		if (g_InputStatesPerWindow[state_index].m_keys[keyIndex].down) return true;
	}
	return false;
}
//	KeyupAny() : Checks if any key was released in the previous frame.
bool core::Input::KeyupAny ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	for (int keyIndex = 0; keyIndex < kVkCount; ++keyIndex)
	{
		if (g_InputStatesPerWindow[state_index].m_keys[keyIndex].up) return true;
	}
	return false;
}

//	FrameInputString() : Returns the sum of all the inputs over the past frame.
const std::string& core::Input::FrameInputString ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_stringLastFrame;
}

//===============================================================================================//
// Accessors (Mouse)

//	MouseX() : Current virtual mouse cursor X position
float core::Input::MouseX ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_mouse.x;
}
//	MouseY() : Current virtual mouse cursor Y position
float core::Input::MouseY ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_mouse.y;
}

//	SysMouseX() : Current local system mouse cursor X position
int core::Input::SysMouseX ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_sysMouse.x;
}
//	SysMouseY() : Current local system mouse cursor X position
int core::Input::SysMouseY ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_sysMouse.y;
}

//	DeltaMouseX() : Current virtual mouse cursor X delta
float core::Input::DeltaMouseX ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_deltaMouse.x;
}
//	DeltaMouseY() : Current virtual mouse cursor Y delta
float core::Input::DeltaMouseY ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_deltaMouse.y;
}
//	DeltaMouseScroll() : Current virtual mouse wheel delta.
int core::Input::DeltaMouseScroll ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_deltaMouseScroll;
}
//	DeltaMouseHScroll() : Current virtual mouse wheel delta.
int core::Input::DeltaMouseHScroll ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_deltaMouseHScroll;
}
//	DeltaMouseZoom() : Current virtual mouse wheel zoom.
int core::Input::DeltaMouseZoom ( const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_deltaMouseZoom + g_InputStatesPerWindow[state_index].m_deltaMouseHZoom;
}

//	Mouse(mousebutton) : Checks if the mouse button is currently down.
bool core::Input::Mouse ( const int mousebutton_id, const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_mouseButtons[mousebutton_id].key;
}
//	MouseDown(mousebutton) : Checks if the mouse button was pressed in the previous frame.
bool core::Input::MouseDown ( const int mousebutton_id, const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_mouseButtons[mousebutton_id].down;
}
//	MouseUp(mousebutton) : Checks if the mouse button was released in the previous frame.
bool core::Input::MouseUp ( const int mousebutton_id, const int windowIndex )
{
	const int state_index = (windowIndex == -1) ? m_Active->m_currentActiveWindow : windowIndex;
	return g_InputStatesPerWindow[state_index].m_mouseButtons[mousebutton_id].up;
}

//===============================================================================================//

ARSINGLETON_CPP_DEF(core::Input);

void core::Input::Initialize ( void )
{
	// Ensure that g_InputStatesPerWindow has one state
	if (g_InputStatesPerWindow.empty())
	{
		g_InputStatesPerWindow.push_back(core::internal_input::InputState());
	}

	Active();
	ARCORE_ASSERT(Active() != NULL);
	
	InitializeVkToAsciiTable();

	Reset();

	SetMouseSensitivity( 1.0F );
}

void core::Input::Reset ( void )
{
	for ( auto& inputState : g_InputStatesPerWindow )
	{
		InputState_Reset( &inputState );
	}
}

void core::Input::ResizeInputState ( int input_state_count )
{
	g_InputStatesPerWindow.resize( std::max<int>( 1, input_state_count ) );
}

void core::Input::Free ( void )
{
	FreeInstance();
}

void core::Input::PreUpdate ( void )
{
	for ( auto& inputState : g_InputStatesPerWindow )
	{
		InputState_PreUpdate_Keyboard( &inputState );
		InputState_PreUpdate_Mouse( &inputState );
	}
}

void core::Input::Update ( void )
{
	for ( auto& inputState : g_InputStatesPerWindow )
	{
		InputState_Update_Keyboard( &inputState );
		InputState_Update_Mouse( &inputState, m_Active );
	}
}

//===============================================================================================//
// State setters from RrWindow

void core::Input::WSetKeyMake ( const unsigned char vkeycode_ascii, const int windowIndex )
{
	ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
	g_InputStatesPerWindow[windowIndex].m_keys[vkeycode_ascii].Make();
	g_InputStatesPerWindow[windowIndex].m_keypressesLastFrame.push_back({vkeycode_ascii, true});
}
void core::Input::WSetKeyBreak ( const unsigned char vkeycode_ascii, const int windowIndex )
{
	ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
	g_InputStatesPerWindow[windowIndex].m_keys[vkeycode_ascii].Break();
	g_InputStatesPerWindow[windowIndex].m_keypressesLastFrame.push_back({vkeycode_ascii, false});
}


void core::Input::WSetSysMouse ( const Vector2i mouse, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_sysMouse = mouse;
	if (g_InputStatesPerWindow[windowIndex].m_syncRawAndSystemMouse)
	{
		g_InputStatesPerWindow[windowIndex].m_mouse = Vector2f((Real)mouse.x, (Real)mouse.y);
	}
}
void core::Input::WAddRawMouse ( const Vector2i deltamouse, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_rawDeltaMouse += deltamouse;
}
void core::Input::WSetCurrMouseScroll ( const int mousew, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_deltaMouseScrollChange = true;
	g_InputStatesPerWindow[windowIndex].m_deltaMouseScroll = mousew;
}
void core::Input::WSetCurrMouseZoom ( const int mousew, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_deltaMouseZoomChange = true;
	g_InputStatesPerWindow[windowIndex].m_deltaMouseZoom = mousew;
}
void core::Input::WSetCurrMouseHScroll ( const int mousew, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_deltaMouseHScrollChange = true;
	g_InputStatesPerWindow[windowIndex].m_deltaMouseHScroll = mousew;
}
void core::Input::WSetCurrMouseHZoom ( const int mousew, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_deltaMouseHZoomChange = true;
	g_InputStatesPerWindow[windowIndex].m_deltaMouseHZoom = mousew;
}

void core::Input::WSetMouseMake ( const int mousebutton_id, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_mouseButtons[mousebutton_id].Make();
}
void core::Input::WSetMouseBreak ( const int mousebutton_id, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_mouseButtons[mousebutton_id].Break();
}

void core::Input::WSetSyncRawAndSystemMouse ( const bool sync, const int windowIndex )
{
	g_InputStatesPerWindow[windowIndex].m_syncRawAndSystemMouse = sync;
}

//===============================================================================================//
