#ifndef UI_EVENTIDE_ELEMENT_H_
#define UI_EVENTIDE_ELEMENT_H_

#include "../eventide/Common.h"

namespace ui {
namespace eventide {

	class UserInterface;

	class Element
	{
	public:
		EVENTIDE_API			Element ( UserInterface* ui = NULL );
		EVENTIDE_API			~Element ( void );

		// basic shape making shit

		// add cube
		// add text
		// add wire cube
		
		// mouse inside event
		// mouse clicked event

		// manager->LoadTexture
		// manager->ReleaseTexture

		EVENTIDE_API const core::math::BoundingBox&
								GetBBox ( void ) const
			{ return m_bbox; }

		EVENTIDE_API void		SetParent ( Element* parent ) const;

		EVENTIDE_API const Element*
								GetParent ( void ) const
			{ return m_parent; }

	private:
		core::math::BoundingBox	m_bbox;
		UserInterface*			m_ui = NULL;

		// Parent element to float position on
		Element*				m_parent = NULL;

		// Is the mouse inside this element? Updated by the UI manager
		bool					m_mouseInside = false;
		// Are we locking the mouse to this element for now?
		bool					m_mouseLocked = false;
	};

}}

#endif//UI_EVENTIDE_ELEMENT_H_