#ifndef M04_EDITORS_SEQUENCE_EDITOR_TOP_MENU_H_
#define M04_EDITORS_SEQUENCE_EDITOR_TOP_MENU_H_

#include "engine-common/dusk/Dusk.h"

namespace m04 {
namespace editor {
	class SequenceEditor;
}}

namespace m04 {
namespace editor {
namespace sequence {

	class TopMenu
	{
	public:
		explicit				TopMenu (dusk::UserInterface* ui, m04::editor::SequenceEditor* editor);
								~TopMenu ( void );

	private:

		dusk::UserInterface*
							dusk_interface;

		m04::editor::SequenceEditor*
							main_editor;

	private:

		void					SaveTest ( void );
		void					LoadTest ( void );

		void					BeginSaveFile ( void );
		dusk::DialogElement*
							savefileDialog = NULL;

		void					SaveFile ( const std::string& filename );

	};

}}}

#endif//M04_EDITORS_SEQUENCE_EDITOR_TOP_MENU_H_