#ifndef UI_EVENTIDE_ELEMENTS_LIST_MENU_HIERARCHICAL_H
#define UI_EVENTIDE_ELEMENTS_LIST_MENU_HIERARCHICAL_H

#include "m04/eventide/elements/ListMenu.h"
#include <memory>

namespace ui {
namespace eventide {
namespace elements {

	class ListMenuHierarchical;

	// @brief Button class for handling mouse input & rendering
	class ListMenuHierarchicalButton : public ui::eventide::elements::Button
	{
	public:
		EVENTIDE_API			ListMenuHierarchicalButton ( ListMenuHierarchical* listMenu, int choiceIndex, int choiceValue, bool isGroup, ui::eventide::UserInterface* ui = NULL )
			: ui::eventide::elements::Button(ui)
			, m_listMenu(listMenu)
			, m_choiceIndex(choiceIndex)
			, m_choiceValue(choiceValue)
			, m_isGroup(isGroup)
		{
		}

	protected:
		EVENTIDE_API			~ListMenuHierarchicalButton ( void ) {}

	public:
		EVENTIDE_API virtual void
								OnEventMouse ( const EventMouse& mouse_event ) override;

	protected:
		//	OnActivated() : Overridable action for on-activation.
		EVENTIDE_API virtual void
								OnActivated ( void ) override;

		ListMenuHierarchical*
							m_listMenu = NULL;
		int					m_choiceIndex = 0;
		int					m_choiceValue = 0;
		bool				m_isGroup = false;
	};

	// @brief Main class for holding the menu. Meant to be inherited to be used.
	class ListMenuHierarchical : public ui::eventide::elements::ListMenu
	{
	public:
		EVENTIDE_API			ListMenuHierarchical ( ui::eventide::UserInterface* ui = NULL );
		EVENTIDE_API virtual	~ListMenuHierarchical ( void );

		EVENTIDE_API virtual void
								OnGameFrameUpdate ( const GameFrameUpdateInput& input_frame ) override;

		//	SetListChoices( choices ) : Sets the list choices, and marks list dirty.
		// Dirty lists have visuals regenerated before the end of the following frame.
		EVENTIDE_API virtual void
								SetListChoices ( const std::vector<std::string>& choices ) override;

		class HeirarchicalChoice
		{
		public:
			std::string name = "";
			uint8_t choiceIndex = UINT8_MAX;
			std::shared_ptr<std::vector<HeirarchicalChoice>> choices = nullptr;
		};

		//	SetListChoices( choices ) : Sets the list choices, and marks list dirty.
		// Dirty lists have visuals regenerated before the end of the following frame.
		EVENTIDE_API virtual void
								SetListChoices ( const std::vector<HeirarchicalChoice>& choices );

	private:
		friend ListMenuHierarchicalButton;

		// @brief Used by ListMenuHierarchicalButton to toggle visibility based on hover callbacks.
		EVENTIDE_API void		ShowChoiceSubgroup ( int choiceIndex, bool visible );

		struct ChoiceLevelInfo
		{
			uint8_t m_level : 3;
			uint8_t m_childrenCount : 5;
			uint8_t m_index;
		};

		std::vector<ChoiceLevelInfo>
							m_choiceLevels;
	};

}}}

#endif//UI_EVENTIDE_ELEMENTS_LIST_MENU_HIERARCHICAL_H