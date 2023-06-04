#ifndef M04_EDITORS_SEQUENCE_EDITOR_RIGHT_CLICK_LIST_MENU_H_
#define M04_EDITORS_SEQUENCE_EDITOR_RIGHT_CLICK_LIST_MENU_H_

#include "m04/eventide/elements/ListMenu.h"
#include <vector>
#include "core/containers/arstring.h"

namespace m04 {
namespace editor {
	class SequenceEditor;
}}

namespace m04 {
namespace editor {
namespace sequence {

	struct BoardNode;

	class RightClickListMenu : public ui::eventide::elements::ListMenu
	{
	public:
		explicit				RightClickListMenu ( SequenceEditor* editor = NULL );

	protected:
		virtual					~RightClickListMenu ( void );

	public:
		virtual void			OnEventMouse ( const EventMouse& mouse_event ) override;
		virtual void			OnActivated ( int choiceIndex ) override;

	public:
		m04::editor::SequenceEditor*
							m_editor = NULL;

		// Check if losing any focus
		bool				m_losingFocus = false;

		enum class Mode
		{
			kEmptyBoard,
			kOnNode,
			kOnConnection,
		};

		Mode				m_mode = Mode::kEmptyBoard;
		BoardNode*			m_targetNode = NULL;
		uint32_t			m_targetConnection = 0;

	protected:
		// Listing of classnames for the right-click menu.
		struct ClassnameEntry
		{
			bool			isExternal = false;
			arstring128		name;
		};

		std::vector<ClassnameEntry>
							m_classnameListing;
	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_RIGHT_CLICK_LIST_MENU_H_