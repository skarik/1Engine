#ifndef UI_EVENTIDE_ELEMENTS_BUTTON_H_
#define UI_EVENTIDE_ELEMENTS_BUTTON_H_

#include <atomic>
#include "m04/eventide/Element.h"

namespace ui {
namespace eventide {
namespace elements {

	class Button : public ui::eventide::Element
	{
	public:
		EVENTIDE_API			Button ( ui::eventide::UserInterface* ui = NULL );
		EVENTIDE_API			~Button ( void );

		EVENTIDE_API virtual void
								BuildMesh ( void ) override;
		EVENTIDE_API virtual void
								OnEventMouse ( const EventMouse& mouse_event ) override;
		EVENTIDE_API virtual void
								OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

		// Label displayed within the element.
		std::string				m_contents;
		
		//	SetEnabled(enable) : Sets if the button is enabled.
		// A disabled button will not receive focus nor events.
		EVENTIDE_API void		SetEnabled ( bool enable );
		EVENTIDE_API bool		GetEnabled ( void )
			{ return m_enabled; }

	protected:

		//	OnActivated() : Overridable action for on-activation.
		EVENTIDE_API virtual void
								OnActivated ( void )
			{}

		// If the button can be activated or focused.
		bool				m_enabled = true;

	private:
		ui::eventide::Texture
							m_fontTexture;
		std::atomic<float>	m_hoverGlowValue = 0;
		std::atomic<float>	m_activateGlowPulse = 0;
	};

}}}

#endif//UI_EVENTIDE_ELEMENTS_BUTTON_H_