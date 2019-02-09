#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_BUTTON_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_BUTTON_H_

#include "engine-common/dusk/Element.h"

namespace dusk {
namespace elements {

	class Button : public dusk::Element
	{
	public:
		ENGCOM_API explicit		Button()
			: dusk::Element(), isPressed(false), beginPress(false)
		{}

		//	Update() : Called every frame by the UI system.
		void					Update ( void ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

		ENGCOM_API bool			Pressed ( void )
			{ return m_isEnabled && isPressed; }

	private:
		// Button state
		bool	isPressed;
		bool	beginPress;
	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_BUTTON_H_