//===============================================================================================//
//
//		CInput - Input backend
//
// Input backend. Forwards input that is captured from the OS's message loop.
// On Windows, mouse input is hardware based. Unless configured, it does not correspond to the
// system cursor.
//
//===============================================================================================//
#ifndef CORE_INPUT_H_
#define CORE_INPUT_H_

#include "core/types.h"
#include "core/containers/arsingleton.h"
#include "core/math/Vector2.h"
#include "core/math/vect2d_template.h"
#include "core/debug.h"

#include <vector>
#include <array>

namespace core
{
	enum VirtualKey : int
	{
		kVkCount = 256,

		kVkBack			= 0x08,
		kVkBackspace	= 0x08,
		kVkTab			= 0x09,
		kVkReturn		= 0x0D,
		kVkEscape		= 0x1B,

		kVkSpace		= 0x20,
		kVkPageUp		= 0x21,
		kVkPageDown		= 0x22,
		kVkEnd			= 0x23,
		kVkHome			= 0x24,
		kVkPrintScreen	= 0x2C,
		kVkInsert		= 0x2D,
		kVkDelete		= 0x2E,

		kVkCapital		= 0x14,
		kVkCapslock		= 0x14,

		kVkShift		= 0x10,
		kVkControl		= 0x11,
		kVkAlt			= 0x12,
		kVkPause		= 0x13,

		kVkLeft			= 0x21,
		kVkUp			= 0x22,
		kVkRight		= 0x23,
		kVkDown			= 0x24,

		kVkNumpad0		= 0x60,
		kVkNumpad1		= 0x61,
		kVkNumpad2		= 0x62,
		kVkNumpad3		= 0x63,
		kVkNumpad4		= 0x64,
		kVkNumpad5		= 0x65,
		kVkNumpad6		= 0x66,
		kVkNumpad7		= 0x67,
		kVkNumpad8		= 0x68,
		kVkNumpad9		= 0x69,
		kVkNumpadMultiply	= 0x6A,
		kVkNumpadAdd	= 0x6B,
		kVkNumpadSubtract	= 0x6D,
		kVkNumpadDivide	= 0x6F,
		kVkNumpadDecimal= 0x6E,

		kVkTilde		= 0xC0,

		kVkF1			= 0x70,
		kVkF2			= 0x71,
		kVkF3			= 0x72,
		kVkF4			= 0x73,
		kVkF5			= 0x74,
		kVkF6			= 0x75,
		kVkF7			= 0x76,
		kVkF8			= 0x77,
		kVkF9			= 0x78,
		kVkF10			= 0x79,
		kVkF11			= 0x7A,
		kVkF12			= 0x7B,
	};

	enum VirtualMouseButton : int
	{
		kMBLeft			= 0,
		kMBRight		= 1,
		kMBMiddle		= 2,
		kMBBackward		= 3,
		kMBForward		= 4,

		kMBCount = 16,
	};

	CORE_API extern unsigned char VkToAsciiTable [256];
	extern void InitializeVkToAsciiTable ( void );

	class Input
	{
		ARSINGLETON_H_STORAGE(Input,CORE_API)
		ARSINGLETON_H_ACCESS(Input)

	private:
		struct /*alignas(1)*/ KeyState // TODO: Fix alignment issue
		{
			uint	key : 1;
			uint	prev : 1;
			uint	down : 1;
			uint	up : 1;

			KeyState()
				: key(false)
				, prev(false)
				, down(false)
				, up(false)
			{
			}

			void PreUpdate ( void )
			{
				prev = key;
				down = false;
				up = false;
			}

			void Make ( void )
			{
				down = true;
				key = true;
			}

			void Break ( void )
			{
				up = true;
				key = false;
			}
		};

	private:
		explicit				Input ( void ) {}
								~Input ( void ) {}

	public:
		//	Initialize() : Initializes input system, allocating memory.
		CORE_API static void	Initialize ( void );
		//	Free() : Frees input system.
		CORE_API static void	Free ( void );

		//	PreUpdate() : Resets the input system just before message loop updates.
		CORE_API static void	PreUpdate ( void )
		{
			m_Active->PreupdateKeyboard();
			m_Active->PreupdateMouse();
		}
		//	Update() : Updates the input logic after the message loop updates.
		CORE_API static void	Update ( void )
		{
			m_Active->UpdateKeyboard();
			m_Active->UpdateMouse();
		}
		//	Reset() : Zeroes out all the current input
		CORE_API static void	Reset ( void );

	public:

		//	Key(virtual_keycode) : Checks if the key is currently down.
		CORE_API static bool	Key ( const unsigned char vkeycode_ascii )
		{
			ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
			return m_Active->m_keys[vkeycode_ascii].key;
		}
		//	Keydown(virtual_keycode) : Checks if the key was pressed in the previous frame.
		CORE_API static bool	Keydown ( const unsigned char vkeycode_ascii )
		{
			ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
			return m_Active->m_keys[vkeycode_ascii].down;
		}
		//	Keydown(virtual_keycode) : Checks if the key was released in the previous frame.
		CORE_API static bool	Keyup ( const unsigned char vkeycode_ascii )
		{
			ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
			return m_Active->m_keys[vkeycode_ascii].up;
		}

		//	KeypressAny() : Checks if any key is currently down.
		CORE_API static bool	KeypressAny ( void )
		{
			for (int keyIndex = 0; keyIndex < kVkCount; ++keyIndex)
			{
				if (m_Active->m_keys[keyIndex].key) return true;
			}
			return false;
		}
		//	KeydownAny() : Checks if any key was pressed in the previous frame.
		CORE_API static bool	KeydownAny ( void )
		{
			for (int keyIndex = 0; keyIndex < kVkCount; ++keyIndex)
			{
				if (m_Active->m_keys[keyIndex].down) return true;
			}
			return false;
		}
		//	KeyupAny() : Checks if any key was released in the previous frame.
		CORE_API static bool	KeyupAny ( void )
		{
			for (int keyIndex = 0; keyIndex < kVkCount; ++keyIndex)
			{
				if (m_Active->m_keys[keyIndex].up) return true;
			}
			return false;
		}

		//	FrameInputString() : Returns the sum of all the inputs over the past frame.
		CORE_API static const std::string&
								FrameInputString ( void )
		{
			return m_Active->m_stringLastFrame;
		}

	public:

		static inline void		WSetKeyMake ( const unsigned char vkeycode_ascii )
		{
			ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
			m_Active->m_keys[vkeycode_ascii].Make();
			m_Active->m_keypressesLastFrame.push_back({vkeycode_ascii, true});
		}
		static inline void		WSetKeyBreak ( const unsigned char vkeycode_ascii )
		{
			ARCORE_ASSERT(vkeycode_ascii > 0 && vkeycode_ascii < kVkCount);
			m_Active->m_keys[vkeycode_ascii].Break();
			m_Active->m_keypressesLastFrame.push_back({vkeycode_ascii, false});
		}

	private:

		// Clears out keyboard state for next frame.
		void					PreupdateKeyboard ( void );
		// Takes keypresses and converts them to an input string
		void					UpdateKeyboard ( void );

		struct KeypressEntry
		{
			uint	code : 8;
			uint	isMake : 1;
		};
		std::vector<KeypressEntry>
							m_keypressesLastFrame;
		std::string			m_stringLastFrame;
		std::array<KeyState, kVkCount>
							m_keys;

	public:

		//	MouseX() : Current virtual mouse cursor X position
		CORE_API static float	MouseX ( void )
		{
			return m_Active->m_mouse.x;
		}
		//	MouseY() : Current virtual mouse cursor Y position
		CORE_API static float	MouseY ( void )
		{
			return m_Active->m_mouse.y;
		}

		//	SysMouseX() : Current local system mouse cursor X position
		CORE_API static int		SysMouseX ( void )
		{
			return m_Active->m_sysMouse.x;
		}
		//	SysMouseY() : Current local system mouse cursor X position
		CORE_API static int		SysMouseY ( void )
		{
			return m_Active->m_sysMouse.y;
		}

		//	DeltaMouseX() : Current virtual mouse cursor X delta
		CORE_API static float	DeltaMouseX ( void )
		{
			return m_Active->m_deltaMouse.x;
		}
		//	DeltaMouseY() : Current virtual mouse cursor Y delta
		CORE_API static float	DeltaMouseY ( void )
		{
			return m_Active->m_deltaMouse.y;
		}
		//	DeltaMouseW() : Current virtual mouse wheel delta.
		CORE_API static int		DeltaMouseW ( void )
		{
			return m_Active->m_deltaMouseW;
		}

		//	Mouse(mousebutton) : Checks if the mouse button is currently down.
		CORE_API static bool	Mouse ( const int mousebutton_id )
		{
			return m_Active->m_mouseButtons[mousebutton_id].key;
		}
		//	MouseDown(mousebutton) : Checks if the mouse button was pressed in the previous frame.
		CORE_API static bool	MouseDown ( const int mousebutton_id )
		{
			return m_Active->m_mouseButtons[mousebutton_id].down;
		}
		//	MouseUp(mousebutton) : Checks if the mouse button was released in the previous frame.
		CORE_API static bool	MouseUp ( const int mousebutton_id )
		{
			return m_Active->m_mouseButtons[mousebutton_id].up;
		}

		//	SetMouseSensitivity(sensitivity) : Sets virtual mouse sensitivity.
		CORE_API static void	SetMouseSensitivity ( const Real sensitivity )
		{
			m_Active->m_mouseSensitivity = {sensitivity, sensitivity};
		}
		//	GetMouseSensitivity() : Returns current mouse sensitivity
		CORE_API static Real	GetMouseSensitivity ( void )
		{
			return m_Active->m_mouseSensitivity.x;
		}

	public:

		static inline void		WSetSysMouse ( const Vector2i mouse )
		{
			m_Active->m_sysMouse = mouse;
			if (m_Active->m_syncRawAndSystemMouse)
			{
				m_Active->m_mouse = Vector2f((Real)mouse.x, (Real)mouse.y);
			}
		}
		static inline void		WAddRawMouse ( const Vector2i deltamouse )
		{
			m_Active->m_rawDeltaMouse += deltamouse;
		}
		static inline void		WSetCurrMouseW ( const int mousew )
		{
			m_Active->m_mouseW = mousew;
		}

		static inline void		WSetMouseMake ( const int mousebutton_id )
		{
			m_Active->m_mouseButtons[mousebutton_id].Make();
		}
		static inline void		WSetMouseBreak ( const int mousebutton_id )
		{
			m_Active->m_mouseButtons[mousebutton_id].Break();
		}

		static inline void		WSetSyncRawAndSystemMouse ( const bool sync )
		{
			m_Active->m_syncRawAndSystemMouse = sync;
		}

	private:

		// Clears out mouse state for next frame.
		void					PreupdateMouse ( void );
		// Updates the mouse logic for the current frame.
		void					UpdateMouse ( void );


		Vector2f			m_mouse;
		Vector2i			m_sysMouse;
		Vector2f			m_deltaMouse;
		Vector2i			m_rawDeltaMouse;
		Vector2i			m_prevRawDeltaMouse;
		Vector2f			m_mouseSensitivity;
		int					m_mouseW = 0;
		int					m_deltaMouseW = 0;
		int					m_prevMouseW = 0;

		bool				m_syncRawAndSystemMouse = false;

		std::array<KeyState, kMBCount>
							m_mouseButtons;
	};
}

#endif//CORE_INPUT_H_