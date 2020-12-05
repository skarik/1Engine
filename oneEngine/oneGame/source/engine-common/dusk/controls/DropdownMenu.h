#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_DROPDOWN_MENU_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_DROPDOWN_MENU_H_

#include "engine-common/dusk/Element.h"
#include "engine-common/dusk/controls/Button.h"

#include <functional>
#include <string>
#include <vector>

namespace dusk {
namespace elements {

	class DropdownMenu : public dusk::elements::Button
	{
	public:
		ENGCOM_API explicit		DropdownMenu (void);

		ENGCOM_API				~DropdownMenu (void);

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

		//	Add(string, shortcut, hideMenu, function...) : Adds a menu option with a function to be executed.
		template <typename Fn, typename... Args>
		int32_t					Add ( const char* displayString, char shortcut, bool hideMenuOnUse, Fn&& function, Args...args )
		{
			m_entries.push_back(MenuEntry());
			MenuEntry& newEntry = m_entries.back();
			
			newEntry.name = displayString;
			newEntry.hide_menu = hideMenuOnUse;
			newEntry.action = std::bind(function, args...);
			newEntry.shortcut = shortcut;
			newEntry.shortcut_index = newEntry.name.find_first_of(shortcut, 0);

			m_elementsNeedUpdate = true;
			m_sizeNeedsUpdate = true;

			return int32_t(m_entries.size() - 1);
		}

		//	GetElement(menuOption) : Returns element with the given menu index, if it exists.
		ENGCOM_API Element*		GetElement ( int32_t menuOption )
		{
			if (menuOption < m_dropdownMenu.size())
			{
				return m_dropdownMenu[menuOption];
			}
			return NULL;
		}

	protected:
		//	ShowMenu() : Shows the menu elements and toggles relevant state.
		void					ShowMenu();
		//	HideMenu() : Hides the menu elements and toggles relevant state.
		void					HideMenu();

	public:

		// Is the menu currently open?
		bool				m_open = false;

	protected:

		// Does the menu elements need an update? Used to control elements.
		bool				m_elementsNeedUpdate = false;
		// Does the width need an update? Used to control visuals.
		bool				m_sizeNeedsUpdate = false;
		// Currently calculated width of the menu's entries.
		float				m_menuWidth = 100.0F;
		float				m_entryHeight = 16.0F;
		float				m_toplevelWidth = 25.0F;

		struct MenuEntry
		{
			std::string			name;
			char				shortcut		= 0;
			size_t				shortcut_index	= 0;
			bool				hide_menu		= true;
			std::function<void()>
								action			= nullptr;
		};

		// List of all entries in the menu
		std::vector<MenuEntry>
							m_entries;

		// Elements for the dropdown menu
		std::vector<dusk::elements::Button*>
							m_dropdownMenu;

		// TODO: folders/nonactionable submenus

	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_DROPDOWN_MENU_H_