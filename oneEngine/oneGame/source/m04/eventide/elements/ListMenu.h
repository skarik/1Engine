#ifndef UI_EVENTIDE_ELEMENTS_LIST_MENU_H
#define UI_EVENTIDE_ELEMENTS_LIST_MENU_H

#include <atomic>
#include <vector>
#include "m04/eventide/Element.h"
#include "./Button.h"

namespace ui {
namespace eventide {
namespace elements {

	class ListMenu;
	class ListMenuButton;
	
	class ListMenuButton : public ui::eventide::elements::Button
	{
	public:
		EVENTIDE_API			ListMenuButton ( ListMenu* listMenu, int choiceIndex, ui::eventide::UserInterface* ui = NULL )
			: ui::eventide::elements::Button(ui)
			, m_listMenu(listMenu)
			, m_choiceIndex(choiceIndex)
		{
		}
		EVENTIDE_API			~ListMenuButton ( void ) {}

	protected:
		//	OnActivated() : Overridable action for on-activation.
		EVENTIDE_API virtual void
								OnActivated ( void ) override;

		ListMenu*			m_listMenu = NULL;
		int					m_choiceIndex = 0;
	};

	class ListMenu : public ui::eventide::Element
	{
	public:
		EVENTIDE_API			ListMenu ( ui::eventide::UserInterface* ui = NULL );
		EVENTIDE_API			~ListMenu ( void );

		EVENTIDE_API virtual void
								BuildMesh ( void ) override;
		EVENTIDE_API virtual void
								OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

		//	SetListChoices( choices ) : Sets the list choices, and marks list dirty.
		// Dirty lists have visuals regenerated before the end of the following frame.
		EVENTIDE_API void		SetListChoices ( const std::vector<std::string>& choices );
		//	GetListChoicesForWrite(): Returns reference to the choices, and marks list dirty.
		// Dirty lists have visuals regenerated before the end of the following frame.
		EVENTIDE_API std::vector<std::string>&
								GetListChoicesForWrite ( void );
		//	GetListChoices() : Returns list of the current choices.
		EVENTIDE_API const std::vector<std::string>&
								GetListChoices ( void ) const
			{ return m_choices; }

		//	SetEnabled(enable) : Sets if the button is enabled.
		// A disabled button will not receive focus nor events.
		EVENTIDE_API void		SetEnabled ( bool enable );
		EVENTIDE_API bool		GetEnabled ( void )
			{ return m_enabled; }

	protected:
		friend ListMenuButton;

		//	OnActivated() : Overridable action for on-activation.
		EVENTIDE_API virtual void
								OnActivated ( int choiceIndex )
			{}

		// If the button can be activated or focused.
		bool				m_enabled = true;

	private:

		// List of choices
		std::vector<std::string>
							m_choices;
		// List of choice buttons
		std::vector<Button*>
							m_choiceButtons;
	};

}}}

#endif//UI_EVENTIDE_ELEMENTS_LIST_MENU_H