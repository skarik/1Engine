#include "CInput.h"
#include "CXboxController.h"
#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"
#include "core/math/Math.h"

ARSINGLETON_CPP_DEF(core::Input);

void core::Input::Initialize ( void )
{
	Active();
	ARCORE_ASSERT(Active() != NULL);
	
	InitializeVkToAsciiTable();

	Reset();

	SetMouseSensitivity( 1.0F );
}

void core::Input::Reset ( void )
{
	if (m_Active != nullptr)
	{
		for ( int i = 0; i < kVkCount; ++i )
		{
			m_Active->m_keys[i] = KeyState();
		}
		for ( int i = 0; i < kMBCount; ++i )
		{
			m_Active->m_mouseButtons[i] = KeyState();
		}
	}
}

void core::Input::Free ( void )
{
	FreeInstance();
}

void core::Input::PreupdateMouse ( void )
{
	m_rawDeltaMouse = {0, 0};

	for ( int i = 0; i < kMBCount; ++i )
	{
		m_mouseButtons[i].PreUpdate();
	}
}

void core::Input::PreupdateKeyboard ( void )
{
	for ( int i = 0; i < kVkCount; ++i )
	{
		m_keys[i].PreUpdate();
	}

	m_keypressesLastFrame.clear();
	m_stringLastFrame.clear();
}

void core::Input::UpdateMouse ( void )
{
	m_prevRawDeltaMouse = m_rawDeltaMouse;

	m_deltaMouse.x = m_rawDeltaMouse.x * m_mouseSensitivity.x;
	m_deltaMouse.y = m_rawDeltaMouse.y * m_mouseSensitivity.y;
	
	// offset them real mouse posses, limiting to window
	m_mouse += m_deltaMouse.mulComponents(m_mouseSensitivity);
	m_mouse.x = math::clamp<Real>(m_mouse.x, 0.0F, (Real)Screen::Info.width);
	m_mouse.y = math::clamp<Real>(m_mouse.y, 0.0F, (Real)Screen::Info.height);

	// update the mouse wheel changes
	if ( !m_deltaMouseScrollChange )
		m_deltaMouseScroll = 0;
	m_deltaMouseScrollChange = false;

	if ( !m_deltaMouseZoomChange )
		m_deltaMouseZoom = 0;
	m_deltaMouseZoomChange = false;

	// If we need to sync up the system mouse tho, we will need to override some values
	if (m_syncRawAndSystemMouse)
	{
		m_mouse = {(Real)m_sysMouse.x, (Real)m_sysMouse.y};
	}
}

void core::Input::UpdateKeyboard ( void )
{
	// loop thru the inputs

	// are we currently in Shift state?
	bool bInShiftState = m_keys[kVkShift].prev;
	for (const auto& key : m_keypressesLastFrame)
	{
		if (key.code == kVkShift)
		{
			bInShiftState = key.isMake;
			continue;
		}

		// Start with simple Ascii mappings.
		unsigned char mappedAscii = VkToAsciiTable[key.code];
		if (mappedAscii != 0 && key.isMake)
		{
			// Do not shift numpad inputs. Push their direct mappings.
			if ((key.code >= kVkNumpad0 && key.code <= kVkNumpad9)
				|| (key.code == kVkNumpadAdd || key.code == kVkNumpadSubtract || key.code == kVkNumpadMultiply || key.code == kVkNumpadDivide || key.code == kVkNumpadDecimal))
			{
				m_stringLastFrame += mappedAscii;
				continue;
			}

			if (!bInShiftState)
			{
				if (mappedAscii >= 'A' && mappedAscii <= 'Z')
				{
					m_stringLastFrame += 'a' + (mappedAscii - 'A');
					continue;
				}

				// Everything else we throw in unmodified.
				if (mappedAscii)
				{
					m_stringLastFrame += mappedAscii;
				}
				continue;
			}
			else
			{
				if (mappedAscii >= 'A' && mappedAscii <= 'Z')
				{
					m_stringLastFrame += mappedAscii;
					continue;
				}

				// US keyboard character mappings.
				switch ( mappedAscii )
				{
					case '1': m_stringLastFrame += '!'; break;
					case '2': m_stringLastFrame += '@'; break;
					case '3': m_stringLastFrame += '#'; break;
					case '4': m_stringLastFrame += '$'; break;
					case '5': m_stringLastFrame += '%'; break;
					case '6': m_stringLastFrame += '^'; break;
					case '7': m_stringLastFrame += '&'; break;
					case '8': m_stringLastFrame += '*'; break;
					case '9': m_stringLastFrame += '('; break;
					case '0': m_stringLastFrame += ')'; break;

					case '[': m_stringLastFrame += '{'; break;
					case ']': m_stringLastFrame += '}'; break;
					case '\\': m_stringLastFrame += '|'; break;
					case ';': m_stringLastFrame += ':'; break;
					case '\'': m_stringLastFrame += '"'; break;
					case ',': m_stringLastFrame += '<'; break;
					case '.': m_stringLastFrame += '>'; break;
					case '/': m_stringLastFrame += '?'; break;

					case '-': m_stringLastFrame += '_'; break;
					case '=': m_stringLastFrame += '+'; break;
					case '+': m_stringLastFrame += '+'; break;

					default: 
					{
						if (mappedAscii)
						{
							m_stringLastFrame += mappedAscii; break;
						}
					}
				}

				continue;
			}
		}

		// End loop thru keypresses
	}
}
