#ifndef M04_EDITORS_SEQUENCE_EDITOR_RIGHT_CLICK_LIST_MENU_H_
#define M04_EDITORS_SEQUENCE_EDITOR_RIGHT_CLICK_LIST_MENU_H_

#include "m04/eventide/elements/ListMenu.h"

namespace m04 {
namespace editor {
namespace sequence {

	class RightClickListMenu : public ui::eventide::elements::ListMenu
	{
	public:
		explicit				RightClickListMenu ( ui::eventide::UserInterface* ui = NULL );

	protected:
		virtual					~RightClickListMenu ( void );

	public:
		virtual void			OnEventMouse ( const EventMouse& mouse_event ) override;
		virtual void			OnActivated ( int choiceIndex ) override;

	public:
		// Check if losing any focus
		bool				m_losingFocus = false;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_RIGHT_CLICK_LIST_MENU_H_