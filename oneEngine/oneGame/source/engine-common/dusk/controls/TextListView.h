#ifndef ENGINE_COMMON_DUSK_UI_ELEMENT_TEXT_LIST_VIEW_H_
#define ENGINE_COMMON_DUSK_UI_ELEMENT_TEXT_LIST_VIEW_H_

#include "engine-common/dusk/Element.h"

#include <functional>

namespace dusk {
namespace elements {

	class TextListView : public dusk::Element
	{
	public:
		ENGCOM_API explicit		TextListView();

		ENGCOM_API				~TextListView (void);

		//	Update() : Called every frame by the UI system.
		void					Update ( const UIStepInfo* stepinfo ) override;
		
		//	Render() : Renders the element.
		void					Render ( UIRendererContext* uir ) override;

		void					ClearMenuEntries ( void );
		void					AddMenuEntry ( const std::string& menuEntry )
		{
			AddMenuEntry(menuEntry.c_str());
		}
		void					AddMenuEntry ( const char* menuEntry );


	public:

		typedef std::function<void(TextListView*, int)> TextListViewCallback;

		ENGCOM_API void			SetOnClickEntry ( TextListViewCallback&& Fn )
		{
			m_onClickEntry = Fn;
		}

		ENGCOM_API void			SetOnDoubleClickEntry ( TextListViewCallback&& Fn )
		{
			m_onDoubleClickEntry = Fn;
		}

	protected:

		TextListViewCallback
							m_onClickEntry = nullptr;
		TextListViewCallback
							m_onDoubleClickEntry = nullptr;

		struct MenuEntry
		{
			std::string			contents;
			bool				hovered = false;
			bool				selected = false;
			Rect				rect; // Updated on draw.
		};

		// List of all entries in the menu
		std::vector<MenuEntry>
							m_entries;

	};

}}

#endif//ENGINE_COMMON_DUSK_UI_ELEMENT_TEXT_LIST_VIEW_H_